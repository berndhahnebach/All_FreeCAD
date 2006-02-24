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
# include <map>
#endif

#include "LanguageFactory.h"
#include "Translator.h"
#include "linguist/metatranslator.h"
#include "../Base/Console.h"
#include "../Base/Parameter.h"
#include "../App/Application.h"

using namespace Gui;

LanguageFactoryInst* LanguageFactoryInst::_pcSingleton = NULL;

LanguageFactoryInst& LanguageFactoryInst::Instance(void)
{
  if (_pcSingleton == NULL)
  {
    _pcSingleton = new LanguageFactoryInst;
    // make sure that producers are created
    LanguageFactorySupplier::Instance();
  }
  return *_pcSingleton;
}

void LanguageFactoryInst::Destruct (void)
{
  if (_pcSingleton != NULL)
    delete _pcSingleton;
}

bool LanguageFactoryInst::installLanguage ( const QString& lang ) const
{
  bool ok = false;

  try
  {
    QStringList IDs = getUniqueIDs( lang );

    for (QStringList::Iterator it = IDs.begin(); it!= IDs.end(); ++it)
    {
      ok |= installTranslator( *it );
    }
  }
  catch (...)
  {
  }

  return ok;
}

bool LanguageFactoryInst::installTranslator ( const QString& lang ) const
{
  LanguageEmbed* tv = (LanguageEmbed*)Produce(lang.latin1());

  bool ok=false;
  if ( !tv )
    return false; // no data

  QDir path = QDir::current();
  QFileInfo fi( path.absPath() );
  bool canDo = fi.permission( QFileInfo::WriteUser );
#ifdef FC_OS_WIN32
  const char* tmp = getenv("TMP");
  if ( canDo==false && tmp )
  {
    path.setPath( tmp );
    fi.setFile( path.absPath() );
    canDo = fi.permission( QFileInfo::WriteUser );
  }
#endif
  if ( canDo==false )
  {
    path = QDir::home();
    fi.setFile( path.absPath() );
    canDo = fi.permission( QFileInfo::WriteUser );
  }

  if ( canDo==false )
    return false; // give up

  // create temporary files
  QString ts = "Language.ts";
  fi.setFile( path, ts );
  ts = fi.absFilePath();
  QString qm = "Language.qm";
  fi.setFile( path, qm );
  qm = fi.absFilePath();
  QFile file( ts );
  if ( !file.open( IO_WriteOnly ) )
    return false;

  QTextStream out( &file );
  out.writeRawBytes((const char*)tv->data, tv->size);

  // all messages written
  file.close();

  // and delete the files again
  QDir dir;
  if ( file.size() > 0 )
  {
    // build the translator messages
    MetaTranslator mt;
    mt.load( ts );
    mt.release( qm );
    QTranslator* t = new Translator( lang );
    t->load( qm, "." );
    dir.remove( qm );

    qApp->installTranslator( t );
    ok = true;
  }

  dir.remove( ts );
  return ok;
}

QValueList<QTranslatorMessage> LanguageFactoryInst::messages( const QString& lang ) const
{
  QValueList<QTranslatorMessage> msgs;

  QDir path = QDir::current();
  QFileInfo fi( path.absPath() );
  bool canDo = fi.permission( QFileInfo::WriteUser );
#ifdef FC_OS_WIN32
  const char* tmp = getenv("TMP");
  if ( canDo==false && tmp )
  {
    path.setPath( tmp );
    fi.setFile( path.absPath() );
    canDo = fi.permission( QFileInfo::WriteUser );
  }
#endif
  if ( canDo==false )
  {
    path = QDir::home();
    fi.setFile( path.absPath() );
    canDo = fi.permission( QFileInfo::WriteUser );
  }

  if ( canDo==false )
    return msgs; // give up

  // create temporary files
  QString fn = "Language.ts";
  fi.setFile( path, fn );
  fn = fi.absFilePath();

  QFile file( fn );
  if (!file.open( IO_WriteOnly ) )
    return msgs;
  QTextStream out( &file );

  try
  {
    QStringList IDs = getUniqueIDs( lang );

    for (QStringList::Iterator it = IDs.begin(); it!= IDs.end(); ++it)
    {
      LanguageEmbed* tv = (LanguageEmbed*)((*it).latin1());

      if ( !tv )
      {
        continue; // no data
      }

      out.writeRawBytes((const char*)tv->data, tv->size);
    }

    // all messages written
    file.close();

    // build the translator messages
    MetaTranslator mt;
    if ( file.size() > 0 )
      mt.load( fn );

    // and delete the files again
    QDir dir;
    dir.remove( fn );

    QValueList<MetaTranslatorMessage> mtmsg = mt.messages();
    for ( QValueList<MetaTranslatorMessage>::Iterator it2 = mtmsg.begin(); it2 != mtmsg.end(); it2++ )
    {
      msgs.append( *it2 );
    }
  }
  catch (...)
  {
  }

  return msgs;
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

QStringList LanguageFactoryInst::getUniqueIDs(const QString& sName) const
{
  QStringList sl;
  std::map<QString, int>::const_iterator it = _mCntLanguageFiles.find( sName );
  if ( it != _mCntLanguageFiles.end() )
  {
    int cnt = it->second;
    for (int i=1; i<=cnt; ++i)
    {
      QString id = QString("%1_%2").arg(sName).arg( i );
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

LanguageProducer::LanguageProducer (const QString& language, const unsigned char* data, const unsigned int& len)
{
  mLanguageData.name = language.latin1();
  mLanguageData.data = data;
  mLanguageData.size = len;

  LanguageFactoryInst& f = LanguageFactoryInst::Instance();
  QString id = f.createUniqueID(language);
  f.AddProducer(id.latin1(), this);

  // install the registered module if it provides the current language
  ParameterGrp::handle hPGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp");
  hPGrp = hPGrp->GetGroup("Preferences")->GetGroup("General");
  QString lang = hPGrp->GetASCII("Language", "English").c_str();
  if ( lang == language )
  {
    f.installTranslator( id );
  }
}

void* LanguageProducer::Produce (void) const
{
  return (void*)(&mLanguageData);
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
