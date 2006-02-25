/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
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

#ifndef _PreComp_
# include <qapplication.h>
# include <qobjectlist.h>
# include <qmap.h>
# include <qmetaobject.h>
# include <qstrlist.h>
# include <qvariant.h>
#endif

#include "Translator.h"
#include "LanguageFactory.h"

#include "../../Base/Console.h"

using namespace Gui;

/** \defgroup i18n Internationalization with FreeCAD
 *
 * The internationalization of FreeCAD makes heavy use of the internationalization
 * support of Qt. For more details refer to your Qt documentation.
 * 
 * \section stepbystep Step by step
 * To integrate a new language into FreeCAD or one of its application modules
 * you have to perform the following steps:
 *
 * \subsection tsfile Creation of a .ts file
 * First you have to generate a .ts file for the language to be translated. You can do this
 * by running the \a lupdate tool in the \a bin path of your Qt installation. As argument
 * you can specify either all related source files and the .ts output file or a Qt project 
 * file (.pro) which contains all relevant source files.
 *
 * \subsection translate Translation into your language
 * To translate the english string literals into the language you want to support you can open your
 * .ts file with \a QtLinguist and translate all literals by hand. Another way
 * for translation is to use the tool \a tsauto from Sebastien Fricker.This tool uses the 
 * engine from Google web page (www.google.com). tsauto supports the languages
 * \li english
 * \li french
 * \li german
 * \li italian
 * \li portuguese and
 * \li spanish
 *
 * \remark To get most of the literals translated you should have removed all
 * special characters (like &, !, ?, ...). Otherwise the translation could fail.
 *
 * \subsection usets Integration of the .ts file
 * The .ts file should now be integrated into the GUI library (either of FreeCAD
 * itself or its application module). The .ts file will be embedded into the
 * resulting binary file. So, at runtime you don't need any .ts or .qm files any
 * more. Indeed you will have a bigger binary file but you haven't any troubles
 * concerning missing .ts or .qm files.
 *
 * To integrate the .ts file into the Visual Studio project file you have to add the
 * file to the project and define the following as custom build:
 *
 * python ..\\Tools\\qembed.py "$(InputDir)\$(InputName).ts" "$(InputDir)\$(InputName).h" "$(InputName)" 
 * For the gcc build system you just have to add the line <Outputofyourtsfile>.h to the BUILT_SOURCES
 * sources section of the Makefile.am, run automake and configure (or ./confog.status) afterwards.
 *
 * Finally, you have to add a few lines to resource.cpp.
 * \code
 * 
 * #include "Language/<Outputofyourtsfile>.h"
 *
 * Gui::LanguageFactorySupplier::LanguageFactorySupplier()
 * {
 *   Gui::BitmapFactory();
 * 
 *   new Gui::LanguageProducer("<your language>", <functionname()>);
 * }
 * 
 * \endcode
 *
 * That's all.
 */

QPtrList<Translator> Translator::_instances;

Translator::Translator( const QString& lang, QObject * parent, const char * name )
: QTranslator( parent, name ), _language( lang )
{
//  QT_TR_NOOP( "dummy" ); // this is just a dummy entry to force an entry in .ts file by lupdate
  _instances.append( this );
}

Translator::~Translator()
{
  _instances.remove( this );
}

/**
 * Returns language.
 */
const QString& Translator::language() const
{
  return _language;
}

/** 
 * Searches for the source text of the messgage \a msg.
 * If none is found, \a msg is returned.
 */
QString Translator::findSourceText( const QString& msg ) const
{
  QString result = msg;

  QValueList<QTranslatorMessage> msgs = LanguageFactory().messages( _language );
  for ( QValueList<QTranslatorMessage>::Iterator it = msgs.begin(); it != msgs.end(); it++ )
  {
    if ( msg == (*it).translation() )
    {
      if ( (*it).sourceText() != 0 )
        result = (*it).sourceText();
      break;
    }
  }

  return result;
}

/** 
 * Searches for the original messgage of \a msg over all installed translators.
 * The function returns after the first found message. If none is found, \a msg is returned.
 */
QString Translator::getFindSourceText( const QString& msg )
{
  if ( msg.isEmpty() )
	  return msg;

  Translator* mf;
  QString res;
  for ( mf = _instances.first(); mf; mf = _instances.next() )
  {
    res = mf->findSourceText( msg );
    if ( res != msg )
      return res;
	}

  return msg;
}

/**
 * Creates the one and only language factory instance if not already created and installs
 * the matching translator objects to the specified language in the preferences.
 */
void Translator::installLanguage()
{
  LanguageFactory();
}

/**
 * \remark It seems to be sufficient to update QAction objects and QMenuBar items only.
 * The languageChange() method of Qt designer generated dialogs is called automatically.
 * \bug When we try to update all kind of QObject class and have pressed the "Apply" button
 * in Gui::Dialog::DlgPreferencesImp the text of all spin boxes disappears and value of 
 * Gui::FloatSpinBox objects is set to 0. Hence we update QAction and QMenuBar objects only.
 */
bool Translator::setLanguage ( const QString& lang )
{
  clear();

  bool ok = LanguageFactory().installLanguage( lang );
/*
  QWidget* mw = qApp->mainWidget();

  // try to update all relevant properties
  if ( mw )
  {

    if ( !mw ) return ok;
    QObjectList  *list = mw->queryList("QAction");
    if ( !list) return ok;

    QObjectListIt it( *list );         // iterate over the widgets
    QObject * obj;
    while ( (obj=it.current()) != 0 ) {  // for each widget...
      ++it;
      updateProperty( obj );
    }

    delete list; // delete the list, not the objects
  }
*/
  return ok;
}

/** 
 * Searches for all properties holding QString by searching for in its meta object
 * and reassigns them.
 */
void Translator::updateProperty( QObject* obj )
{
  // get all properties for this object
  bool super = true;
  QMetaObject* meta = obj->metaObject();
  QStrList obj_prop;
  obj_prop = meta->propertyNames( super );
  for ( QStrList::Iterator prop = obj_prop.begin(); prop != obj_prop.end(); ++prop ) 
  {
    // search for QString properties
    QVariant var = obj->property( *prop );
    if ( var.type() == QVariant::String )
    {
      int idx = meta->findProperty( *prop, super );
      // get more information about the property
      const QMetaProperty* mp = meta->property( idx, super );
      if ( mp && mp->writable() )
      {
        obj->setProperty( (*prop), tr( var.toString() ) );
/*
#ifdef FC_DEBUG
        Base::Console().Log("Update '%s'\n  Class: %s\n  Property: %s\n  Type: %s\n", 
            obj->name(), obj->className(), *prop, QVariant::typeToName(var.type()) );
#endif*/
      }
    }
  }
}

/**
 * Uninstalls all translators.
 */
void Translator::clear()
{
  // Note: Use here a "while" instead of a "for" loop since we remove the translator 
  // in its destructor from the global list, otherwise the iterator will become invalid
  Translator* mf;
  while ( _instances.count() > 0 )
  {
    mf = _instances.first();
    qApp->removeTranslator( mf );
    // remove also from the list
    delete mf;
  }
}

/** 
 * Returns the language of the currently installed translator. If none is
 * installed QString::null is returned.
 */
QString Translator::currentLanguage()
{
  Translator* t = _instances.last();
  if ( t )
    return t->language();

  return QString::null;
}

#include "moc_Translator.cpp"
