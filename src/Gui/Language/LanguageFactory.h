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


#ifndef __FC_LANGUAGE_FACTORY_H__
#define __FC_LANGUAGE_FACTORY_H__

#include "../Base/Factory.h"

/** The LanguageFactory singleton
  */
class GuiExport FCLanguageFactory : public Base::Factory
{
	public:
		static FCLanguageFactory& Instance(void);
		static void Destruct (void);

		void SetLanguage (const QString& sName);
		QString CreateUniqueID(const QString& sName);
		QStringList GetUniqueIDs(const QString& sName);
		QStringList GetRegisteredLanguages() const;

	private:
		static FCLanguageFactory* _pcSingleton;

		FCLanguageFactory(){}
		~FCLanguageFactory(){}

		/// several files for the same language (important for language files from modules)
		std::map<QString, int> _mCntLanguageFiles;
};

inline GuiExport FCLanguageFactory& GetLanguageFactory(void)
{
	return FCLanguageFactory::Instance();
}

// --------------------------------------------------------------------

class GuiExport FCLanguageProducer : public Base::AbstractProducer
{
	public:
		/// Constructor
		FCLanguageProducer (const QString& language, const std::vector<const char*>& languageFile);

		virtual ~FCLanguageProducer (void){}

		/// Produce an instance
		virtual void* Produce (void) const;

	private:
		const std::vector<const char*>& mLanguageFile;
};

// --------------------------------------------------------------------

/** 
 * The language factory supplier class
 */
class GuiExport FCLanguageFactorySupplier
{
  private:
	  // Singleton
	  FCLanguageFactorySupplier();
  	static FCLanguageFactorySupplier *_pcSingleton;

  public:
  	static FCLanguageFactorySupplier &Instance(void);
};

#endif // __FC_LANGUAGE_FACTORY_H__
