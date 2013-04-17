/*
 * PageModel.h
 *
 *  Created on: 2013-04-16
 *      Author: pilgrim
 */

#ifndef PAGEMODEL_H_
#define PAGEMODEL_H_

#include <bb/cascades/DataModel>
#include <bb/cascades/AbstractPane>
#include <QObject>
#include <QList>

using namespace bb::cascades;

namespace bb {
namespace javelind {

typedef QList< QObject* > DataList;

class PageModel : public QObject {
	Q_OBJECT

protected:
	AbstractPane* _root;
	DataModel* _dataModel;

public:
	PageModel();
	virtual ~PageModel();
};

} /* namespace javelind */
} /* namespace bb */
#endif /* PAGEMODEL_H_ */
