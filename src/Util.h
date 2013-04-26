/*
 * Util.h
 *
 *	This class provides utility to the Present Buddy application in the form of static members and functions.
 *
 *  Created on: 2013-04-25
 *      Author: pilgrim
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <QString>
#include <QVariant>
#include <QMap>
#include <QList>
#include "Presentation.h"

namespace javelind {
namespace bb {
namespace pbuddy {

typedef QList< Presentation* > PresentationList;

class Util {
private:
    /* This class is not meant to be initialized - hence the constructor and destructor are private */
	Util();
	virtual ~Util();

public:
    /* Static Members */
    static const QString DATA_FILE;
    static const QString DISPLAY_DATE_TIME_FORMAT;

    static QVariantMap createTimeQVarMap(int time);
    static QVariantMap wrapToQVarMap(Presentation* presentation);
    static QVariantMap wrapToQVarMap(Slide* slide);
    static QVariantList wrapListToQVarList(PresentationList list);
    static QVariantList wrapListToQVarList(SlideList list);
    static PresentationList unWrapListFromQVarList(QVariantList qVarList);
    static PresentationList getListFromJSON(QString filePath);
    static void saveListToJSON(PresentationList list, QString filePath);
};

} /* namespace pbuddy */
} /* namespace bb */
} /* namespace javelind */


#endif /* UTIL_H_ */
