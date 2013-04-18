/*
 * Slide.cpp
 *
 *  Created on: Apr 6, 2013
 *      Author: dvijayak
 */

#include <QDebug>
#include "Slide.h"

namespace bb {
namespace javelind {

Slide::Slide() {
	_title = "Untitled";
	_time = 60;
}

Slide::Slide(QString title, int time) {
	_title = title;
	_time = time;
}

Slide::~Slide() {
	// TODO Auto-generated destructor stub
}

/* Accessors */

QString Slide::title() {
	return _title;
}

int Slide::time() {
	return _time;
}

/* Mutators */

void Slide::setTitle(QString title) {
	_title = title;
	emit titleChanged(title);
}

void Slide::setTime(int time) {
	_time = time;
	emit timeChanged(time);
}

/* Member Functions */

/* Print the slide to stdout */
void Slide::print() {
	qDebug() << "\t---xx---";
	qDebug() << "\tSlide";
	qDebug() << "\t--------";
	qDebug() << "\tTitle = " << _title;
	qDebug() << "\tTime = " << (int)(_time/60) << ":" << _time%60; // Display a time in seconds as minutes:seconds
//	qDebug() << "\tLast Modified = " << _lastModified.toString();
	qDebug() << "\t---xx---";
}

} /* namespace javelind */
} /* namespace bb */
