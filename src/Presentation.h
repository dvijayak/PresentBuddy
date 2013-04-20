/*
 * Presentation.h
 *
 *  Created on: Apr 6, 2013
 *      Author: dvijayak
 */

#ifndef PRESENTATION_H_
#define PRESENTATION_H_

#include <QObject>
#include <QList>
#include <QVariant>
#include <QDateTime>
#include <QMetaType>
#include "Slide.h"

namespace bb {
namespace javelind {

typedef QList< Slide* > SlideList;

class Presentation: public QObject {
	Q_OBJECT
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(int totalTime READ totalTime WRITE setTotalTime NOTIFY totalTimeChanged)
	Q_PROPERTY(QDateTime lastModified READ lastModified NOTIFY lastModifiedChanged) // Do not allow the time stamp to be modified manually
	Q_PROPERTY(QVariantList slides READ slidesQML)

private:
	qint64 _id; // Read-only unique identifier. Valid only for each session. Used to uniquely identify objects that are grouped in unordered lists
	QString _name;
	int _totalTime;
	QDateTime _lastModified; // Read-only; by definition, it must modify by itself under the hood
	SlideList _slides;

	/* House Keeping */

	void initialize();

public:
	/* Constructors/Destructors */

	Presentation();
	Presentation(QString name, int totalTime, QDateTime lastModified);
	Presentation(QString name, int totalTime, QDateTime lastModified, SlideList slides);
	virtual ~Presentation();

	/* Accessors */

	qint64 id();
	QString name();
	int totalTime();
	QDateTime lastModified();
	SlideList slides();
	SlideList& slidesRef(); // get reference to the slide list for manipulation directly (efficiency purposes)

	/* QML Accessors */

	QVariantList slidesQML();

	/* Mutators */

	void setName(QString name);
	void setTotalTime(int time);
	void setSlides(SlideList slides);

	/* Member Functions */

	Presentation* copy(); // Note: QObject does not have a visible copy constructor, thus we need to make an explicit copy function ourselves
	void addSlide(Slide* slide);
	Q_INVOKABLE void print();

signals:
	void nameChanged(QString newName);
	void totalTimeChanged(int newTotalTime);
	void lastModifiedChanged(QDateTime newTimeStamp);
	void slidesChanged(SlideList newSlides);

	void presentationChanged(Presentation* presentation); // Indicates that at least one member of the presentation object was modified

public slots:
	void updateLastModified();
	void setPresentation();
};

} /* namespace javelind */
} /* namespace bb */

Q_DECLARE_METATYPE(bb::javelind::Presentation*);
Q_DECLARE_METATYPE(bb::javelind::SlideList);

#endif /* PRESENTATION_H_ */

// TODO be able to add a slide in between another slide (i.e., prepend/append to an existing slide)
