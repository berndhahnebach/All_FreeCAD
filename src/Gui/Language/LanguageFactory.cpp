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


#include "LanguageFactory.h"
#include "Translator.h"
//#include "linguist/metatranslator.h"
#include "../Base/Console.h"
#include "../Base/Parameter.h"
#include "../App/Application.h"

using namespace Gui;

LanguageFactoryInst* LanguageFactoryInst::_pcSingleton = NULL;

LanguageFactoryInst& LanguageFactoryInst::instance(void)
{
  if (_pcSingleton == NULL)
  {
    _pcSingleton = new LanguageFactoryInst;
    // make sure that producers are created
    LanguageFactorySupplier::instance();
  }
  return *_pcSingleton;
}

void LanguageFactoryInst::destruct (void)
{
  // remove all translators
  Translator::removeLanguage();
  
  if (_pcSingleton)
    delete _pcSingleton;
  _pcSingleton=0;

  LanguageFactorySupplier::destruct();
}

LanguageProducer::LanguageProducer (const QString& language, const unsigned char* data, const unsigned int& len)
{
  mLanguageData.data = data;
  mLanguageData.size = len;

  LanguageFactoryInst& f = LanguageFactoryInst::instance();
  f.installProducer(language, this);
}

void LanguageFactoryInst::installProducer (const QString& language, Base::AbstractProducer *pcProducer)
{
  QString id = createUniqueID(language);
  AddProducer(id.latin1(), pcProducer);

  // install the registered module if it provides the current language
  ParameterGrp::handle hPGrp = App::GetApplication().GetUserParameter().GetGroup("BaseApp");
  hPGrp = hPGrp->GetGroup("Preferences")->GetGroup("General");
  QString lang = hPGrp->GetASCII("Language", "English").c_str();
  if ( lang == language )
  {
    installTranslator( id );
  }
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
{/*
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
  return ok;*/
  // TODO rewrite
  //Q3ValueList<QTranslatorMessage> msgs = messages( lang );
  bool ok=false;
 /* if ( msgs.size() > 0 )
  {
    QTranslator* t = new Translator( lang );
    for ( Q3ValueList<QTranslatorMessage>::Iterator it = msgs.begin(); it != msgs.end(); it++ )
	    t->insert( *it );
    t->squeeze( QTranslator::Stripped );
    qApp->installTranslator( t );
    ok = true;
  }*/
  return ok;
}
/*
Q3ValueList<QTranslatorMessage> LanguageFactoryInst::messages( const QString& lang ) const
{
  Q3ValueList<QTranslatorMessage> msgs;
  LanguageEmbed* tv = (LanguageEmbed*)Produce(lang.latin1());

  if ( tv )
  {
    QByteArray buf;
    QBuffer out(buf);

    if ( out.open( QIODevice::WriteOnly ) )
    {
      out.writeBlock((const char*)tv->data, tv->size);

      // all messages written
      out.close();

      // build the translator messages
      MetaTranslator mt;
      if ( out.size() > 0 )
        mt.load( &out );

      Q3ValueList<MetaTranslatorMessage> mtmsg = mt.messages();
      for ( Q3ValueList<MetaTranslatorMessage>::Iterator it2 = mtmsg.begin(); it2 != mtmsg.end(); it2++ )
      {
        msgs.append( *it2 );
      }
    }
  }

  return msgs;
}
*/

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

void* LanguageProducer::Produce (void) const
{
  return (void*)(&mLanguageData);
}

LanguageFactorySupplier* LanguageFactorySupplier::_pcSingleton = 0L;

LanguageFactorySupplier & LanguageFactorySupplier::instance(void)
{
  // not initialized?
  if(!_pcSingleton)
  {
    _pcSingleton = new LanguageFactorySupplier;
  }

  return *_pcSingleton;
}

void LanguageFactorySupplier::destruct(void)
{
  delete _pcSingleton;
  _pcSingleton = 0;
}

// TODO Replace
//#include "linguist/metatranslator.cpp"
