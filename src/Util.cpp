/*
 * Util.cpp
 *
 *  Created on: 2013-04-25
 *      Author: pilgrim
 */

#include <bb/system/SystemToast>
#include <bb/data/JsonDataAccess>
#include <QDebug>
#include "Util.h"

using namespace bb::data;
using namespace bb::system;

namespace javelind {
namespace bb {
namespace pbuddy {

const QString Util::DATA_FILE("/pbuddy-data.json");
const QString Util::DISPLAY_DATE_TIME_FORMAT("MMMM d yyyy h:mm:ss AP"); // A separate format solely for rendering on the device

Util::Util() {}
Util::~Util() {}

/* Retrieves a list of Presentation objects from a QVariantList wrapper */
PresentationList Util::unWrapListFromQVarList(QVariantList qVarList) {
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
	else {
		qDebug() << "Error: no presentations exist in the JSON file. Creating empty presentations list...";
	}

	return list;
}

/* Parse a JSON file, store the data in appropriate structures and return a list of this data.
 * Note: dateModified field must be of the ISO 8601 date/time standard format. */
PresentationList Util::getListFromJSON(QString filePath) {
	// Create a JDA object and read the JSON file
	JsonDataAccess jda;
	QVariant wrapList = jda.load(filePath);
	QList<QVariant> fileEntries = wrapList.value<QVariantList>();

	PresentationList list;
	// Ensure that there was no error reading the file or any other error
	if (!jda.hasError()
			&& !wrapList.isNull()
			&& wrapList.isValid()
			&& !fileEntries.isEmpty()) {

		// Create the list of Presentation objects
		list = Util::unWrapListFromQVarList(fileEntries);

		// For an empty data file, nudge the user to create a new presentation
		if (list.size() <= 0) {
			SystemToast* toast = new SystemToast();
			toast->setBody("Create a new presentation!");
			toast->show();
			delete toast; // We are responsible for freeing it since we are not attaching it to any QObject
		}
	}
	else {
		qDebug() << "JSON file is invalid or empty. Creating an empty presentations list...";

		// Error checking
		if (jda.hasError()) {
			// Retrieve the error
			DataAccessError theError = jda.error();

			// Log the error
			QString errorMessage;
			if (theError.errorType() == DataAccessErrorType::SourceNotFound)
				errorMessage = "File not found: " + theError.errorMessage();
			else if (theError.errorType() == DataAccessErrorType::ConnectionFailure)
				errorMessage = "Connection failure: " + theError.errorMessage();
			else if (theError.errorType() == DataAccessErrorType::OperationFailure)
				 errorMessage = "Operation failure: " + theError.errorMessage();
			qDebug() << errorMessage;
		}

		// Nudge the user to create a new presentation
		SystemToast* toast = new SystemToast();
		toast->setBody("Create a new presentation!");
		toast->show();
		delete toast; // Ditto as above
	}

	return list;
}

/* Create a QVariantMap representation of a time object */
QVariantMap Util::createTimeQVarMap(int time) {
	QVariantMap timeMap;
	timeMap["minutes"] = QVariant((int)(time/60));
	timeMap["seconds"] = QVariant(time%60);
	return timeMap;
}

/* Wraps a slide around a QVariantMap */
QVariantMap Util::wrapToQVarMap(Slide* slide) {
	QVariantMap qVarMap;
	qVarMap["title"] = QVariant(slide->title());
	qVarMap["time"] = Util::createTimeQVarMap(slide->time());
//	qVarMap["dateModified"] = QVariant(slide->lastModified());
	return qVarMap;
}


/* Wraps a presentation around a QVariantMap */
QVariantMap Util::wrapToQVarMap(Presentation* presentation) {
	QVariantMap qVarMap;
	qVarMap["id"] = QVariant(presentation->id());
	qVarMap["name"] = QVariant(presentation->name());
	qVarMap["totalTime"] = Util::createTimeQVarMap(presentation->totalTime());
	qVarMap["dateModified"] = QVariant(presentation->lastModified());

	// Create a QVariant in turn for each slide object belonging to the presentation, also wrapping as QVariantMap objects
	QVariantList slideQVarList = Util::wrapListToQVarList(presentation->slides());
	qVarMap["slides"] = QVariant(slideQVarList);

	return qVarMap;
}

/* Wraps a list of Slide objects around a list of QVariant objects */
QVariantList Util::wrapListToQVarList(SlideList list) {
	QVariantList qVarList;
	// Create a QVariant for each slide object that wraps its members as QVariantMap objects
	foreach (Slide* slide, list) {
		QVariantMap qVarMap = Util::wrapToQVarMap(slide);

		// Push each Slide QVariantMap object to qVarList
		qVarList.append(QVariant(qVarMap));
	}

	return qVarList;
}

/* Wraps a list of Presentation objects around a list of QVariant objects */
QVariantList Util::wrapListToQVarList(PresentationList list) {
	QVariantList qVarList;
	// Create a QVariant for each presentation object that wraps its members as QVariantMap objects
	foreach (Presentation* presentation, list) {
		QVariantMap qVarMap = Util::wrapToQVarMap(presentation);

		// Push each Presentation QVariantMap object to qVarList
		qVarList.append(QVariant(qVarMap));
	}

	return qVarList;
}

/* Serialize the specified list of presentations to the specified JSON file.
 * Note: dateModified field is saved using the ISO 8601 date/time standard format. */
void Util::saveListToJSON(PresentationList list, QString filePath) {
	// Retrieve the list as a QVariantList
	QVariantList qVarList = Util::wrapListToQVarList(list);

	// Create a JDA object and save to the JSON file
	JsonDataAccess jda;
	jda.save(QVariant(qVarList), filePath);

	// Error checking
	if (jda.hasError()) {
		// Retrieve the error
		DataAccessError theError = jda.error();

		// Log the error
		QString errorMessage;
		if (theError.errorType() == DataAccessErrorType::ConnectionFailure)
			errorMessage = "Connection failure: " + theError.errorMessage();
		else if (theError.errorType() == DataAccessErrorType::OperationFailure)
			 errorMessage = "Operation failure: " + theError.errorMessage();
		qDebug() << errorMessage;
	}
}

} /* namespace pbuddy */
} /* namespace bb */
} /* namespace javelind */
