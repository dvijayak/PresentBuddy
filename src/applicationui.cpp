// Default empty project template
#include "applicationui.hpp"

#include <bb/cascades/QmlDocument>
#include <bb/cascades/Page>
#include <bb/cascades/ListView>
#include <bb/cascades/DataModel>
#include <bb/cascades/GroupDataModel>
#include <bb/cascades/QListDataModel>
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

    // Create scene document from main.qml asset and
    // set parent to created document to ensure it exists for the whole application lifetime
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    // Register new meta types
    qRegisterMetaType<Presentation*>();
    qRegisterMetaType<Slide*>();

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
    _dataModel = root->findChild<GroupDataModel*>("mainDataModel");

	// Create initial list of Presentation objects from the data file
    QString filePath(QDir::currentPath() + "/app/native/assets/presentations.json");
//    QString filePath(QDir::homePath() + "/presentations_save.json");
    // TODO Check if file exists. If not, or if file is empty, then show no presentations
    _presentations = this->getListFromJSON(filePath);

	// Insert the list of presentations into the data model
	this->updateDataModel();

	// Connect signals with slots
//	bool res = QObject::connect(preparePage, SIGNAL(creationCompleted()), this, SLOT(loadPreparePage()));
//	Q_ASSERT(res);
//	Q_UNUSED(res);

	// Set created root object as a scene
    app->setScene(root);
}

ApplicationUI::~ApplicationUI() {
	// Save the current list of presentations to a JSON file
	this->saveListToJSON(_presentations, QDir::homePath() + "/presentations_save.json");

	// Free all memory that we are responsible for
	foreach(Presentation* presentation, *_presentations) {
		// TODO free all slides
		delete presentation;
	}
	delete _presentations;
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

PresentationList* ApplicationUI::presentations() {
	return _presentations;
}

QVariantList ApplicationUI::presentationsQML() {
	return *(this->wrapListToQVarList(_presentations));
}

/* Mutators */
// TODO

/* Member Functions */

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

/* Wraps a list of Presentation objects around a list of QVariant objects */
QVariantList ApplicationUI::wrapListToQVarList(PresentationList list) {
	QVariantList qVarList;
	// Create a QVariant for each presentation object that wraps its members as QVariantMap objects
	foreach (Presentation* presentation, list) {
		QVariantMap qVarMap;
		qVarMap["name"] = QVariant(presentation->name());

		// A nested map must be wrapped within its own QVariant object
		QVariantMap timeMap;
		int totalTime = presentation->totalTime();
		timeMap["minutes"] = QVariant((int)(totalTime/60));
		timeMap["seconds"] = QVariant(totalTime%60);
		qVarMap["totalTime"] = QVariant(timeMap);
		qVarMap["dateModified"] = QVariant(presentation->lastModified());

		// Create a QVariant in turn for each slide object belonging to the presentation, also wrapping as QVariantMap objects
		SlideList slideList = presentation->slides();
		QVariantList slideQVarList;
		foreach(Slide* slide, slideList) {
			QVariantMap qVarMap;
			qVarMap["title"] = QVariant(slide->title());

			QVariantMap timeMap;
			int slideTime = slide->time();
			timeMap["minutes"] = QVariant((int)(slideTime/60));
			timeMap["seconds"] = QVariant(slideTime%60);
			qVarMap["time"] = QVariant(timeMap);

			slideQVarList.append(QVariant(qVarMap));
		}

		qVarMap["slides"] = QVariant(slideQVarList);

		// Push each QVariant object to qVarList
		qVarList.append(QVariant(qVarMap));
	}

	return qVarList;
}

/* Wraps a list of Slide objects around a list of QVariant objects */
QVariantList ApplicationUI::wrapListToQVarList(SlideList list) {
	QVariantList qVarList;
	// Create a QVariant for each slide object that wraps its members as QVariantMap objects
	foreach (Slide* slide, list) {
		QVariantMap qVarMap;
		qVarMap["title"] = QVariant(slide->title());

		// A nested map must be wrapped within its own QVariant object
		QVariantMap timeMap;
		int slideTime = slide->time();
		timeMap["minutes"] = QVariant((int)(slideTime/60));
		timeMap["seconds"] = QVariant(slideTime%60);
		qVarMap["time"] = QVariant(timeMap);
//		qVarMap["dateModified"] = QVariant(slide->lastModified());

		// Push each QVariant object to qVarList
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

/* Update the default data model with the current list of presentations */
void ApplicationUI::updateDataModel() {
	this->updateDataModel(_presentations, _dataModel);
}

/* Update the specified data model with the specified list of presentations */
void ApplicationUI::updateDataModel(PresentationList list, DataModel* model) {
	// Retrieve the list as a QVariantList
	QVariantList qVarList = this->wrapListToQVarList(list);

	// Update the data model with the new list
	if (((GroupDataModel*) model)->size() > 0) {
		((GroupDataModel*) model)->clear();
	}
	((GroupDataModel*) model)->insertList(qVarList);
}

/* Update the specified data model with the specified list of slides */
void ApplicationUI::updateDataModel(SlideList list, DataModel* model) {
	// Retrieve the list as a QVariantList
	QVariantList qVarList = this->wrapListToQVarList(list);

	// Update the data model with the new list
	if (((GroupDataModel*) model)->size() > 0) {
		((GroupDataModel*) model)->clear();
	}
	((GroupDataModel*) model)->insertList(qVarList);
}

/* Slots */

