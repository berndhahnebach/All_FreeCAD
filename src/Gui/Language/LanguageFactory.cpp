/***************************************************************************
                          LanguageFactory.cpp  -  description
                             -------------------
    begin                : 2003/01/12 23:25:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file LanguageFactory.cpp
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



#include "PreCompiled.h"
#ifndef _PreComp_
# include <qapplication.h>
# include <qstring.h>
# include <qstringlist.h>
# include <qfile.h>
# include <qdir.h>
# include <vector>
# include <map>
#endif
#include "../Base/Console.h"
#include "LanguageFactory.h"
#include "linguist/metatranslator.h"

typedef std::vector<const char*>  TCharVector;
typedef std::vector<const char*>* PCharVector;
typedef const std::vector<const char*>& RCharVector;

using namespace Gui;

CLanguageFactory* CLanguageFactory::_pcSingleton = NULL;

CLanguageFactory& CLanguageFactory::Instance(void)
{
  if (_pcSingleton == NULL)
    _pcSingleton = new CLanguageFactory;
  return *_pcSingleton;
}

void CLanguageFactory::Destruct (void)
{
  if (_pcSingleton != NULL)
    delete _pcSingleton;
}

void CLanguageFactory::SetLanguage (const QString& sName)
{
	// make sure that producers are created
	CLanguageFactorySupplier::Instance();

	try
	{
		QStringList IDs = GetUniqueIDs(sName);

		for (QStringList::Iterator it = IDs.begin(); it!= IDs.end(); ++it)
		{
			PCharVector tv = (PCharVector) Produce((*it).latin1());

			if ( !tv )
			{
#ifdef FC_DEBUG
				Base::Console().Warning("\"%s\" is not registered\n", sName.latin1());
#endif
				continue; // no data
			}

			RCharVector text = *tv;

			// create temporary files
			QString fn1 = "Language.ts";
			QString fn2 = "Language.qm";
			QFile file(fn1);
			file.open(IO_WriteOnly);
			QTextStream out(&file);
			for (std::vector<const char*>::const_iterator i = text.begin(); i!=text.end(); ++i)
				out << (*i);
			file.close();

			// build the translator messages
			MetaTranslator mt;
			mt.load(fn1);
			mt.release(fn2);
			QTranslator* t = new QTranslator( 0 );
			t->load(fn2, ".");
			qApp->installTranslator(t);

			// and delete the files again
			QDir dir;
			dir.remove(fn1);
			dir.remove(fn2);
		}
	}
	catch (...)
	{
	}
}

QString CLanguageFactory::CreateUniqueID(const QString& sName)
{
	if (_mCntLanguageFiles.find(sName) != _mCntLanguageFiles.end())
	{
		_mCntLanguageFiles[sName]++;
	}
	else
	{
		_mCntLanguageFiles[sName] = 1;
	}

	QString id = QString("%1_%2").arg(sName).arg(_mCntLanguageFiles[sName]);
	return id;
}

QStringList CLanguageFactory::GetUniqueIDs(const QString& sName)
{
	QStringList sl;
	if (_mCntLanguageFiles.find(sName) != _mCntLanguageFiles.end())
	{
		int cnt = _mCntLanguageFiles[sName];
		for (int i=1; i<=cnt; ++i)
		{
			QString id = QString("%1_%2").arg(sName).arg(_mCntLanguageFiles[sName]);
			sl.append(id);
		}
	}

	return sl;
}

QStringList CLanguageFactory::GetRegisteredLanguages() const
{
	QStringList sl;
	for (std::map<QString, int>::const_iterator it = _mCntLanguageFiles.begin(); it != _mCntLanguageFiles.end(); ++it)
	{
		sl.append(it->first);
	}

	return sl;
}

// ----------------------------------------------------

CLanguageProducer::CLanguageProducer (const QString& language, const std::vector<const char*>& languageFile)
 : mLanguageFile(languageFile)
{
	CLanguageFactory& f = CLanguageFactory::Instance();
	f.AddProducer(f.CreateUniqueID(language).latin1(), this);
}

void* CLanguageProducer::Produce (void) const
{ 
	return (void*)(&mLanguageFile);
}

CLanguageFactorySupplier* CLanguageFactorySupplier::_pcSingleton = 0L;

CLanguageFactorySupplier & CLanguageFactorySupplier::Instance(void)
{
	// not initialized?
	if(!_pcSingleton)
	{
		_pcSingleton = new CLanguageFactorySupplier;
	}

  return *_pcSingleton;
}

#include "linguist/metatranslator.cpp"
