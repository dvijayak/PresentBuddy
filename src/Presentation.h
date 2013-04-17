/*
 * Presentation.h
 *
 *  Created on: Apr 6, 2013
 *      Author: dvijayak
 */

#ifndef PRESENTATION_H_
#define PRESENTATION_H_

#include <QObject>
#include <QDateTime>
#include <QVector>
#include <QMetaType>
#include "Slide.h"

namespace bb {
namespace javelind {

typedef QList< Slide* > SlideList;

class Presentation: public QObject {
	Q_OBJECT
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(int* totalTime READ totalTime WRITE setTotalTime NOTIFY totalTimeChanged)
	Q_PROPERTY(QDateTime lastModified READ lastModified WRITE setLastModified NOTIFY lastModifiedChanged)
	Q_PROPERTY(SlideList* slides READ slides)

private:
	QString _name;
	int* _totalTime;
	QDateTime _lastModified;
	SlideList* _slides;

public:
	Presentation();
	Presentation(QString name, int* totalTime, QDateTime lastModified, SlideList* slides);
	virtual ~Presentation();

	QString name();
	int* totalTime();
	QDateTime lastModified();
	QString lastModified(QString format);
	SlideList* slides();

	void setName(QString name);
	void setTotalTime(int* time);
	void setLastModified(QDateTime lastModified);
	void setSlides(SlideList* slides);

	/* Replace the slide at the specified index with the new slide */
	void setSlide(int index, Slide* slide);

	/* Insert (append) the newly created slide into the presentation */
	Q_INVOKABLE void appendSlide (Slide* slide);

	/* Print the presentation to stdout */
	void print();

signals:
	void nameChanged();
	void totalTimeChanged();
	void lastModifiedChanged();
	void slidesChanged();
};

} /* namespace javelind */
} /* namespace bb */

Q_DECLARE_METATYPE(bb::javelind::Presentation*);
Q_DECLARE_METATYPE(bb::javelind::SlideList);

#endif /* PRESENTATION_H_ */
