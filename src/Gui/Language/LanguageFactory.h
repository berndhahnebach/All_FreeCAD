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


#ifndef __LANGUAGE_FACTORY_H__
#define __LANGUAGE_FACTORY_H__

#include <Base/Factory.h>

#ifndef _PreComp_
# include <qtranslator.h>
# include <qvaluelist.h>
# include <vector>
#endif

namespace Gui {

/** The LanguageFactoryInst singleton
 * \author Werner Mayer
 */
class GuiExport LanguageFactoryInst : public Base::Factory
{
public:
  /** @name singleton stuff */
  //@{
  /// Creates an instance
  static LanguageFactoryInst& Instance(void);
  /// Destroys the instance
  static void Destruct (void);
  //@}

  /** Installs the specified language \a lang if available */
  bool installLanguage ( const QString& lang ) const;
  /** Gets all translated messages to language \a lang. */
  QValueList<QTranslatorMessage> messages( const QString& lang ) const;
  /**
   * Creates a unique ID (QString) for the specified language.
   * This makes possible to register several files for the same language for example
   * if you have files coming from several application modules.
   */
  QString createUniqueID(const QString& sName);
  /** Returns all IDs to the specified language. */
  QStringList getUniqueIDs(const QString& sName) const;
  /** Returns a list of all registered languages. */
  QStringList getRegisteredLanguages() const;

private:
  static LanguageFactoryInst* _pcSingleton;

  LanguageFactoryInst(){}
  ~LanguageFactoryInst(){}

  bool installTranslator ( const QString& lang ) const;

  /// several files for the same language (important for language files from modules)
  std::map<QString, int> _mCntLanguageFiles;

  // friends
  friend class LanguageProducer;
};

/** Returns the global LanguageFactoryInst object */
inline GuiExport LanguageFactoryInst& LanguageFactory(void)
{
  return LanguageFactoryInst::Instance();
}

// --------------------------------------------------------------------

struct LanguageEmbed {
  const char *name;
  const unsigned char *data;
  unsigned int size;
};

/** Retrieves the translated literals to the specified language.
 * \author Werner Mayer
 */
class GuiExport LanguageProducer : public Base::AbstractProducer
{
public:
  /**
   * Appends itself to the language factory.
   * @see LanguageFactoryInst
   */
  LanguageProducer (const QString& language, const unsigned char* data, const unsigned int& len);

  virtual ~LanguageProducer (void){}

  /// Returns the generated language file
  virtual void* Produce (void) const;

private:
  /// Contains all translations
  LanguageEmbed mLanguageData;
};

// --------------------------------------------------------------------

/**
 * The language factory supplier class.
 * \author Werner Mayer
 */
class GuiExport LanguageFactorySupplier
{
private:
  /**
   * You can register new languages generated from ts file here
   */
  LanguageFactorySupplier();
  static LanguageFactorySupplier *_pcSingleton;

public:
  static LanguageFactorySupplier &Instance(void);
};

} // namespace Gui

#endif // __LANGUAGE_FACTORY_H__
