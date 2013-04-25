/*
 * Slide.cpp
 *
 *  Created on: Apr 6, 2013
 *      Author: dvijayak
 */

#include <QDebug>
#include "Slide.h"

namespace javelind {
namespace bb {
namespace pbuddy {

/* House Keeping */

void Slide::initialize() {
	bool res;
	res = QObject::connect(this, SIGNAL(titleChanged(QString)), this, SLOT(setSlide()));
	Q_ASSERT(res);
	res = QObject::connect(this, SIGNAL(timeChanged(int)), this, SLOT(setSlide()));
	Q_ASSERT(res);
}

Slide::Slide() {
	_title = "Untitled";
	_time = 60;

	this->initialize();
}

Slide::Slide(QString title, int time) {
	_title = title;
	_time = time;

	this->initialize();
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
	if (_title != title) {
		qDebug() << QString("%1 != %2, updating slide title").arg(_title).arg(title);
		_title = title;
		emit titleChanged(title);
	}
	else qDebug() << QString("%1 == %2, no change in slide title").arg(_title).arg(title);
}

void Slide::setTime(int time) {
	if (_time != time) {
		qDebug() << QString("%1 != %2, updating slide time").arg(_time).arg(time);
		_time = time;
		emit timeChanged(time);
	}
	else qDebug() << QString("%1 == %2, no change in slide time").arg(_time).arg(time);
}

/* Operator Overloads */

/* Copy */
bool Slide::operator ==(const Slide*& other) const {
	if (_title == other->_title
			&& _time == other->_time) {
		return true;
	}
	else {
		return false;
	}
}


/* Member Functions */

Slide* Slide::copy() {
	return new Slide(_title, _time);
}

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

/* Slots */

void Slide::setSlide() {
	emit slideChanged(this);
}

} /* namespace pbuddy */
} /* namespace bb */
} /* namespace javelind */
