#ifndef __SPLASHSCREEN_H__
#define __SPLASHSCREEN_H__

#include "Window.h"
#include <qvariant.h>
#include <qlabel.h>
#include <qthread.h>
#include <qtextbrowser.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QProgressBar;
class QTextView;

// text browser for the help dialog
class FCSplashBrowser : public QTextBrowser
{
  Q_OBJECT

  public:
    FCSplashBrowser(QWidget * parent=0, const char * name=0);
    void setSource ( const QString & name );

  signals:
    void linkClicked(const QString& txt);
};

// splash widget base class
class GuiExport FCSplashWidget : public QLabel, public QThread
{
  Q_OBJECT

  public:
    FCSplashWidget( QWidget* parent = 0, const char* name = 0, WFlags f = 0 );
    ~FCSplashWidget();

    QString SplasherText;
    bool bRun;

  protected:
    virtual void run() = 0;
    virtual QString getName() = 0;
    void hideEvent ( QHideEvent * e);
#pragma warning(disable : 4251)
    FCmap<FCstring, FCpair<FCstring, QPixmap> > _aclDevelopers;

  protected slots:
    void aboutToQuit();
};

// splasher at startup
class GuiExport FCSplashScreen : public FCSplashWidget
{ 
  Q_OBJECT

  public:
    FCSplashScreen( QWidget* parent = 0, const char* name = 0 );
    ~FCSplashScreen();

  protected:
    virtual void run();
    virtual QString getName();
    QVBoxLayout* SplasherDialogLayout;
    QLabel* PixmapLabel;
    QTextView* SplasherTextView;
    QProgressBar* SplasherProgress;
};

// splasher for the help dialog
class GuiExport FCSplashAbout : public FCSplashWidget
{ 
  Q_OBJECT

  public:
	  static void Destruct(void);
  	static FCSplashAbout* Instance(void);
    void setFCPixmap(const QPixmap& image0);

  private:
    FCSplashAbout( QWidget* parent = 0, const char* name = 0 );
    ~FCSplashAbout();
    static FCSplashAbout* _pclSingleton;

  protected:
    virtual void run();
    virtual QString getName();
    void keyPressEvent ( QKeyEvent * e );
    void hideEvent ( QHideEvent * e);
    QVBoxLayout* SplasherDialogLayout;
    QPushButton* ButtonOK;
    QLabel* PixmapLabel;
    FCSplashBrowser* SplasherTextView;

  protected slots:
    void clicked();
    void linkClicked(const QString& txt);
};

#endif // __SPLASHSCREEN_H__
