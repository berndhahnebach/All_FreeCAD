/***************************************************************************
                          imageconv.cpp  -  description
                             -------------------
    begin                : Die Apr 23 21:02:14 CEST 2002
    copyright            : (C) 2002 by Werner Mayer
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   Werner Mayer 2002                                                     *
 *                                                                         *
 ***************************************************************************/

#include "imageconv.h"
#include <qstringlist.h>
#include <qbuffer.h>
#include <qtextstream.h>
#include <qimage.h>
#include <iostream>

CCmdLineParser::CCmdLineParser(int argc, char** argv)
{
  SplitLine(argc, argv);
}

CCmdParam CCmdLineParser::GetArgumentList(const char* pSwitch)
{
  if (HasSwitch(pSwitch))
  {
    CCmdLineParser::iterator theIterator;

    theIterator = find(pSwitch);
	  if (theIterator!=end())
    {
      return (*theIterator).second;
    }
  }

  CCmdParam param;
  return param;
}

// ------------------------------------------------------------

QString CImageConvApp::m_Executable = "ImageConv";

CImageConvApp::CImageConvApp(const QString& nameFilter, const QString& sFile)
{
  m_Output = sFile;
  if (nameFilter.isEmpty())
    m_Dir.setNameFilter("*.png;*.bmp;*.xbm;*.pnm;*.jpg;*.jpeg;*.mng;*.gif"); // not "*.xpm"!
  else
    m_Dir.setNameFilter(nameFilter);
}

void CImageConvApp::SetOutputFile(const QString& sFile) 
{
  m_Output = sFile;
}

void CImageConvApp::SetNameFilter(const QString& nameFilter)
{
  m_Dir.setNameFilter(nameFilter);
}

bool CImageConvApp::Save(const QString& fn)
{
  int iPos = fn.find(".");

  QString ext  = fn.mid(iPos+1);  // extension of filename
  QString name = fn.mid(0,iPos);  // filename without extension

  if ( !m_clPixmap.isNull() ) 
  {
    if ( !fn.isEmpty() )
    {
      return m_clPixmap.save( fn, ext.upper().latin1());
    }
  }

  return false;
}

bool CImageConvApp::Load(const QString& fn)
{
  const char* ext = QPixmap::imageFormat(fn);

  if ( !fn.isEmpty() )
    return m_clPixmap.load( fn, ext);

  return false;
}

const QPixmap& CImageConvApp::GetPixmap() const
{
  return m_clPixmap;
}

bool CImageConvApp::ConvertToXPM(bool bAppendToFile)
{
  QStringList list = m_Dir.entryList();

  // print to the console
  cout << "Try converting to XPM..." << endl;
  if (list.count() == 0)
  {
    cout << "Cannot find " << m_Dir.nameFilter().latin1() << endl;
    return false;
  }

  for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) 
  {
    const char* ext = QPixmap::imageFormat(*it);
    if (ext == 0L)
      continue; // no image format

    if (m_Output == *it)
      continue; // if the file is the output file itself

    cout << "Converting " << (*it).latin1() << " ...";
    
    if (Load(*it) == true)
    {
      QString name(*it);
      name.replace(name.find(".")+1, 4, "xpm");

      bool ok;

      QFileInfo fi(*it);
      if ( bAppendToFile )
        ok = AppendToFile(fi.baseName());
      else
        ok = Save(name);

      if ( ok )
        cout << "Done" << endl;
      else
        cout << "failed" << endl;
    }
    else
    {
      cout << "failed" << endl;
    }
  }

  return true;
}

bool CImageConvApp::AppendToFile(const QString& file)
{
  QString ohead("static char");
  QString nhead("static const char");

  // save as XPM into tmp. buffer
  QCString str;
  QBuffer buf(str);
  buf.open ( IO_WriteOnly );
  QImageIO iio(&buf, "XPM");
  QImage im;
  im = m_clPixmap;
  iio.setImage(im);
  iio.write();
  buf.close();

  // convert to string and make changes
  QString txt = str;
  txt.replace(ohead, nhead);
  txt.replace(QString("dummy"), file);

  // open file
  bool found = false;
  bool empty = false;
  QFile fw(m_Output);
  if ( fw.open( IO_ReadOnly ))
  {
    QTextStream tr (&fw);
    QString line;
    do 
    {
      line = tr.readLine();
      if ((line.find(file)) != -1) // icon alreeady registered
      {
        found = true;
      }
    } while (!tr.eof() && !found);  

    fw.close();
  }
  else
  {
    // empty file
    //
    empty = true;
    QTextStream tw (&fw);
    if (!fw.open( IO_Translate | IO_Raw | IO_WriteOnly ))
      return false;

    // write header stuff
    tw << "\n";
    tw << "#include <qmap.h>\n";
    tw << "#include <qstring.h>\n";
    tw << "\n";
    tw << "QMap <QString, QString> mRegIcons;\n";
    tw << "\n";
    tw << "void RegisterIcon(const QString& icon)\n";
    tw << "{\n";
    tw << "  mRegIcons[icon] = icon;\n";
    tw << "}\n";
    tw << "\n";
    fw.close();
  }

  // register new icon
  if (!found)
  {
    if (!fw.open( IO_Translate | IO_Raw | IO_ReadWrite | IO_Append ))
      return false;

    // write into file now
    QTextStream tw (&fw);
    tw << txt << "RegisterIcon(" << file << ");\n\n";
    fw.close();
  }

  return true;
}

void CImageConvApp::Error()
{
  cerr << "Usage: " << m_Executable.latin1() << " [OPTION(S)] -i input file(s) {-o output file}" << endl;
  cerr << "Try '"   << m_Executable.latin1() << " --help' for more information." << endl;

  exit(0);
}

void CImageConvApp::Version()
{
  cerr << m_Executable.latin1() << " 1.0.0 " << endl;
  exit(0);
}

void CImageConvApp::Usage()
{
  cerr << "Usage: " << m_Executable.latin1() << " [OPTION(S)] -i input file(s) {-o output file}\n" << endl;
  cerr << "Options:" << endl;

  cerr << "  -i       \tSpecify the input file(s).\n"
          "           \tSeveral filenames must be separated by a blank.\n"
          "           \tIf you want to select all files of a format\n"
          "           \tyou also can write \"*.[FORMAT]\" (e.g. *.png).\n"  
          "           \tSpecifying several files only makes sense in\n" 
          "           \taddition with -a or -x." << endl;

  cerr << "  -o       \tSpecify the output file." << endl;
  
  cerr << "  -x, --xpm\tConvert all specified image files to XPM.\n"
          "           \tFor each specified image file a corresponding\n"
          "           \tXPM file will be created.\n"
          "           \tWith -i you can specify the input files." << endl;
  
  cerr << "  -a, --append\tConvert all specified image files to XPM and\n"
          "           \tappend the result to the file specified with -o.\n"
          "           \tThis is a special mode to add icons to the FreeCAD's\n"
          "           \tbitmap factory automatically.\n"
          "           \tWith -i you can specify the input files.\n" << endl;
 
  cerr << "  -v, --version\tPrint the version and exit." << endl;

  cerr << "  -h, --help\tPrint this message and exit.\n" << endl;
  
  cerr << "This program supports the following image formats:\n"
          " BMP, GIF, JPEG, MNG, PNG, PNM, XBM and XPM\n\n"
          << m_Executable.latin1() << " uses Qt Version " << qVersion() << "\n"
          "Qt can be downloaded at http://www.trolltech.com." << endl;

  exit(0);
}
