// Default empty project template
#include "applicationui.hpp"

#include <bb/cascades/ScreenIdleMode>
#include <bb/cascades/Window>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/TitleBar>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/Page>
#include <bb/cascades/ListView>
#include <bb/cascades/TextField>
#include <bb/cascades/Label>
#include <bb/cascades/Slider>
#include <bb/cascades/DataModel>
#include <bb/cascades/Container>
#include <bb/cascades/GroupDataModel>
#include <bb/cascades/QListDataModel>
#include <bb/cascades/ActionItem>
#include <bb/cascades/OrientationSupport>
#include <bb/cascades/DisplayDirection>
#include <bb/system/SystemToast>
#include <bb/data/JsonDataAccess>

#include <QTimer>
#include <QFile>
#include <QDate>
#include <QDateTime>
#include <QVector>
#include <QDebug>

using namespace bb::cascades;
using namespace bb::system;
using namespace bb::data;
using namespace javelind::bb::pbuddy;

//const QString ApplicationUI::DATA_FILE("/pbuddy-data.json");
//const QString ApplicationUI::READ_DATE_TIME_FORMAT("yyyy-MM-dd hh:mm:ss");
//const QString ApplicationUI::WRITE_DATE_TIME_FORMAT(ApplicationUI::READ_DATE_TIME_FORMAT);
//const QString ApplicationUI::DISPLAY_DATE_TIME_FORMAT("MMMM d yyyy h:mm:ss AP"); // A separate format solely for rendering on the device

/* BBM Functions */

void ApplicationUI::bbmRegistration() {
    // Every application is required to have its own unique UUID. You should
    // keep using the same UUID when you release a new version of your
    // application.
    // Genered from http://www.guidgenerator.com/
    const QUuid uuid(QLatin1String("53707577-1159-45D7-87AB-C961AE320838"));

    // Register with BBM Social Platform (SP)
    RegistrationHandler *registrationHandler = new RegistrationHandler(uuid, this);
    _registrationHandler = registrationHandler;
    // Expose the registrationHandler to QML in order to facilitate dynamic registration (only when needed)
    // This entire function MUST be called before the document root is created, etc.
    _qml->setContextProperty("_registrationHandler", _registrationHandler);

    InviteToDownload *inviteToDownload = new InviteToDownload(registrationHandler->context(), this);
    // Ditto exposing in order to facilitate dynamic invitation to download the app
//    _qml->setContextProperty("_inviteToDownload", inviteToDownload);

    // After successful registration, we need to connect to a slot in the application
    bool res;
    res = QObject::connect(registrationHandler, SIGNAL(registered()), inviteToDownload, SLOT(sendInvite()));
    Q_ASSERT(res);
    Q_UNUSED(res);
}

/* Constructors & Destructors */

ApplicationUI::ApplicationUI(Application *app) : QObject(app)
{
	_app = app;
	_bufferPresentation = 0;

    // Create scene document from main.qml asset and
    // set parent to created document to ensure it exists for the whole application lifetime
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    _qml = qml;

    // Initialize BBM Social Platform registration
    this->bbmRegistration();

    // Register new meta types
    qRegisterMetaType<Presentation*>();
    qRegisterMetaType<PresentationList>();
    qRegisterMetaType<Slide*>();
    qRegisterMetaType<SlideList>();

    // Attach class values to the QML document
    QDeclarativePropertyMap* applicationUIPropertyMap = new QDeclarativePropertyMap;
    applicationUIPropertyMap->insert("DISPLAY_DATE_TIME_FORMAT", QVariant(Util::DISPLAY_DATE_TIME_FORMAT));
    qml->setContextProperty("applicationUIPropertyMap", applicationUIPropertyMap);
    qml->setContextProperty("appUI", this);

    // Create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();
    _root = root;

    // Store a reference to the main page Presentations data model defined in QML
    _dataModel = this->mainDataModel();

	// Create initial list of Presentation objects from the data file
    QString filePath(QDir::homePath() + Util::DATA_FILE);

    if (QFile::exists(filePath)) {
    	qDebug() << QString("File %1 found! Creating presentations list...").arg(filePath);

    	_presentations = Util::getListFromJSON(filePath);
    }
    else {
    	qDebug() << "No data file found. Creating an empty presentations list...";
    	// Nudge the user to create a new presentation
    	SystemToast* toast = new SystemToast(this);
    	toast->setBody("Create a new presentation!");
    	toast->show();

    	_presentations = PresentationList(); // Create empty list
    }
	qDebug() << "List created.";

	// Insert the list of presentations into the data model
    if (!_presentations.isEmpty()) {
		QVariantList presentationList = Util::wrapListToQVarList(_presentations);
		((GroupDataModel*)_dataModel)->insertList(presentationList);
    }

	// Connect signals with slots
	bool res;
	NavigationPane* mRoot = (NavigationPane*)_root; // Need to create this separately in order to properly connect
	res = QObject::connect(mRoot, SIGNAL(topChanged(bb::cascades::Page*)), this, SLOT(goToPage(bb::cascades::Page*)));
	Q_ASSERT(res);
	Q_UNUSED(res);

	// Set created root object as a scene
    app->setScene(root);
}

ApplicationUI::~ApplicationUI() {
	// Save the current list of presentations to a JSON file
	if (!_presentations.isEmpty()) { // A paranoid defense mechanism. I have no idea what it could defend against, but it seems a good idea.
		Util::saveListToJSON(_presentations, QDir::homePath() + Util::DATA_FILE);
	}

	// Free all memory that we are responsible for
	foreach(Presentation* presentation, _presentations) {
		// TODO free all slides
		delete presentation;
	}
}

/* Accessors */

Application* ApplicationUI::app() {
	return _app;
}

AbstractPane* ApplicationUI::root() {
	return _root;
}

DataModel* ApplicationUI::dataModel() {
	return _dataModel;
}

PresentationList ApplicationUI::presentations() {
	return _presentations;
}

/* QML Accessors */

QVariantMap ApplicationUI::activePresentationQML() {
	return Util::wrapToQVarMap(_activePresentation);
}

/* Mutators */


/* Member Functions */

/* Pad zeros to the left of an integer ensuring that it is a n-character string */
QString ApplicationUI::padInt(int number, int n) {
	QString str = QString::number(number);
	int length = str.length();
	if (length < n) {
		for (int i = 0; i < n - length; ++i) {
			str = "0" + str;
		}
	}
	return str;
}

/* Convert an integer time (seconds) to a minutes/seconds representation */
int* ApplicationUI::timeToMinSecs(int time) {
	int* timeRep = new int[2];
	timeRep[0] = time/60;
	timeRep[1] = time%60;
	return timeRep;
}

/* Convert a minutes/seconds time to an integer (seconds) representation */
int ApplicationUI::timeFromMinSecs(int minutes, int seconds) {
	return (minutes*60) + seconds;
}

/* Display a time in minutes/seconds as a string. First, convert integer time to minutes/seconds representation. */
QString ApplicationUI::timeToText(int time) {
	int* timeRep = ApplicationUI::timeToMinSecs(time);
	QString output = QString("%1:%2").arg(timeRep[0]).arg(ApplicationUI::padInt(timeRep[1], 2));
	delete timeRep;
	return output;
}

/* Display a time (already in minutes/seconds representation) as a string. */
QString ApplicationUI::minSecToText(int minutes, int seconds) {
	return QString("%1:%2").arg(minutes).arg(ApplicationUI::padInt(seconds, 2));
}

/* Application Logic */

DataModel* ApplicationUI::mainDataModel() {
	return _root->findChild<GroupDataModel*>("mainDataModel");
}

/* Retrieve the active presentation that is currently being worked on */
Presentation* ApplicationUI::activePresentation() {
	// If the id is -1, we create a new presentation
	if (_root->property("activePresentationID").value<qint64>() == -1) {
		Presentation *presentation = new Presentation();
		SlideList slideList;
		presentation->setSlides(slideList);

		// Append the presentation to the list of presentations
		_presentations.append(presentation);

		return presentation;
	}
	// Else, return the active presentation
	else {
		foreach (Presentation* presentation, _presentations) {
			// The active presentation is 'selected' in the QML via the "activePresentation" property of the root navigation pane
			if (presentation->id() == _root->property("activePresentationID").value<qint64>()) {
				return presentation;
			}
		}
	}
	return 0; // Return NULL if not found
}

/* Delete the active presentation */
void ApplicationUI::deletePresentation() {
	this->deletePresentation(this->activePresentation());
}

/* Delete the specified presentation, both from the raw list as well as from the data model */
void ApplicationUI::deletePresentation(Presentation* presentation) {
	qDebug() << QString("Deleting presentation %1...").arg(presentation->name());

	// Find the respective index in the data model
	GroupDataModel* model = (GroupDataModel*)_dataModel;
	QVariantList indexPath = this->findInDataModel(presentation, model);

	// Delete the presentation from the presentations list
	_presentations.removeOne(presentation);
	delete presentation; // Free the memory TODO Probably should do this in the destructor? Or am I even responsible for it

	qDebug() << "Deleted presentation.";

	// Remove from the data model
	model->removeAt(indexPath);

	qDebug() << "Removed from data model.";


}

/* Initialize the Prepare page */
void ApplicationUI::initializePreparePage(Page* page) {
	qDebug() << "Going to prepare page...";

	// Set the presentation that needs to be prepared (or create a brand new one)
	_activePresentation = this->activePresentation();

	// Create a buffer of the active presentation to store changes cumulatively
	_bufferPresentation = _activePresentation->copy();

	// Fill in the Presentation name and total time fields and slider
	TextField* nameText = page->findChild<TextField*>("nameText");
	nameText->setText(_activePresentation->name());
	Slider* totalTimeSlider = page->findChild<Slider*>("totalTimeSlider");
	int totalTime = _activePresentation->totalTime();
	totalTimeSlider->setValue(totalTime);
	Label* totalTimeLabel = page->findChild<Label*>("totalTimeValueLabel");
	totalTimeLabel->setText(ApplicationUI::timeToText(totalTime));

	// Create a new QVariantListDataModel, fill it with the slides (wrapped as QVariant objects) and set it to the list view in the page
	QVariantListDataModel* dataModel = new QVariantListDataModel();
	QVariantList qVarList = Util::wrapListToQVarList(_activePresentation->slides());
	dataModel->append(qVarList);
	ListView* listView = page->findChild<ListView*>("slideListView");
	listView->setDataModel(dataModel);
	dataModel->setParent(page); // Attaching this to the Page allows smooth destruction, i.e. when the page is destroyed, the model is also destroyed

	// Connect the onTriggered signal of the action buttons to respective slot functions
	ActionItem* resetButton = page->findChild<ActionItem*>("resetButton");
	ActionItem* doneButton = page->findChild<ActionItem*>("doneButton");
	ActionItem* newButton = page->findChild<ActionItem*>("newButton");
	bool res;
	res = QObject::connect(resetButton, SIGNAL(triggered()), this, SLOT(resetPreparedChanges()));
	Q_ASSERT(res);
	res = QObject::connect(doneButton, SIGNAL(triggered()), this, SLOT(commitPreparedChanges()));
	Q_ASSERT(res);
	res = QObject::connect(newButton, SIGNAL(triggered()), this, SLOT(addNewSlide()));
	Q_ASSERT(res);
	Q_UNUSED(res);
}

/* Initialize the Preview page */
void ApplicationUI::initializePreviewPage(Page* page) {
	qDebug() << "Going to preview page...";

	// Set the presentation that needs to be prepared
	_activePresentation = this->activePresentation();

	// Fill in the Presentation name and total time fields
	Label* nameValueLabel = page->findChild<Label*>("nameValueLabel");
	nameValueLabel->setText(_activePresentation->name());
	int totalTime = _activePresentation->totalTime();
	Label* totalTimeLabel = page->findChild<Label*>("totalTimeValueLabel");
	totalTimeLabel->setText(ApplicationUI::timeToText(totalTime));

	// Create a new QVariantListDataModel, fill it with the slides (wrapped as QVariant objects) and set it to the list view in the page
	QVariantListDataModel* dataModel = new QVariantListDataModel();
	QVariantList qVarList = Util::wrapListToQVarList(_activePresentation->slides());
	dataModel->append(qVarList);
	ListView* listView = page->findChild<ListView*>("slideListView");
	listView->setDataModel(dataModel);
	dataModel->setParent(page); // Attaching this to the Page allows smooth destruction, i.e. when the page is destroyed, the model is also destroyed
}

/* Initialize the Perform page */
void ApplicationUI::initializePerformPage(Page* page) {
	qDebug() << "Going to perform page...";

	// Set the presentation that needs to be prepared
	_activePresentation = this->activePresentation();

	// Ensure that the presentation has at least 1 slide for a slideshow to work
	SlideList& slides = _activePresentation->slidesRef();
	if (slides.size() <= 0) {
		SystemToast* toast = new SystemToast(this);
		toast->setBody(tr("There are no slides for this presentation."));
		toast->show();

		// Pop out of the perform page
		((NavigationPane*)_root)->pop();
	}
	else {
		// Get references to required UI elements
		TitleBar* nameUI = page->findChild<TitleBar*>("titleBar");
		Label* titleUI = page->findChild<Label*>("performTitle");
		Label* timeUI = page->findChild<Label*>("performTime");
		Label* elapsedUI = page->findChild<Label*>("performElapsed");
		Label* totalTimeUI = page->findChild<Label*>("performTotalTime");

		// Initialize UI element values/text
		nameUI->setTitle(_activePresentation->name());
		titleUI->setText(slides[0]->title());
		timeUI->setText(ApplicationUI::timeToText(slides[0]->time()));
		elapsedUI->setText(ApplicationUI::timeToText(0));
		totalTimeUI->setText(ApplicationUI::timeToText(_activePresentation->totalTime()));

		// Rotate (and lock) screen to Landscape mode. Provides greater screen real-estate for the slideshow
		OrientationSupport* orientInstance = OrientationSupport::instance();
		orientInstance->setSupportedDisplayOrientation(SupportedDisplayOrientation::DisplayLandscape);
		orientInstance->requestDisplayDirection(DisplayDirection::West);
		orientInstance->setSupportedDisplayOrientation(SupportedDisplayOrientation::CurrentLocked);

		// Ensure that the screen is kept from powering off due to inactivity (important for slideshow)
		_app->mainWindow()->setScreenIdleMode(ScreenIdleMode::KeepAwake);

		// Inform the QML UI that data and device-specific initialization is complete
		emit performInitialized();
	}
}

void ApplicationUI::reinitializeMainPage(Page* page) {
	qDebug() << "Returning to main page...";

	// Reset screen orientation to portrait mode
	OrientationSupport* orientInstance = OrientationSupport::instance();
	orientInstance->setSupportedDisplayOrientation(SupportedDisplayOrientation::DisplayPortrait);
	orientInstance->requestDisplayDirection(DisplayDirection::North);
	orientInstance->setSupportedDisplayOrientation(SupportedDisplayOrientation::CurrentLocked);

	// Reset the idle behaviour to normal (VERY important to save battery power)
	_app->mainWindow()->setScreenIdleMode(ScreenIdleMode::Normal);

	qDebug() << _dataModel;

	Q_UNUSED(page);
}

/* Working with data */



/* Find the index path of the specified presentation in the specified data model according to id
 * (This is needed because the sorted order is different from the raw list order) */
QVariantList ApplicationUI::findInDataModel(Presentation* presentation, DataModel* model) {
	QVariantList indexPath;
	QList<QVariantMap> modelContents = ((GroupDataModel*)model)->toListOfMaps();
	for (int i = 0; i < modelContents.size(); ++i) {
		QVariantMap map = modelContents[i];
		qint64 mapId = map["id"].value<qint64>();
		if (presentation->id() == mapId) {
			indexPath << i;
		}
	}
	return indexPath;
}

/* Update the target presentation in the presentations data model. */
void ApplicationUI::updatePresentationDataModel(Presentation* presentation) {
	// Find the index of the presentation
	GroupDataModel* model = (GroupDataModel*)_dataModel;
	QVariantList indexPath = this->findInDataModel(presentation, model);

	// Update the presentation at the computed index (if the presentation exists in the model)
	if (!indexPath.isEmpty()) {
		// Note: we do not need to check if the new value is different from the old value, since this is handled in the class mutators themselves
		QVariantMap updatedMap = Util::wrapToQVarMap(presentation);
		model->updateItem(indexPath, updatedMap);
	}
	// Else, the presentation does not exist in the model so create a new entry and append it
	else {
		QVariantMap newMap = Util::wrapToQVarMap(presentation);
		model->insert(newMap);
	}
}

/* Slots */

void ApplicationUI::goToPage(bb::cascades::Page* page) {
	Q_ASSERT(page != 0);
	QString pageName = page->objectName();

	if (pageName == "preparePage") {
		this->initializePreparePage(page);
	}
	else if (pageName == "previewPage") {
		this->initializePreviewPage(page);
	}
	else if (pageName == "performPage") {
		this->initializePerformPage(page);
	}
	// Returning to the main page
	else if (pageName == "mainPage") {
		this->reinitializeMainPage(page);
	}
}

/* Commit all buffered changes to the active presentation made in the prepare page. */
void ApplicationUI::commitPreparedChanges() {
	qDebug() << "Committing all changes to the active presentation...";

	// Commit the changes to the actual presentation via the active presentation reference
	_activePresentation->setName(_bufferPresentation->name());
	_activePresentation->setTotalTime(_bufferPresentation->totalTime());

	// The case of committing the slide list is a little more sophisticated...
	SlideList& activeSlideList = _activePresentation->slidesRef();
	int activeSlideListSize = activeSlideList.size();
	SlideList& bufferSlideList = _bufferPresentation->slidesRef();
	int bufferSlideListSize = bufferSlideList.size();
	// If the number of slides changed, commit the entire slide list directly as usual
	if (activeSlideListSize != bufferSlideListSize) {
		_activePresentation->setSlides(_bufferPresentation->slides());
	}
	// else, commit each slide individually
	else {
		for (int i = 0; i < bufferSlideListSize; ++i) {
			activeSlideList[i]->setTitle(bufferSlideList[i]->title());
			activeSlideList[i]->setTime(bufferSlideList[i]->time());
		}
	}

	// Reflect these commits in the presentations data model
	this->updatePresentationDataModel(_activePresentation);

	// Free the buffer
	delete _bufferPresentation;

	qDebug() << "Changes committed.";
}

/* Reset all buffered changes in the prepare page to the existing active presentation information */
void ApplicationUI::resetPreparedChanges() {
	qDebug() << "Resetting all buffered changes to the active presentation...";

	// Free the buffer and re-copy the active presentation
	delete _bufferPresentation;
	_bufferPresentation = _activePresentation->copy();

	// TODO Reflect the changes in the page
	// Update the Presentation name and total time fields and slider with the reset values
	Page* page = _root->findChild<Page*>("preparePage");
	TextField* nameText = page->findChild<TextField*>("nameText");
	nameText->setText(_activePresentation->name());
	Slider* totalTimeSlider = page->findChild<Slider*>("totalTimeSlider");
	int totalTime = _activePresentation->totalTime();
	totalTimeSlider->setValue(totalTime);
	Label* totalTimeLabel = page->findChild<Label*>("totalTimeValueLabel");
	totalTimeLabel->setText(ApplicationUI::timeToText(totalTime));

	// Update the existing data model with the reset slide data (wrapped as a QVariantList)
	ListView* listView = page->findChild<ListView*>("slideListView");
	QVariantListDataModel* dataModel = (QVariantListDataModel*)(listView->dataModel());
	QVariantList qVarList = Util::wrapListToQVarList(_activePresentation->slides());
	dataModel->clear();
	dataModel->append(qVarList);

	qDebug() << "Changes reset.";
}

/* Append a new slide to the slide list of the active presentation */
void ApplicationUI::addNewSlide() {
	qDebug() << "Adding a new slide to the buffer...";

	// Create new empty slide and add it to the active presentation
	Slide* slide = new Slide();
	_bufferPresentation->addSlide(slide);

	// Append the new slide to the slides data model
	Page* page = _root->findChild<Page*>("preparePage");
	ListView* listView = page->findChild<ListView*>("slideListView");
	QVariantListDataModel* dataModel = (QVariantListDataModel*)listView->dataModel();
	// Wrap around a QVariantMap in order for QML to recognize object members
	QVariantMap qVarMap = Util::wrapToQVarMap(slide);
	dataModel->append(QVariant(qVarMap));

	qDebug() << "Buffered new slide. Ready to commit.";
}

/* Delete an existing slide (according to index) from the active presentation */
void ApplicationUI::deleteSlide(int index) {
	qDebug() << QString("Deleting slide %1: %2...").arg(index).arg(_bufferPresentation->slides().at(index)->title());

	// Delete the slide from the raw list
	_bufferPresentation->deleteSlide(index);

	qDebug() << "Buffered slide deletion. Ready to commit.";

	// Also, remove the slide from the slides data model
	Page* page = _root->findChild<Page*>("preparePage");
	ListView* listView = page->findChild<ListView*>("slideListView");
	QVariantListDataModel* dataModel = (QVariantListDataModel*)listView->dataModel();
	dataModel->removeAt(index);

	qDebug() << "Removed from slides data model.";
}

/* Functions for buffering changes */

/* Invoked from QML. */
void ApplicationUI::bufferNameChange(QString name) {
	qDebug() << "Buffering presentation name change...";

	// Buffer the new presentation name
	_bufferPresentation->setName(name);
	_activePresentation->print();
	_bufferPresentation->print();

	qDebug() << "Buffered name change. Ready to commit.";
}

/* Invoked from QML. */
void ApplicationUI::bufferTotalTimeChange(float value) {
	qDebug() << "Buffering presentation total time change...";

	// Buffer the new presentation total time
	_bufferPresentation->setTotalTime((int)value);
	_activePresentation->print();
	_bufferPresentation->print();

	qDebug() << "Buffered total time change. Ready to commit.";
}

/* Invoked from QML. The slide index is passed to know which slide to modify. */
void ApplicationUI::bufferSlideTitleChange(int index, QString title) {
	qDebug() << "Buffering slide title change...";

	qDebug() << QString("SLIDE TITLE RETURNED IS: %1, THE INDEX RETURNED IS %2").arg(title).arg(index);

	// Buffer the new slide title
	SlideList& slides = _bufferPresentation->slidesRef();
	// Fixes bugs where this function is unexpectedly (and erroneously) called (when that happens, the index passed is usually out of bounds)
	if (index < slides.size()) {
		slides[index]->setTitle(title);
		_activePresentation->print();
		_bufferPresentation->print();
	}

	qDebug() << "Buffered slide title change. Ready to commit.";

}

/* Invoked from QML. The slide index is passed to know which slide to modify. */
void ApplicationUI::bufferSlideTimeChange(int index, float value) {

	qDebug() << "Buffering slide time change...";

	qDebug() << QString("SLIDE TIME RETURNED IS: %1, THE INDEX RETURNED IS %2").arg(value).arg(index);

	// Buffer the new slide time
	SlideList& slides = _bufferPresentation->slidesRef();
	// Fixes bugs where this function is unexpectedly (and erroneously) called (when that happens, the index passed is usually out of bounds)
	if (index < slides.size()) {
		slides[index]->setTime(((int)value));
		_activePresentation->print();
		_bufferPresentation->print();
	}

	qDebug() << "Buffered slide time change. Ready to commit.";

}

//////////////// Perform Page



// Memo: One problem with debugging errors is moc errors, since they are not in your code. Usually, you get these errors when not including certain classes explicitly


// TODO Implement time slider auto-correction logic for prepare page
// TODO Maybe convert main page UI to plain list with no buttons. Buttons should be provided at the action bar (same for prepare page)
// TODO Implement the practise functionality!
// TODO Need a restore defaults button in setting to restore all data to initial load state
