/*
 * Presentation.cpp
 *
 *  Created on: Apr 6, 2013
 *      Author: dvijayak
 */

#include <QDebug>
#include <QDateTime>
#include "applicationui.hpp"
#include "Util.h"
#include "Presentation.h"
#include "Slide.h"

namespace javelind {
namespace bb {
namespace pbuddy {

/* House Keeping */

const int Presentation::MAX_TIME = 59999;

void Presentation::initialize() {
	// Initialize any uninitialized data members
	_id = QUuid::createUuid().toString(); // Guaranteed to be unique for an entire session

	// Connect signals with slots
	bool res;
	res = QObject::connect(this, SIGNAL(nameChanged(QString)), this, SLOT(updateLastModified()));
	Q_ASSERT(res);
	res = QObject::connect(this, SIGNAL(totalTimeChanged(int)), this, SLOT(updateLastModified()));
	Q_ASSERT(res);
	res = QObject::connect(this, SIGNAL(slidesChanged(SlideList)), this, SLOT(updateLastModified()));
	Q_ASSERT(res);
	res = QObject::connect(this, SIGNAL(slidesChanged(SlideList)), this, SLOT(consolidateTotalTime()));
	Q_ASSERT(res);

	// Connect the presentation with each of its slides
	foreach (Slide* slide, _slides) {
		res = QObject::connect(slide, SIGNAL(slideChanged(Slide*)), this, SLOT(updateLastModified()));
		Q_ASSERT(res);
		res = QObject::connect(slide, SIGNAL(slideChanged(Slide*)), this, SLOT(consolidateTotalTime()));
		Q_ASSERT(res);
	}

	res = QObject::connect(this, SIGNAL(lastModifiedChanged(QDateTime)), this, SLOT(setPresentation()));
	Q_ASSERT(res);
}

/* Constructors/Destructors */

Presentation::Presentation() {
	_name = "Untitled";
	_totalTime = 600; // Default total time of 10 minutes
	_lastModified = QDateTime::currentDateTime();

	this->initialize();
}

Presentation::Presentation(QString name, int totalTime, QDateTime lastModified) {
	_name = name;
	_totalTime = totalTime;
	_lastModified = lastModified;

	this->initialize();
}

Presentation::Presentation(QString name, int totalTime, QDateTime lastModified, SlideList slides) {
	_name = name;
	_totalTime = totalTime;
	_lastModified = lastModified;
	_slides = slides;

	this->initialize();
}

Presentation::~Presentation() {
	// TODO Presentation Destructor stuff
}

/* Accessors */

QString Presentation::id() {
	return _id;
}

QString Presentation::name() {
	return _name;
}

int Presentation::totalTime() {
	return _totalTime;
}

QDateTime Presentation::lastModified() {
	return _lastModified;
}

SlideList Presentation::slides() {
	return _slides;
}

SlideList& Presentation::slidesRef() {
	return _slides;
}

/* QML Accessors */

/* QVariant[List] wrappers for exposing to QML */
QVariantList Presentation::slidesQML() {
	QVariantList qVarList;
	foreach (Slide* slide, _slides) {
		qVarList.append(QVariant::fromValue(slide));
	}
	return qVarList;
}

/* Mutators */

void Presentation::setName(QString name) {
	if (_name != name) {
		qDebug() << QString("%1 != %2, updating presentation name").arg(_name).arg(name);
		_name = name;
		emit nameChanged(name);
	}
	else qDebug() << QString("%1 == %2, no change in presentation name").arg(_name).arg(name);
}

void Presentation::setTotalTime(int time) {
	if (_totalTime != time && _totalTime <= Presentation::MAX_TIME) {
		qDebug() << QString("%1 != %2, updating presentation total time").arg(_totalTime).arg(time);
		_totalTime = time;
		emit totalTimeChanged(time);
	}
	else qDebug() << QString("%1 == %2, no change in presentation total time").arg(_totalTime).arg(time);
}

void Presentation::setSlides(SlideList slides) {
	if (_slides != slides) {
		qDebug() << QString("%1 != %2, updating presentation slide list").arg("old slide list").arg("new slide list");
		_slides = slides;
		emit slidesChanged(slides);
	}
	else qDebug() << QString("%1 == %2, no change in presentation slide list").arg("old slide list").arg("new slide list");
}

/* Member Functions */

Presentation* Presentation::copy() {
	// Each slide in the list of slides must also be copied
	SlideList slidesCopy;
	foreach (Slide* slide, _slides) {
		slidesCopy.append(slide->copy());
	}

	Presentation* copy = new Presentation(_name, _totalTime, _lastModified, slidesCopy);
	copy->_id = _id;
	return copy;
}

/* Add the specified slide (append at the end) to the presentation list of slides */
void Presentation::addSlide(Slide* slide) {
	_slides.append(slide);
	slide->print();
	emit slidesChanged(_slides);
}

/* Delete the slide corresponding to the specified slide index from the presentation list of slides */
void Presentation::deleteSlide(int index) {
	_slides.removeAt(index);
	emit slidesChanged(_slides);
}

/* Delete the specified slide from the presentation list of slides */
void Presentation::deleteSlide(Slide* slide) {
	_slides.removeOne(slide);
	emit slidesChanged(_slides);
}

/* Print the presentation to stdout */
void Presentation::print() {
	qDebug() << "---xx---";
	qDebug() << "Presentation";
	qDebug() << "--------";
	qDebug() << "Name = " << _name;
	qDebug() << "Total Time = " << (int)(_totalTime/60) << ":" << _totalTime%60; // Display a time in seconds as minutes:seconds
	qDebug() << "Last Modified = " << _lastModified.toString(Util::DISPLAY_DATE_TIME_FORMAT);
	qDebug() << "Slides = ";
	foreach (Slide* slide, _slides) {
		slide->print();
	}
	qDebug() << "---xx---";
}

/* Slots */

/* Update the time stamp of this presentation whenever a member has been modified */
void Presentation::updateLastModified() {
	_lastModified = QDateTime::currentDateTime();
	emit lastModifiedChanged(_lastModified);
}

void Presentation::setPresentation() {
//	qDebug() << "PRESENTATION CHANGED!!!";
	emit presentationChanged(this);
}

/* Adjust the presentation's time to be the sum of the times of its slides */
void Presentation::consolidateTotalTime() {
	int consolidatedTime = 0;
	foreach (Slide* slide, _slides) {
		consolidatedTime += slide->time();
	}

	_totalTime = consolidatedTime;
	emit totalTimeChanged(consolidatedTime);
}

} /* namespace pbuddy */
} /* namespace bb */
} /* namespace javelind */
