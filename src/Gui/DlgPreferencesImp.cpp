/***************************************************************************
 *   Copyright (c) 2002 Jürgen Riegel <juergen.riegel@web.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"

#include <Base/Exception.h>

#include "DlgPreferencesImp.h"
#include "PropertyPage.h"
#include "WidgetFactory.h"
#include "BitmapFactory.h"
#include "MainWindow.h"

#ifdef FC_DEBUG
# include <Base/Console.h> 
#endif

using namespace Gui::Dialog;

/* TRANSLATOR Gui::Dialog::DlgPreferencesImp */

QList<QPair<QString, QStringList> > DlgPreferencesImp::_pages;

/**
 *  Constructs a DlgPreferencesImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgPreferencesImp::DlgPreferencesImp( QWidget* parent, Qt::WFlags fl )
    : QDialog(parent, fl)
{
    this->setupUi(this);
    connect( buttonHelp,  SIGNAL ( clicked() ), getMainWindow(), SLOT ( whatsThis() ));
    connect(listBox, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            this, SLOT(changeGroup(QListWidgetItem *, QListWidgetItem*)));

    setupPages();
}

/**
 *  Destroys the object and frees any allocated resources.
 */
DlgPreferencesImp::~DlgPreferencesImp()
{
  // no need to delete child widgets, Qt does it all for us
}

void DlgPreferencesImp::setupPages()
{
    // make sure that pages are ready to create
    GetWidgetFactorySupplier();
    for (QList<QPair<QString, QStringList> >::iterator it = _pages.begin(); it != _pages.end(); ++it) {
        QTabWidget* tabWidget = new QTabWidget;
        this->tabWidgetStack->addWidget(tabWidget);
        
        QByteArray group = it->first.toAscii();
        QListWidgetItem *item = new QListWidgetItem(listBox);
        item->setData(Qt::UserRole, QVariant(group));
        item->setText(QObject::tr(group.constData()));
        QString iconName = QString("preferences-%1").arg(QString(group).toLower());
        QPixmap icon = Gui::BitmapFactory().pixmapFromSvg(iconName, QSize(96,96));
        item->setIcon(icon);
        item->setTextAlignment(Qt::AlignHCenter);
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        for (QStringList::Iterator jt = it->second.begin(); jt != it->second.end(); ++jt) {
            PreferencePage* page = WidgetFactory().createPreferencePage((*jt).toAscii().constData());
            tabWidget->addTab(page, page->windowTitle());
            page->loadSettings();
        }
    }

    // show the first group
    listBox->setCurrentRow(0);
}

void DlgPreferencesImp::changeGroup(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;
    tabWidgetStack->setCurrentIndex(listBox->row(current));
}

/**
 * Adds a preference page with its class name \a className and
 * the group \a group it belongs to. To create this page it must
 * be registered in the WidgetFactory.
 * @see WidgetFactory
 * @see PrefPageProducer
 */
void DlgPreferencesImp::addPage(const QString& className, const QString& group)
{
    for (QList<QPair<QString, QStringList> >::iterator it = _pages.begin(); it != _pages.end(); ++it) {
        if (it->first == group) {
            it->second.push_back(className);
            return;
        }
    }

    QStringList pages;
    pages << className;
    _pages.push_back(qMakePair(group, pages));
}

/**
 * Activates the page at position \a pos of the group with name \a groupName.
 */
//void DlgPreferencesImp::activatePageOfGroup( int pos, const char* groupName )
//{
//  QMap<QString, int>::ConstIterator it = _mGroupIDs.find( groupName );
//  if ( it != _mGroupIDs.end() )
//  {
//    QTabWidget* tab = getPreferenceGroup( it.value() );
//    listBox->setCurrentItem( it.value() );
//    tab->setCurrentIndex( pos );
//  }
//}

void DlgPreferencesImp::accept()
{
    this->invalidParameter = false;
    on_buttonApply_clicked();
    if (!this->invalidParameter)
        QDialog::accept();
}

void DlgPreferencesImp::on_buttonApply_clicked()
{
    try {
        for (int i=0; i<tabWidgetStack->count(); i++) {
            QTabWidget* tabWidget = (QTabWidget*)tabWidgetStack->widget(i);
            for (int j=0; j<tabWidget->count(); j++) {
                QWidget* page = tabWidget->widget(j);
                int index = page->metaObject()->indexOfMethod("checkSettings()");
                try {
                    if (index >= 0) {
                        page->qt_metacall(QMetaObject::InvokeMetaMethod, index, 0);
                    }
                } catch (const Base::Exception& e) {
                    listBox->setCurrentRow(i);
                    tabWidget->setCurrentIndex(j);
                    QMessageBox::warning(this, tr("Wrong parameter"), e.what());
                    throw;
                }
            }
        }
    } catch (const Base::Exception&) {
        this->invalidParameter = true;
        return;
    }

    for (int i=0; i<tabWidgetStack->count(); i++) {
        QTabWidget* tabWidget = (QTabWidget*)tabWidgetStack->widget(i);
        for (int j=0; j<tabWidget->count(); j++) {
            PreferencePage* page = qobject_cast<PreferencePage*>(tabWidget->widget(j));
	  	    if (page)
                page->saveSettings();
        }
    }
}

void DlgPreferencesImp::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        retranslateUi(this);
        // update the widget's tabs
        for (int i=0; i<tabWidgetStack->count(); i++) {
            QTabWidget* tabWidget = (QTabWidget*)tabWidgetStack->widget(i);
            for (int j=0; j<tabWidget->count(); j++) {
                QWidget* page = tabWidget->widget(j);
                tabWidget->setTabText(j, page->windowTitle());
            }
        }
    } else {
        QWidget::changeEvent(e);
    }
}

#include "moc_DlgPreferencesImp.cpp"
