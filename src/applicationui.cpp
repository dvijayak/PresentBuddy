// Default empty project template
#include "applicationui.hpp"

#include <bb/cascades/QmlDocument>
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
#include <bb/data/JsonDataAccess>

#include <QTimer>
#include <QDate>
#include <QDateTime>
#include <QVector>
#include <QDebug>

using namespace bb::cascades;
using namespace bb::data;
using namespace bb::javelind;

const QString ApplicationUI::READ_DATE_TIME_FORMAT("yyyy-MM-dd hh:mm:ss");
const QString ApplicationUI::WRITE_DATE_TIME_FORMAT(ApplicationUI::READ_DATE_TIME_FORMAT);
const QString ApplicationUI::DISPLAY_DATE_TIME_FORMAT("MMMM d yyyy h:mm:ss AP"); // A separate format solely for rendering on the device

ApplicationUI::ApplicationUI(Application *app) : QObject(app)
{
	_app = app;
	_bufferPresentation = 0;

    // Create scene document from main.qml asset and
    // set parent to created document to ensure it exists for the whole application lifetime
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    // Register new meta types
    qRegisterMetaType<Presentation*>();
    qRegisterMetaType<PresentationList>();
    qRegisterMetaType<Slide*>();
    qRegisterMetaType<SlideList>();

    // Attach class values to the QML document
    QDeclarativePropertyMap* applicationUIPropertyMap = new QDeclarativePropertyMap;
    applicationUIPropertyMap->insert("READ_DATE_TIME_FORMAT", QVariant(READ_DATE_TIME_FORMAT));
    applicationUIPropertyMap->insert("WRITE_DATE_TIME_FORMAT", QVariant(WRITE_DATE_TIME_FORMAT));
    applicationUIPropertyMap->insert("DISPLAY_DATE_TIME_FORMAT", QVariant(DISPLAY_DATE_TIME_FORMAT));
    qml->setContextProperty("applicationUIPropertyMap", applicationUIPropertyMap);
    qml->setContextProperty("appUI", this);

    // Create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();
    _root = root;

    // Store a reference to the main page Presentations data model defined in QML
    _dataModel = this->getMainDataModel();

	// Create initial list of Presentation objects from the data file
    QString filePath(QDir::currentPath() + "/app/native/assets/presentations.json");
//    QString filePath(QDir::homePath() + "/presentations_save.json");
    // TODO Check if file exists. If not, or if file is empty, then show no presentations
    _presentations = this->getListFromJSON(filePath);

	// Insert the list of presentations into the data model
    QVariantList presentationList = this->wrapListToQVarList(_presentations);
    ((GroupDataModel*)_dataModel)->insertList(presentationList);

	// Connect signals with slots
	bool res;
	NavigationPane* mRoot = (NavigationPane*)_root; // Need to create this separately in order to properly connect
	res = QObject::connect(mRoot, SIGNAL(topChanged(bb::cascades::Page*)), this, SLOT(goToPage(bb::cascades::Page*)));
	Q_ASSERT(res);
//	// Commit changes made to the active presentation. This would occur when the (or any) page was popped, i.e. exited
//	res = QObject::connect(mRoot, SIGNAL(popTransitionEnded(bb::cascades::Page*)), this, SLOT(commitPreparedChanges()));
//	Q_ASSERT(res);

	// The data model must be aware of potential changes to each of its corresponding presentations
//	foreach (Presentation* presentation, _presentations) {
//		QObject::connect(presentation, SIGNAL(presentationChanged(Presentation*)), this, SLOT(updatePresentationDataModel(Presentation*)));
//		Q_ASSERT(res);
//	}
	Q_UNUSED(res);

	// Set created root object as a scene
    app->setScene(root);
}

ApplicationUI::~ApplicationUI() {
	// Save the current list of presentations to a JSON file
	this->saveListToJSON(_presentations, QDir::homePath() + "/presentations_save.json");

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

//QVariantList ApplicationUI::presentationsQML() {
//	return this->wrapListToQVarList(_presentations);
//}

/* Mutators */

/* Store a copy of the original active presentation */
void ApplicationUI::makeCopyActivePresentation() {
	// Free memory (if allocated before) prior to reuse
	if (_activePresentationBeforeChange != 0) {
		delete _activePresentationBeforeChange;
	}

	// Note: QObject does not have a visible copy constructor, thus we need to explicitly copy ourselves
	_activePresentationBeforeChange = new Presentation(_activePresentation->name()
			, _activePresentation->totalTime()
			, _activePresentation->lastModified()
			, _activePresentation->slides());
}

/* Member Functions */

/* Application Logic */

DataModel* ApplicationUI::getMainDataModel() {
	return _root->findChild<GroupDataModel*>("mainDataModel");
}

void ApplicationUI::initializePreparePage(Page* page) {
	qDebug() << "Going to prepare page...";

	// Set the presentation that needs to be prepared
	foreach (Presentation* presentation, _presentations) {
		if (presentation->name() == _root->property("activePresentation").value<QString>()) {
			_activePresentation = presentation;
			break;
		}
	}

	_activePresentation->print();
	// Create a buffer of the active presentation to store changes
	_bufferPresentation = _activePresentation->copy();

	// Fill in the Presentation name and total time fields and slider
	TextField* nameText = page->findChild<TextField*>("nameText");
	nameText->setText(_activePresentation->name());
	Slider* totalTimeSlider = page->findChild<Slider*>("totalTimeSlider");
	int totalTime = _activePresentation->totalTime();
	totalTimeSlider->setValue(totalTime);
	Label* totalTimeLabel = page->findChild<Label*>("totalTimeValueLabel");
	totalTimeLabel->setText(QString("%1:%2").arg((int)(totalTime/60)).arg(totalTime%60));

	// Connect these objects with their respective buffer slots
	bool res;
//	res = QObject::connect(nameText, SIGNAL(textChanged(QString)), this, SLOT(bufferNameChange(QString)));
//	Q_ASSERT(res);
//	res = QObject::connect(totalTimeSlider, SIGNAL(valueChanged(float)), this, SLOT(bufferTotalTimeChange(float)));
//	Q_ASSERT(res);

	// Create a new QVariantListDataModel, fill it with the slides (wrapped as QVariant objects) and set it to the list view in the page
	QVariantListDataModel* dataModel = new QVariantListDataModel();
	QVariantList qVarList = this->wrapListToQVarList(_activePresentation->slides());
	dataModel->append(qVarList);
	ListView* listView = page->findChild<ListView*>("slideListView");
	listView->setDataModel(dataModel);
	dataModel->setParent(page); // Attaching this to the Page allows smooth destruction, i.e. when the page is destroyed, the model is also destroyed

	// Connect the onTriggered signal of the action buttons to respective slot functions
	ActionItem* doneButton = page->findChild<ActionItem*>("doneButton");
	ActionItem* newButton = page->findChild<ActionItem*>("newButton");
	res = QObject::connect(doneButton, SIGNAL(triggered()), this, SLOT(commitPreparedChanges()));
	Q_ASSERT(res);
	res = QObject::connect(newButton, SIGNAL(triggered()), this, SLOT(addNewSlide()));
	Q_ASSERT(res);
	Q_UNUSED(res);
}

void ApplicationUI::testSlot() {
	QList<Container*> slideUIList = _root->findChildren<Container*>("slideListItem");
	qDebug() << QString("There are %1 slides in this view now.").arg(slideUIList.size());
}

void ApplicationUI::initializePerformPage(Page* page) {
	qDebug() << "Going to perform page...";
	Q_UNUSED(page)
}

void ApplicationUI::reinitializeMainPage(Page* page) {
	qDebug() << "Returning to main page...";

//		// TODO If required, clear all slide data models and restore back to original data model
//		if (dynamic_cast<QListDataModel<Slide*> *>(_dataModel) != 0) {
//			delete _dataModel;
//			_dataModel = this->getMainDataModel();
//		}

	qDebug() << _dataModel;

	Q_UNUSED(page);
}

/* Working with data */

/* Retrieves a list of Presentation objects from a QVariantList wrapper */
PresentationList ApplicationUI::unWrapListFromQVarList(QVariantList qVarList) {
	PresentationList list;
	foreach (QVariant wrappedEntry, qVarList) {
		QVariantMap entry = wrappedEntry.value<QVariantMap>();

		// Name of the presentation
		QString name = entry["name"].value<QString>();

		// Time-stamp of the date that the presentation was last modified
		QDateTime lastModified = QDateTime::fromString(entry["dateModified"].value<QString>(), Qt::ISODate);

		// Total time required for the presentation
		QVariantMap time = entry["totalTime"].value<QVariant>().value<QVariantMap>();
		int totalTime = (60*time["minutes"].value<int>()) + time["seconds"].value<int>();

		// Slides of the presentation
		QVariantList qVarSlideList = entry["slides"].value<QVariant>().value<QVariantList>();
		SlideList slideList;
		foreach (QVariant wrappedSlide, qVarSlideList) {
			QVariantMap slideEntry = wrappedSlide.value<QVariantMap>();

			// Title of the slide
			QString title = slideEntry["title"].value<QString>();

			// Time allotted for the slide
			QVariantMap time = slideEntry["time"].value<QVariant>().value<QVariantMap>();
			int slideTime = (60*time["minutes"].value<int>()) + time["seconds"].value<int>();

			Slide* slide = new Slide(title, slideTime);
			slideList.append(slide);
		}

		Presentation* presentation = new Presentation(name, totalTime, lastModified, slideList);
		presentation->print();
		list.append(presentation);
	}

	return list;
}

/* Parse a JSON file, store the data in appropriate structures and return a list of this data.
 * Note: dateModified field must be of the ISO 8601 date/time standard format. */
PresentationList ApplicationUI::getListFromJSON(QString filePath) {
	// Create a JDA object and read the JSON file
	JsonDataAccess jda;
	QVariant wrapList = jda.load(filePath);
	QList<QVariant> fileEntries = wrapList.value<QVariantList>();

	// TODO Error checking - ensuring all required fields are present, and if not, how to deal with them swiftly

	// Create the list of Presentation objects
	PresentationList list = this->unWrapListFromQVarList(fileEntries);
	return list;
}

/* Create a QVariantMap representation of a time object */
QVariantMap ApplicationUI::createTimeQVarMap(int time) {
	QVariantMap timeMap;
	timeMap["minutes"] = QVariant((int)(time/60));
	timeMap["seconds"] = QVariant(time%60);
	return timeMap;
}

/* Wraps a slide around a QVariantMap */
QVariantMap ApplicationUI::wrapToQVarMap(Slide* slide) {
	QVariantMap qVarMap;
	qVarMap["title"] = QVariant(slide->title());
	qVarMap["time"] = this->createTimeQVarMap(slide->time());
//	qVarMap["dateModified"] = QVariant(slide->lastModified());
	return qVarMap;
}


/* Wraps a presentation around a QVariantMap */
QVariantMap ApplicationUI::wrapToQVarMap(Presentation* presentation) {
	QVariantMap qVarMap;
	qVarMap["name"] = QVariant(presentation->name());
	qVarMap["totalTime"] = this->createTimeQVarMap(presentation->totalTime());
	qVarMap["dateModified"] = QVariant(presentation->lastModified());

	// Create a QVariant in turn for each slide object belonging to the presentation, also wrapping as QVariantMap objects
	QVariantList slideQVarList = this->wrapListToQVarList(presentation->slides());
	qVarMap["slides"] = QVariant(slideQVarList);

	return qVarMap;
}

/* Wraps a list of Slide objects around a list of QVariant objects */
QVariantList ApplicationUI::wrapListToQVarList(SlideList list) {
	QVariantList qVarList;
	// Create a QVariant for each slide object that wraps its members as QVariantMap objects
	foreach (Slide* slide, list) {
		QVariantMap qVarMap = this->wrapToQVarMap(slide);

		// Push each Slide QVariantMap object to qVarList
		qVarList.append(QVariant(qVarMap));
	}

	return qVarList;
}

/* Wraps a list of Presentation objects around a list of QVariant objects */
QVariantList ApplicationUI::wrapListToQVarList(PresentationList list) {
	QVariantList qVarList;
	// Create a QVariant for each presentation object that wraps its members as QVariantMap objects
	foreach (Presentation* presentation, list) {
		QVariantMap qVarMap = this->wrapToQVarMap(presentation);

		// Push each Presentation QVariantMap object to qVarList
		qVarList.append(QVariant(qVarMap));
	}

	return qVarList;
}

/* Serialize the specified list of presentations to the specified JSON file.
 * Note: dateModified field is saved using the ISO 8601 date/time standard format. */
void ApplicationUI::saveListToJSON(PresentationList list, QString filePath) {
	// Retrieve the list as a QVariantList
	QVariantList qVarList = this->wrapListToQVarList(list);

	// Create a JDA object and save to the JSON file
	JsonDataAccess jda;
	jda.save(QVariant(qVarList), filePath);
}

/* Update the target presentation in the presentations data model with its corresponding buffer. */
void ApplicationUI::updatePresentationDataModel(Presentation* target, Presentation* buffer) {
	GroupDataModel* model = (GroupDataModel*)_dataModel;

	// Find the index path of the presentation in the data model (this is needed because the sorted order is different from the raw list order)
	QVariantMap targetMap = this->wrapToQVarMap(target);
	QVariantList indexPath = model->find(targetMap);

	qDebug() << "PRESENTATION MAP IS ";
	qDebug() << targetMap;
	qDebug() << "INDEX PATH IS: ";
	qDebug() <<	 indexPath;
	qDebug() << "DATA IS: ";
	qDebug() << model->data(indexPath);

	// Update the presentation at the computed index (if the presentation exists in the model)
	if (!indexPath.isEmpty()) {
		// Note: we do not need to check if the new value is different from the old value, since this is handled in the class mutators themselves
		QVariantMap updatedMap = this->wrapToQVarMap(buffer);
		model->updateItem(indexPath, updatedMap);
	}
}

/* Slots */

void ApplicationUI::goToPage(bb::cascades::Page* page) {
	Q_ASSERT(page != 0);
	QString pageName = page->objectName();

	if (pageName == "preparePage") {
		this->initializePreparePage(page);
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

	// Preserve the original values of the presentation in order to be able to locate it in the data model
	Presentation* target = _activePresentation->copy();

	// Commit the changes to the actual presentation via the active presentation reference
	_activePresentation->setName(_bufferPresentation->name());
	_activePresentation->setTotalTime(_bufferPresentation->totalTime());
	_activePresentation->setSlides(_bufferPresentation->slides());

	// Reflect this commit in the presentations data model
	this->updatePresentationDataModel(target, _bufferPresentation);

	// Free the original copy and the buffer
	delete target;
	delete _bufferPresentation;

	qDebug() << "Changes committed.";
}

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
	QVariantMap qVarMap = this->wrapToQVarMap(slide);
	dataModel->append(QVariant(qVarMap));

	qDebug() << "Buffered new slide. Ready to commit.";
}

/* Functions for buffering changes */

void ApplicationUI::bufferNameChange(QString name) {
	qDebug() << "Buffering presentation name change...";

	// Buffer the new presentation name
	_bufferPresentation->setName(name);
	_activePresentation->print();
	_bufferPresentation->print();

	qDebug() << "Buffered name change. Ready to commit.";
}

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

	SlideList& slides = _bufferPresentation->slidesRef();
	slides[index]->setTitle(title);
	_activePresentation->print();
	_bufferPresentation->print();

	qDebug() << "Buffered slide title change. Ready to commit.";

}

/* Invoked from QML. The slide index is passed to know which slide to modify. */
void ApplicationUI::bufferSlideTimeChange(int index, float value) {

	qDebug() << "Buffering slide time change...";

	// Buffer the new slide time
	SlideList& slides = _bufferPresentation->slidesRef();
	slides[index]->setTime(((int)value));
	_activePresentation->print();
	_bufferPresentation->print();

	qDebug() << "Buffered slide time change. Ready to commit.";

}


// Memo: One problem with debugging errors is moc errors, since they are not in your code. Usually, you get these errors when not including certain classes explicitly


// TODO Need a slide delete button for prepare page
// TODO Need to convert main page UI to plain list with no buttons. Buttons should be provided at the action bar (same for prepare page)
// TODO Need a reset button and functionality for prepare page
// TODO Implement the perform functionality!
// TODO Implement the practise functionality!
// TODO Need a restore defaults button in setting to restore all data to initial load state
