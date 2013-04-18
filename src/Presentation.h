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

	QString name();
	int totalTime();
	QDateTime lastModified();
	SlideList slides();

	/* QML Accessors */

	QVariantList slidesQML();

	/* Mutators */

	void setName(QString name);
	void setTotalTime(int time);
	void setSlides(SlideList slides);

	/* Member Functions */

	Q_INVOKABLE void print();

signals:
	void nameChanged(QString newName);
	void totalTimeChanged(int newTotalTime);
	void lastModifiedChanged(QDateTime newTimeStamp); // TODO Seems like an unnecessary signal
	void slidesChanged(SlideList newSlides);

	void presentationModified(); // Indicates that at least one member of the presentation object was modified

public slots:
	void updateLastModified();
};

} /* namespace javelind */
} /* namespace bb */

Q_DECLARE_METATYPE(bb::javelind::Presentation*);
Q_DECLARE_METATYPE(bb::javelind::SlideList);

#endif /* PRESENTATION_H_ */
