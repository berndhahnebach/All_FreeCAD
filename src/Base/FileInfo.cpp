/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2005                        *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        *
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/


#include "PreCompiled.h"

#ifndef _PreComp_
# include <assert.h>
# include <stdio.h>
# include <fstream>
# if defined (FC_OS_LINUX) || defined(FC_OS_CYGWIN)
# include <unistd.h>
# include <sys/stat.h>
# elif defined (FC_OS_WIN32)
# include <direct.h>
# include <io.h>
# include <windows.h>
# endif
#endif


#include "FileInfo.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#define new DEBUG_CLIENTBLOCK

using namespace Base;
using namespace std;


FileInfo::FileInfo (const char* _FileName)
{
    setFile(_FileName);
}

FileInfo::FileInfo (const std::string &_FileName)
{
	setFile(_FileName.c_str());
}

const char *FileInfo::getTempPath(void)
{
    static std::string tempPath;

    if (tempPath == "") {
#ifdef FC_OS_WIN32
        char buf[MAX_PATH + 2];
        GetTempPath(MAX_PATH + 1,buf);
        tempPath = buf;
#else
        const char* tmp = getenv("TMPDIR");
        tempPath = tmp ? tmp : "/tmp/";
#endif
    }

    return tempPath.c_str();
}

std::string FileInfo::getTempFileName(void)
{
    //FIXME: To avoid race conditons we should rather return a file pointer
    //than a file name.
#ifdef FC_OS_WIN32
    char buf[MAX_PATH + 2];
    // this already creates the file
    GetTempFileName(getTempPath(),"FCTempFile",0,buf);
    return string(buf);
#else
    char buf[PATH_MAX+1];
    strncpy(buf, getTempPath(), PATH_MAX);
    buf[PATH_MAX] = 0; // null termination needed
    strcat(buf, "/fileXXXXXX");
    /*int id =*/ mkstemp(buf);
    //FILE* file = fdopen(id, "w");
    return std::string(buf);
#endif
}

void FileInfo::setFile(const char* name)
{
    std::string result;
    const char *It=name;

    while(*It != '\0') {
        switch(*It)
        {
        case '\\':
            result += "/";
            break;
        default:
            result += *It;
        }
        It++;
    }

    FileName = result;
}

std::string FileInfo::filePath () const
{
    return FileName;
}

std::string FileInfo::fileName () const
{
    return FileName.substr(FileName.find_last_of('/')+1);
}

std::string FileInfo::dirPath () const
{
    return FileName.substr(0,FileName.find_last_of('/'));
}

std::string FileInfo::fileNamePure () const
{
    std::string temp = fileName();
    std::string::size_type pos = temp.find_last_of('.');
  
    if (pos != string::npos)
        return temp.substr(0,pos);
    else 
        return temp;
}

#ifdef FC_OS_WIN32
std::wstring ConvertToWideString(const std::string& string)
{
    int neededSize = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, 0, 0);
    wchar_t* wideCharString = new wchar_t[neededSize];
    MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, wideCharString, neededSize);
    std::wstring wideString(wideCharString);
    delete [] wideCharString;
    wideCharString = NULL;
    return wideString;
}
#endif

std::wstring FileInfo::toStdWString() const
{
    // As FileName is UTF-8 is encoded we have to convert it
    // for Windows because the path names are UCS-2 encoded.
#ifdef FC_OS_WIN32
    return ConvertToWideString(FileName);
#else
    // FIXME: For MacOS the path names are UCS-4 encoded.
    // For the moment we cannot handle path names containing
    // non-ASCII characters.
    // For Linux the paths names are encoded in UTF-8 so we actually
    // don't need this method therefore.
    return std::wstring();
#endif
}

std::string FileInfo::extension (bool complete) const
{
    // complete not implemented
    assert(complete==false);
    return FileName.substr(FileName.find_last_of('.')+1);
}

bool FileInfo::hasExtension (const char* Ext) const
{
#if defined (_MSC_VER)
    return _stricmp(Ext,extension().c_str()) == 0;
#elif defined (__GNUC__)
    return strcasecmp(Ext,extension().c_str()) == 0;
#endif
}

bool FileInfo::exists () const
{
#if defined (_MSC_VER)
    std::wstring wstr = toStdWString();
    return _waccess(wstr.c_str(),0) == 0;
#elif defined (__GNUC__)
    return access(FileName.c_str(),0) == 0;
#endif
}

bool FileInfo::isReadable () const
{
#if defined (_MSC_VER)
    std::wstring wstr = toStdWString();
    return _waccess(wstr.c_str(),4) == 0;
#elif defined (__GNUC__)
    return access(FileName.c_str(),4) == 0;
#endif
}

bool FileInfo::isWritable () const
{
#if defined (_MSC_VER)
    std::wstring wstr = toStdWString();
    return _waccess(wstr.c_str(),2) == 0;
#elif defined (__GNUC__)
    return access(FileName.c_str(),2) == 0;
#endif
}

bool FileInfo::isFile () const
{
#ifdef _MSC_VER
    if (exists()) {
        std::wstring wstr = toStdWString();
        FILE* fd = _wfopen(wstr.c_str(), L"rb");
        bool ok = (fd != 0);
        fclose(fd);
        return ok;
    }
#else
    if (exists()) {
        // If we can open it must be an existing file, otherwise we assume it
        // is a directory (which doesn't need to be true for any cases)
        std::ifstream str(FileName.c_str(), std::ios::in | std::ios::binary);
        if (!str) return false;
        str.close();
        return true;
    }
#endif

    // TODO: Check for valid file name
    return true;
}

bool FileInfo::isDir () const
{
    if (exists()) {
        // if we can chdir then it must be a directory, otherwise we assume it
        // is a file (which doesn't need to be true for any cases)
#if defined (_MSC_VER)
        wchar_t cwd[MAX_PATH+1];
        std::wstring wstr = toStdWString();
        if (_wgetcwd(cwd,MAX_PATH+1) != 0 && _wchdir(wstr.c_str()) == 0) {
            _wchdir(cwd);
            return true;
        }
#elif defined (__GNUC__)
        char cwd[PATH_MAX+1];
        if (getcwd(cwd,PATH_MAX+1) != 0 && chdir(FileName.c_str()) == 0) {
            chdir(cwd);
            return true;
        }
#endif
        return false;
    }

    // TODO: Check for valid path name
    return true;
}

unsigned int FileInfo::size () const
{
    // not implemented
    assert(0);
    return 0;
}

bool FileInfo::deleteFile(void)
{
#if defined (_MSC_VER)
    std::wstring wstr = toStdWString();
    return ::_wremove(wstr.c_str()) == 0;
#elif defined (__GNUC__)
    return (::remove(FileName.c_str())==0);
#else
#   error "FileInfo::deleteFile() not implemented for this platform!"
#endif
}

bool FileInfo::createDirectory(const char* directory) const
{
#if defined (_MSC_VER)
    FileInfo fi(directory);
    std::wstring wstr = fi.toStdWString();
    return _wmkdir(wstr.c_str()) == 0;
#elif defined (__GNUC__)
    return mkdir(directory, 0777) == 0;
#else
#   error "FileInfo::createDirectory() not implemented for this platform!"
#endif
}

int FileInfo::RunFile(void) const
{
#if defined (_MSC_VER)
    std::wstring wstr = this->toStdWString();
    return (int)ShellExecuteW(NULL, NULL, wstr.c_str(), NULL, NULL, SW_SHOWNORMAL);
#elif defined (__GNUC__)
#   error "FileInfo::RunFile() not implemented for this platform!"
#else
#   error "FileInfo::RunFile() not implemented for this platform!"
#endif

}

int FileInfo::RunFile(const char* FileName)
{
#if defined (_MSC_VER)
    return (int)ShellExecute(NULL, NULL, FileName, NULL, NULL, SW_SHOWNORMAL);
#elif defined (__GNUC__)
#   error "FileInfo::RunFile() not implemented for this platform!"
#else
#   error "FileInfo::RunFile() not implemented for this platform!"
#endif

}
