/*
 * Presentation.cpp
 *
 *  Created on: Apr 6, 2013
 *      Author: dvijayak
 */

#include <QDebug>
#include <QDateTime>
#include "Presentation.h"
#include "Slide.h"

namespace bb {
namespace javelind {

Presentation::Presentation() {
	_name = "Untitled Presentation";
	int totalTime[] = {0, 0};
	_totalTime = totalTime;
	_lastModified = QDateTime::currentDateTime();
}

Presentation::Presentation(QString name, int* totalTime, QDateTime lastModified, SlideList* slides) {
	_name = name;
	_totalTime = totalTime;
	_lastModified = lastModified;
	_slides = slides;
}

Presentation::~Presentation() {
	delete[] _totalTime;
}

/* Accessors */

QString Presentation::name() {
	return _name;
}

int* Presentation::totalTime() {
	return _totalTime;
}

QDateTime Presentation::lastModified() {
	return _lastModified;
}

QString Presentation::lastModified(QString format) {
	return _lastModified.toString(format);
}

SlideList* Presentation::slides() {
	return _slides;
}

/* Mutators */

void Presentation::setName(QString name) {
	_name = name;
}

void Presentation::setTotalTime(int* time) {
	_totalTime = time;
}

void Presentation::setLastModified(QDateTime lastModified) {
	_lastModified = lastModified;
}

void Presentation::setSlides(SlideList* slides) {
	_slides = slides;
}

/* Member Functions */

/* Replace the slide at the specified index with the new slide */
void Presentation::setSlide(int index, Slide* slide) {

}

/* Insert (append) the newly created slide into the presentation */
void Presentation::appendSlide(Slide* slide) {
	_slides->append(slide);
}

/* Print the presentation to stdout */
void Presentation::print() {
	qDebug() << "---xx---";
	qDebug() << "Presentation";
	qDebug() << "--------";
	qDebug() << "Name = " << _name;
	qDebug() << "Total Time = " << _totalTime[0] << ":" << _totalTime[1];
	qDebug() << "Last Modified = " << _lastModified.toString();
	qDebug() << "Slides = ";
	foreach (Slide* slide, *_slides) {
		slide->print();
	}
	qDebug() << "---xx---";
}

} /* namespace javelind */
} /* namespace bb */
