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
    Presentation* _bufferPresentation; // A buffer to store any changes made to the active presentation before actually committing them

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
    Presentation* bufferPresentation();

    Presentation* activePresentation();
    DataModel* mainDataModel();

    /* QVariant[List] wrappers for exposing to QML */
//    QVariantList presentationsQML();

    /* Mutators */

    void setActivePresentation(Presentation* presentation);

    /* Member Functions */

    QVariantList findInDataModel(Presentation* presentation, DataModel* model);

    void initializePreparePage(Page* page);
    void initializePerformPage(Page* page);
    void reinitializeMainPage(Page* page);

    QVariantMap createTimeQVarMap(int time);
    QVariantMap wrapToQVarMap(Presentation* slide);
    QVariantMap wrapToQVarMap(Slide* slide);
    QVariantList wrapListToQVarList(PresentationList list);
    QVariantList wrapListToQVarList(SlideList list);
    PresentationList unWrapListFromQVarList(QVariantList qVarList);
    PresentationList getListFromJSON(QString filePath);
    void saveListToJSON(PresentationList list, QString filePath);
//    void updatePresentationDataModel(Presentation* presentation); // Can be invoked in response to certain actions, like page transitions (returning to the main page)

public slots:
	void updatePresentationDataModel(Presentation* presentation); // Can be invoked in response to certain actions, like page transitions (returning to the main page)
	void goToPage(bb::cascades::Page* page); // Cascades data types need to be fully qualified
	void addNewSlide(); // Add a new slide to the active presentation
	void commitPreparedChanges(); // Commit all buffered changes to the active presentation made in the prepare page.
	void testSlot();

	Q_INVOKABLE void deletePresentation();
	Q_INVOKABLE void deletePresentation(Presentation* presentation);

	/* Buffer functions for prepare page */

	void bufferNameChange(QString name);
	void bufferTotalTimeChange(float value);
	Q_INVOKABLE void bufferSlideTitleChange(int index, QString title);
	Q_INVOKABLE void bufferSlideTimeChange(int index, float value);
};

Q_DECLARE_METATYPE(PresentationList);

#endif /* ApplicationUI_HPP_ */
