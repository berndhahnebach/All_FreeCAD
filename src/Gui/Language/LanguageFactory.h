/***************************************************************************
                          LanguageFactory.h  -  description
                             -------------------
    begin                : 2003/01/12 23:25:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file LanguageFactory.h
 *  \brief Produce widgets
 *  \author Werner Mayer
 *  \version 0.1
 *  \date    2003/01/06
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Werner Mayer 2002                                                     *
 *                                                                         *
 ***************************************************************************/


#ifndef __LANGUAGE_FACTORY_H__
#define __LANGUAGE_FACTORY_H__

#include "../Base/Factory.h"

namespace Gui {

/** The CLanguageFactory singleton
  */
class GuiExport CLanguageFactory : public Base::Factory
{
	public:
		/** @name singleton stuff */
		//@{
		/// Creates an instance
		static CLanguageFactory& Instance(void);
		/// Destroys the instance
		static void Destruct (void);
		//@}

		/** Sets the specified language if available */
		void SetLanguage (const QString& sName);
		/**
		 * Creates a unique ID (QString) for the specified language.
		 * This makes possible to register several files for the same language for example
		 * if you have files coming from several application modules.
		 */
		QString CreateUniqueID(const QString& sName);
		/** Returns all IDs to the specified language. */
		QStringList GetUniqueIDs(const QString& sName);
		/** Returns a list of all registered languages. */
		QStringList GetRegisteredLanguages() const;

	private:
		static CLanguageFactory* _pcSingleton;

		CLanguageFactory(){}
		~CLanguageFactory(){}

		/// several files for the same language (important for language files from modules)
		std::map<QString, int> _mCntLanguageFiles;
};

/** Returns the global CLanguageFactory object */
inline GuiExport CLanguageFactory& LanguageFactory(void)
{
	return CLanguageFactory::Instance();
}

// --------------------------------------------------------------------

class GuiExport CLanguageProducer : public Base::AbstractProducer
{
	public:
		/** 
		 * Appends itself to the language factory.
		 * @see CLanguageFactory
		 */
		CLanguageProducer (const QString& language, const std::vector<const char*>& languageFile);

		virtual ~CLanguageProducer (void){}

		/// Returns the generated language file
		virtual void* Produce (void) const;

	private:
		/// Contains all translations
		const std::vector<const char*>& mLanguageFile;
};

// --------------------------------------------------------------------

/** 
 * The language factory supplier class
 */
class GuiExport CLanguageFactorySupplier
{
  private:
		/**
		 * You can register new languages generated from ts file here
		 */
	  CLanguageFactorySupplier();
  	static CLanguageFactorySupplier *_pcSingleton;

  public:
  	static CLanguageFactorySupplier &Instance(void);
};

} // namespace Gui

#endif // __LANGUAGE_FACTORY_H__
