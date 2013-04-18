// Default empty project template
#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>
#include <QList>
#include <bb/cascades/Application>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/Page>
#include <bb/cascades/DataModel>
#include "Presentation.h"

using namespace bb::cascades;
using namespace bb::javelind;

namespace bb { namespace cascades { class Application; }}

typedef QList< Presentation* > PresentationList;

/*!
 * @brief Application pane object
 *
 *Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class ApplicationUI : public QObject
{
    Q_OBJECT
private:
    Application* _app;
    AbstractPane* _root;
    DataModel* _dataModel; // the current data model of the application
    PresentationList _presentations;
    Presentation* _activePresentation; // reference to the presentation currently being worked on

public:
    /* Static Members */

    static const QString READ_DATE_TIME_FORMAT;
    static const QString WRITE_DATE_TIME_FORMAT;
    static const QString DISPLAY_DATE_TIME_FORMAT;

    /* Constructors/Destructors */

    ApplicationUI(bb::cascades::Application *app);
    virtual ~ApplicationUI();

    /* Accessors */

    Application* app();
    AbstractPane* root();
    DataModel* dataModel();
    PresentationList presentations();
    Presentation* activePresentation();

    /* QVariant[List] wrappers for exposing to QML */
//    QVariantList presentationsQML();

    /* Mutators */

    void setActivePresentation(Presentation* presentation);

    /* Member Functions */

    DataModel* getMainDataModel();

    PresentationList unWrapListFromQVarList(QVariantList qVarList);
    PresentationList getListFromJSON(QString filePath);
    QVariantList wrapListToQVarList(PresentationList list);
    QVariantList wrapListToQVarList(SlideList list);
    void saveListToJSON(PresentationList list, QString filePath);
    void updateDataModel();
    void updateDataModel(PresentationList list, DataModel* dataModel);
    void updateDataModel(SlideList list, DataModel* dataModel);

public slots:
	void goToPage(bb::cascades::Page* page); // Cascades data types need to be fully qualified
};

Q_DECLARE_METATYPE(PresentationList);

#endif /* ApplicationUI_HPP_ */
