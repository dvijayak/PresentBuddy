// Default empty project template
#include "applicationui.hpp"

#include <bb/cascades/ScreenIdleMode>
#include <bb/cascades/Window>
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
#include <bb/cascades/OrientationSupport>
#include <bb/cascades/DisplayDirection>
#include <bb/data/JsonDataAccess>

#include <QTimer>
#include <QFile>
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
    _qml = qml;

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
    _dataModel = this->mainDataModel();

	// Create initial list of Presentation objects from the data file
//    QString filePath(QDir::currentPath() + "/app/native/assets/presentations.json");
    QString filePath(QDir::homePath() + "/presentations_save.json");
    // TODO Create more generic file name
    if (QFile::exists(filePath)) {
    	qDebug() << QString("File %1 found! Creating presentations list...").arg(filePath);

    	_presentations = this->getListFromJSON(filePath);
    }
    else {
    	qDebug() << "No base file found. Creating an empty presentations list...";

    	_presentations = PresentationList(); // Create empty list
    }
	qDebug() << "List created.";

	// Insert the list of presentations into the data model
    if (!_presentations.isEmpty()) {
		QVariantList presentationList = this->wrapListToQVarList(_presentations);
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

/* QML Accessors */

//QVariantList ApplicationUI::presentationsQML() {
//	return this->wrapListToQVarList(_presentations);
//}

QVariantMap ApplicationUI::activePresentationQML() {
	return this->wrapToQVarMap(_activePresentation);
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
	foreach (Presentation* presentation, _presentations) {
		// The active presentation is 'selected' in the QML via the "activePresentation" property of the root navigation pane
		if (presentation->id() == _root->property("activePresentationID").value<qint64>()) {
			return presentation;
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

void ApplicationUI::initializePreparePage(Page* page) {
	qDebug() << "Going to prepare page...";

	// Set the presentation that needs to be prepared
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
	totalTimeLabel->setText(QString("%1:%2").arg((int)(totalTime/60)).arg(totalTime%60));

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
	bool res;
	res = QObject::connect(doneButton, SIGNAL(triggered()), this, SLOT(commitPreparedChanges()));
	Q_ASSERT(res);
	res = QObject::connect(newButton, SIGNAL(triggered()), this, SLOT(addNewSlide()));
	Q_ASSERT(res);
	Q_UNUSED(res);
}

void ApplicationUI::initializePerformPage(Page* page) {
	qDebug() << "Going to perform page...";

	// Set the presentation that needs to be prepared
	_activePresentation = this->activePresentation();

	// Get references to required UI elements
	Label* nameUI = page->findChild<Label*>("performName");
	Label* titleUI = page->findChild<Label*>("performTitle");
	Label* timeUI = page->findChild<Label*>("performTime");
	Label* elapsedUI = page->findChild<Label*>("performElapsed");
	Label* totalTimeUI = page->findChild<Label*>("performTotalTime");

	// Initialize UI element values/text
	nameUI->setText(_activePresentation->name());
	SlideList& slides = _activePresentation->slidesRef();
	if (slides.size() <= 0) {
		// TODO Peform: Handle situation when there are no slides for the presentation
	}
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

void ApplicationUI::reinitializeMainPage(Page* page) {
	qDebug() << "Returning to main page...";

	// Reset screen orientation to portrait mode
	OrientationSupport* orientInstance = OrientationSupport::instance();
	orientInstance->setSupportedDisplayOrientation(SupportedDisplayOrientation::DisplayPortrait);
	orientInstance->requestDisplayDirection(DisplayDirection::North);
	orientInstance->setSupportedDisplayOrientation(SupportedDisplayOrientation::CurrentLocked);

	// Reset the idle behaviour to normal (VERY important to save battery power)
	_app->mainWindow()->setScreenIdleMode(ScreenIdleMode::KeepAwake);

	qDebug() << _dataModel;

	Q_UNUSED(page);
}

/* Working with data */

/* Retrieves a list of Presentation objects from a QVariantList wrapper */
PresentationList ApplicationUI::unWrapListFromQVarList(QVariantList qVarList) {
	qDebug() << "Extracting presentations from the JSON file...";

	PresentationList list;
	if (!qVarList.isEmpty()) {
		foreach (QVariant wrappedEntry, qVarList) {
			QVariantMap entry = wrappedEntry.value<QVariantMap>();

			// Name of the presentation
			QString name;
			if (entry.contains("name")) {
				name = entry["name"].value<QString>();
				if (name.isEmpty()) { // (Might be irrelevant, but) works well since a default empty string is assigned to key "name" if the key doesn't exist at all (default QMap behaviour)
					qDebug() << "Error: presentation has not been provided a name. Skipping this JSON entry...";

					continue;
				}
			}
			else {
				qDebug() << "Error: presentation has not been provided a Name. Skipping this JSON entry...";

				continue;
			}

			// Time-stamp of the date that the presentation was last modified
			QDateTime lastModified;
			if (entry.contains("dateModified")) {
				lastModified = QDateTime::fromString(entry["dateModified"].value<QString>(), Qt::ISODate);
				if (!lastModified.isValid()) { // Also (might be irrelevant, but) works well since the default QDateTime value is a null date/time, which is invalid as per QDateTime
					qDebug() << "Invalid date/time (most likely bad format - the only format accepted is ISO 8601 standard). Skipping this JSON entry...";

					continue;
				}
			}
			else {
				qDebug() << "Error: presentation has not been provided a Date/Time Time-stamp (format ISO 8601 standard). Skipping this JSON entry...";

				continue;
			}

			// Total time required for the presentation
			int totalTime;
			if (entry.contains("totalTime")) {
				QVariantMap time = entry["totalTime"].value<QVariant>().value<QVariantMap>();
				if (time.contains("minutes")) {
					totalTime = (60*time["minutes"].value<int>());
					if (time.contains("seconds")) {
						totalTime += time["seconds"].value<int>();
					}

					// The total time for the presentation cannot be negative or  0
					if (totalTime <= 0) {
						totalTime = 600; // Defaults to 10 minutes

						qDebug() << "Warning: presentation Total Time is negative or 0. Defaulted to 600 (10 minutes).";
					}
				}
				else {
					totalTime = 600; // Ditto as above

					qDebug() << "Warning: presentation Total Time has not been provided. Defaulted to 600 (10 minutes 0 seconds).";
				}
			}
			else {
				totalTime = 600; // Ditto as above

				qDebug() << "Warning: presentation Total Time has not been provided. Defaulted to 600 (10 minutes 0 seconds).";
			}

			// Slides of the presentation
			SlideList slideList;
			if (entry.contains("slides")) {
				QVariantList qVarSlideList = entry["slides"].value<QVariant>().value<QVariantList>();
				if (!qVarSlideList.isEmpty()) {
					foreach (QVariant wrappedSlide, qVarSlideList) {
						QVariantMap slideEntry = wrappedSlide.value<QVariantMap>();

						// Title of the slide
						QString title;
						if (slideEntry.contains("title")) {
							title = slideEntry["title"].value<QString>();
							if (title.isEmpty()) {
								title = "Untitled";

								qDebug() << "Warning: slide Title is empty/invalid. Defaulted to \"Untitled\".";
							}
						}
						else {
							title = "Untitled";

							qDebug() << "Warning: slide Title has not been provided. Defaulted to \"Untitled\".";
						}

						// Time allotted for the slide
						int slideTime;
						if (slideEntry.contains("time")) {
							QVariantMap time = slideEntry["time"].value<QVariant>().value<QVariantMap>();
							if (time.contains("minutes")) {
								slideTime = (60*time["minutes"].value<int>());
								if (time.contains("seconds")) {
									slideTime += time["seconds"].value<int>();
								}

								// The slide time for the presentation cannot be negative or 0 and must be less than or equal to the presentation total time
								if (slideTime <= 0 || slideTime > totalTime) {
									slideTime = 1; // Defaults to 1 second

									qDebug() << "Warning: slide Time is negative or 0. Defaulted to 1 (0 minutes 1 second).";
								}
								else if (slideTime > totalTime) {
									slideTime = totalTime; // Defaults to presentation total time
								}
							}
							else {
								slideTime = 1; // Defaults to 1 second

								qDebug() << "Warning: slide Time has not been provided. Defaulted to 1 (0 minutes 1 second).";
							}
						}
						else {
							slideTime = totalTime; // Ditto

							qDebug() << "Warning: slide Time has not been provided. Defaulted to 1 (0 minutes 1 second).";
						}

						Slide* slide = new Slide(title, slideTime);
						slideList.append(slide);
					}
				}
			}
			qDebug() << "Warning: presentation has not been provided any Slides. Empty slide list created.";

			Presentation* presentation = new Presentation(name, totalTime, lastModified, slideList);
			presentation->print();
			list.append(presentation);
		}
	}
	qDebug() << "Error: no presentations exist in the JSON file. Creating empty presentations list...";

	return list;
}

/* Parse a JSON file, store the data in appropriate structures and return a list of this data.
 * Note: dateModified field must be of the ISO 8601 date/time standard format. */
PresentationList ApplicationUI::getListFromJSON(QString filePath) {
	// Create a JDA object and read the JSON file
	JsonDataAccess jda;
	QVariant wrapList = jda.load(filePath);
	QList<QVariant> fileEntries = wrapList.value<QVariantList>();

	PresentationList list;
	if (!wrapList.isNull() && wrapList.isValid() && !fileEntries.isEmpty()) {
		// Create the list of Presentation objects
		list = this->unWrapListFromQVarList(fileEntries);
	}
	else {
		qDebug() << "JSON file is invalid or empty. Creating an empty presentations list...";
	}

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
	qVarMap["id"] = QVariant(presentation->id());
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
		QVariantMap updatedMap = this->wrapToQVarMap(presentation);
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
	QVariantMap qVarMap = this->wrapToQVarMap(slide);
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

//////////////// Perform Page

//void ApplicationUI::playPresentation() {
//	SlideList& slides = _activePresentation->slidesRef();
//
//	foreach (Slide* slide, slides) {
//		QString title = slide->title();
//		int time = slide->time();
//
//		QTimer countdown, colour;
//		countdown.setInterval(1000);
//		colour.setInterval(100);
//
//		bool res;
//		res = QObject::connect(countdown, SIGNAL(timeout()), this, SLOT(countdownSlide()));
//		Q_ASSERT(res);
//	}
//}


// Memo: One problem with debugging errors is moc errors, since they are not in your code. Usually, you get these errors when not including certain classes explicitly


// TODO Implement time slider auto-correction logic for prepare page
// TODO Maybe convert main page UI to plain list with no buttons. Buttons should be provided at the action bar (same for prepare page)
// TODO Need a reset button and functionality for prepare page
// TODO Implement the perform functionality!
// TODO Implement the practise functionality!
// TODO Need a restore defaults button in setting to restore all data to initial load state
