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

LanguageFactoryInst* LanguageFactoryInst::_pcSingleton = NULL;

LanguageFactoryInst& LanguageFactoryInst::Instance(void)
{
  if (_pcSingleton == NULL)
    _pcSingleton = new LanguageFactoryInst;
  return *_pcSingleton;
}

void LanguageFactoryInst::Destruct (void)
{
  if (_pcSingleton != NULL)
    delete _pcSingleton;
}

void LanguageFactoryInst::setLanguage (const QString& sName)
{
  // make sure that producers are created
  LanguageFactorySupplier::Instance();

  try
  {
    QStringList IDs = getUniqueIDs(sName);

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

QString LanguageFactoryInst::createUniqueID(const QString& sName)
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

QStringList LanguageFactoryInst::getUniqueIDs(const QString& sName)
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

QStringList LanguageFactoryInst::getRegisteredLanguages() const
{
  QStringList sl;
  for (std::map<QString, int>::const_iterator it = _mCntLanguageFiles.begin(); it != _mCntLanguageFiles.end(); ++it)
  {
    sl.append(it->first);
  }

  return sl;
}

// ----------------------------------------------------

LanguageProducer::LanguageProducer (const QString& language, const std::vector<const char*>& languageFile)
 : mLanguageFile(languageFile)
{
  LanguageFactoryInst& f = LanguageFactoryInst::Instance();
  f.AddProducer(f.createUniqueID(language).latin1(), this);
}

void* LanguageProducer::Produce (void) const
{
  return (void*)(&mLanguageFile);
}

LanguageFactorySupplier* LanguageFactorySupplier::_pcSingleton = 0L;

LanguageFactorySupplier & LanguageFactorySupplier::Instance(void)
{
  // not initialized?
  if(!_pcSingleton)
  {
    _pcSingleton = new LanguageFactorySupplier;
  }

  return *_pcSingleton;
}

#include "linguist/metatranslator.cpp"
