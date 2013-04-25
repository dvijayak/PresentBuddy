// Default empty project template
#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>
#include <QList>
#include <QTimer>
#include <bb/cascades/ScreenIdleMode>
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/Page>
#include <bb/cascades/DataModel>
#include "Presentation.h"
#include "Util.h"

using namespace bb::cascades;
using namespace javelind::bb::pbuddy;

namespace bb { namespace cascades { class Application; }}

/*!
 * @brief Application pane object
 *
 *Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class ApplicationUI : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap activePresentation READ activePresentationQML)

    // Expose the ScreenIdleMode enum from bb::cascades::ScreenIdleMode to QML - for some reason, it does not recognize it
//    Q_ENUMS(bb::cascades::ScreenIdleMode::Normal)

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
    void initializePreviewPage(Page* page);
    void initializePerformPage(Page* page);
    void reinitializeMainPage(Page* page);

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
