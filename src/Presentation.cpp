/*
 * Presentation.cpp
 *
 *  Created on: Apr 6, 2013
 *      Author: dvijayak
 */

#include <QDebug>
#include <QDateTime>
#include "applicationui.hpp"
#include "Presentation.h"
#include "Slide.h"

namespace bb {
namespace javelind {

/* House Keeping */

void Presentation::initialize() {
	// Connect signals with slots
	bool res;
	res = QObject::connect(this, SIGNAL(presentationModified()), this, SLOT(updateLastModified()));
	Q_ASSERT(res);
}

/* Constructors/Destructors */

Presentation::Presentation() {
	_name = "Untitled Presentation";
	_totalTime = 0;
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
	_name = name;
	emit nameChanged(name);
	emit presentationModified();
}

void Presentation::setTotalTime(int time) {
	_totalTime = time;
	emit totalTimeChanged(time);
	emit presentationModified();
}

void Presentation::setSlides(SlideList slides) {
	_slides = slides;
	emit slidesChanged(slides);
	emit presentationModified();
}

/* Slots */

void Presentation::updateLastModified() {
	_lastModified = QDateTime::currentDateTime();
	emit lastModifiedChanged(_lastModified); // TODO Seems like an unnecessary signal
}

/* Member Functions */

/* Print the presentation to stdout */
void Presentation::print() {
	qDebug() << "---xx---";
	qDebug() << "Presentation";
	qDebug() << "--------";
	qDebug() << "Name = " << _name;
	qDebug() << "Total Time = " << (int)(_totalTime/60) << ":" << _totalTime%60; // Display a time in seconds as minutes:seconds
	qDebug() << "Last Modified = " << _lastModified.toString(ApplicationUI::DISPLAY_DATE_TIME_FORMAT);
	qDebug() << "Slides = ";
	foreach (Slide* slide, _slides) {
		slide->print();
	}
	qDebug() << "---xx---";
}

} /* namespace javelind */
} /* namespace bb */
