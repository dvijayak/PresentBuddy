/*
 * PrepareUI.h
 *
 *  Created on: 2013-04-16
 *      Author: pilgrim
 */

#ifndef PREPAREUI_H_
#define PREPAREUI_H_

#include <QObject>
#include <QList>
#include <bb/cascades/Application>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/DataModel>

#include "applicationui.hpp"
#include "Presentation.h"

using namespace bb::cascades;
using namespace bb::javelind;

class PrepareUI : public QObject {
	Q_OBJECT
private:
    Application* _app;
    ApplicationUI* _appUI;
    AbstractPane* _root;
    DataModel* _dataModel;
    Presentation* _presentation;

public:
	/* Constructors/Destructors */
	PrepareUI(Application* app, ApplicationUI* appUI, AbstractPane* root, Presentation* presentation);
	virtual ~PrepareUI();

	/* Accessors */
    Application* app();
    ApplicationUI* appUI();
    AbstractPane* root();
    DataModel* dataModel();
    Presentation* presentation();

	/* Mutators */
    // TODO

	/* Member Functions */
};

#endif /* PREPAREUI_H_ */
