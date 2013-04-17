/*
 * PrepareUI.cpp
 *
 *  Created on: 2013-04-16
 *      Author: pilgrim
 */

#include <QDebug>
#include <bb/cascades/GroupDataModel>
#include "PrepareUI.h"

PrepareUI::PrepareUI(Application* app, ApplicationUI* appUI, AbstractPane* root, Presentation* presentation) {
	qDebug() << "BEGIN";
	_app = app;
	_appUI = appUI;
	_presentation = presentation;
//	_root = app->scene()->findChild<AbstractPane*>("preparePage");
	_root = root;

	// Populate the slide data model
	GroupDataModel* model = _root->findChild<GroupDataModel*>("slideDataModel");
	appUI->updateDataModel(presentation->slides(), model);

	qDebug() << "DONE";
}

PrepareUI::~PrepareUI() {
	// TODO Auto-generated destructor stub
}
