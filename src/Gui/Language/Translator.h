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


#ifndef TRANSLATOR_H__
#define TRANSLATOR_H__

#include <qptrlist.h>
#include <qtranslator.h>

namespace Gui {

/**
 * The Translator class is an extension to Qt's QTranslator class.
 * The class provides the possibility to get the original source text
 * of an already translated text. Furthermore it provides a static method
 * to change language on the fly.
 * For more details see the \link i18n.html Internationalization with FreeCAD \endlink
 * documentation.
 *
 * \author Werner Mayer
 */
class Translator : public QTranslator
{
  Q_OBJECT

public:
  Translator( const QString& lang, QObject * parent = 0, const char * name = 0 );
  virtual ~Translator();

  const QString& language() const;
  QString findSourceText( const QString& ) const;

  static QString getFindSourceText( const QString& );
  static bool setLanguage ( const QString& );
  static void installLanguage();
  static QString currentLanguage();

private:
  static void clear();
  static void updateProperty( QObject* obj );

  static QPtrList<Translator> _instances; /**< A list of all created instances */ 
  QString _language;
};

} // namespace Gui

#endif // TRANSLATOR_H__
