/*
 * Slide.h
 *
 *  Created on: Apr 6, 2013
 *      Author: dvijayak
 */

#ifndef SLIDE_H_
#define SLIDE_H_

#include <QObject>
#include <QMetaType>
#include <QString>

namespace javelind {
namespace bb {
namespace pbuddy {


class Slide : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
	Q_PROPERTY(int time READ time WRITE setTime NOTIFY timeChanged)

private:
	QString _title;
	int _time; // in seconds

	/* House Keeping */
	void initialize();

public:
	/* Constructors/Destructors */

	Slide();
	Slide(QString title, int time);
	virtual ~Slide();

	/* Accessors */

	QString title();
	int time();

	/* Mutators */

	void setTitle(QString title);
	void setTime(int time);

	/* Operator Overloads */

	bool operator ==(const Slide*& other) const;

	/* Member Functions */

	Slide* copy(); // Note: QObject does not have a visible copy constructor, thus we need to make an explicit copy function ourselves
	Q_INVOKABLE void print();

signals:
	void titleChanged(QString newTitle);
	void timeChanged(int newTime);

	void slideChanged(Slide* slide); // Indicates that at least one member of the slide object was modified

public slots:
	void setSlide();
};

} /* namespace pbuddy */
} /* namespace bb */
} /* namespace javelind */

Q_DECLARE_METATYPE(javelind::bb::pbuddy::Slide*);

#endif /* SLIDE_H_ */
