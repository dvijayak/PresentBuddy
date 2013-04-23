// Default empty project template
#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>
#include <QList>
#include <QTimer>
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
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
    Q_PROPERTY(QVariantMap activePresentation READ activePresentationQML)

private:
    Application* _app;
    QmlDocument* _qml;
    AbstractPane* _root;
    DataModel* _dataModel; // the current data model of the application
    PresentationList _presentations;
    Presentation* _activePresentation; // reference to the presentation currently being worked on
    Presentation* _bufferPresentation; // A buffer to store any changes made to the active presentation before actually committing them
    QTimer _slideCountdown;
    QTimer _siideColour;

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

    /* QML Accessors */
//    QVariantList presentationsQML();
    QVariantMap activePresentationQML();

    /* Mutators */

    void setActivePresentation(Presentation* presentation);

    /* Member Functions */

    static QString padInt(int number, int n);
    Q_INVOKABLE static int* timeToMinSecs(int time);
    Q_INVOKABLE static int timeFromMinSecs(int minutes, int seconds);
    Q_INVOKABLE static QString timeToText(int time);
    Q_INVOKABLE static QString minSecToText(int minutes, int seconds);

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

signals:
	void performInitialized();

public slots:
	void updatePresentationDataModel(Presentation* presentation); // Can be invoked in response to certain actions, like page transitions (returning to the main page)
	void goToPage(bb::cascades::Page* page); // Cascades data types need to be fully qualified
	void addNewSlide(); // Add a new slide to the active presentation
	void deleteSlide(int index); // Delete the specified slide (by index) from the active presentation
	void commitPreparedChanges(); // Commit all buffered changes to the active presentation made in the prepare page.
	void resetPreparedChanges();

	Q_INVOKABLE void deletePresentation();
	Q_INVOKABLE void deletePresentation(Presentation* presentation);

	/* Buffer functions for prepare page */

	void bufferNameChange(QString name);
	void bufferTotalTimeChange(float value);
	Q_INVOKABLE void bufferSlideTitleChange(int index, QString title);
	Q_INVOKABLE void bufferSlideTimeChange(int index, float value);

	/* Perform page */

};

Q_DECLARE_METATYPE(PresentationList);

#endif /* ApplicationUI_HPP_ */
