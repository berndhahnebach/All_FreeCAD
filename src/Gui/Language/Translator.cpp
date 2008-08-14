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

#include "Translator.h"

#include <Base/Console.h>
#include <Base/Console.h>
#include <Base/Parameter.h>
#include <App/Application.h>

using namespace Gui;

/** \defgroup i18n Internationalization with FreeCAD
 *
 * The internationalization of FreeCAD makes heavy use of the internationalization
 * support of Qt. For more details refer to your Qt documentation.
 * 
 * \section stepbystep Step by step
 * To integrate a new language into FreeCAD or one of its application modules
 * you have to perform the following steps:
 *
 * \subsection tsfile Creation of a .ts file
 * First you have to generate a .ts file for the language to be translated. You can do this
 * by running the \a lupdate tool in the \a bin path of your Qt installation. As argument
 * you can specify either all related source files and the .ts output file or a Qt project 
 * file (.pro) which contains all relevant source files.
 *
 * \subsection translate Translation into your language
 * To translate the english string literals into the language you want to support you can open your
 * .ts file with \a QtLinguist and translate all literals by hand. Another way
 * for translation is to use the tool \a tsauto from Sebastien Fricker.This tool uses the 
 * engine from Google web page (www.google.com). tsauto supports the languages
 * \li english
 * \li french
 * \li german
 * \li italian
 * \li portuguese and
 * \li spanish
 *
 * \remark To get most of the literals translated you should have removed all
 * special characters (like &, !, ?, ...). Otherwise the translation could fail.
 * After having translated all literals you can load the .ts file into QtLinguist and 
 * invoke the menu item \a Release which generates the binary .qm file.
 *
 * \subsection usets Integration of the .qm file
 * The .qm file should now be integrated into the GUI library (either of FreeCAD
 * itself or its application module). The .qm file will be embedded into the
 * resulting binary file. So, at runtime you don't need any .qm files any
 * more. Indeed you will have a bigger binary file but you haven't any troubles
 * concerning missing .qm files.
 *
 * To integrate the .qm file into the executable you have to create a resource file (.qrc), first.
 * This is an XML file where you can append the .qm file. For the .qrc file you have to define the following
 * curstom build step inside the Visual Studio project file:
 *
 * Command Line: rcc.exe -name $(InputName) $(InputPath) -o "$(InputDir)qrc_$(InputName).cpp"
 * Outputs:      $(InputDir)qrc_$(InputName).cpp
 * 
 * For the gcc build system you just have to add the line <resourcefile>.qrc to the BUILT_SOURCES
 * sources section of the Makefile.am, run automake and configure (or ./confog.status) afterwards.
 *
 * Finally, you have to add a the line
 * \code
 * 
 * Q_INIT_RESOURCE(resource);
 *
 * \endcode
 * 
 * where \a resource is the name of the .qrc file. That's all!
 */

/* TRANSLATOR Gui::Translator */

Translator* Translator::_pcSingleton = 0;

Translator* Translator::instance(void)
{
    if (!_pcSingleton)
        _pcSingleton = new Translator;
    return _pcSingleton;
}

void Translator::destruct (void)
{
    if (_pcSingleton)
        delete _pcSingleton;
    _pcSingleton=0;
}

Translator::Translator()
{
    // This is needed for Qt's lupdate
    mapLanguageTopLevelDomain[QT_TR_NOOP("English" )] = "en";
    mapLanguageTopLevelDomain[QT_TR_NOOP("German"  )] = "de";
    mapLanguageTopLevelDomain[QT_TR_NOOP("French"  )] = "fr";
    mapLanguageTopLevelDomain[QT_TR_NOOP("Italian" )] = "it";
    mapLanguageTopLevelDomain[QT_TR_NOOP("Japanese")] = "jp";
    mapLanguageTopLevelDomain[QT_TR_NOOP("Chinese" )] = "cn";
    activatedLanguage = "English";
}

Translator::~Translator()
{
    removeTranslators();
}

TStringList Translator::supportedLanguages() const
{
    // List all .qm files
    TStringList languages;
    QDir dir(QLatin1String(":/translations"));
    for (std::map<std::string,std::string>::const_iterator it = mapLanguageTopLevelDomain.begin();
        it != mapLanguageTopLevelDomain.end(); ++it) {
        QString filter = QString::fromAscii("*_%1.qm").arg(QLatin1String(it->second.c_str()));
        QStringList fileNames = dir.entryList(QStringList(filter), QDir::Files, QDir::Name);
        if (!fileNames.isEmpty())
            languages.push_back(it->first);
    }

    return languages;
}

void Translator::activateLanguage (const char* lang)
{
    removeTranslators(); // remove the currently installed translators
    this->activatedLanguage = lang;
    TStringList languages = supportedLanguages();
    if (std::find(languages.begin(), languages.end(), lang) != languages.end()) {
        std::map<std::string, std::string>::iterator tld = mapLanguageTopLevelDomain.find(lang);
        QString filter = QString::fromAscii("*_%1.qm").arg(QLatin1String(tld->second.c_str()));
        QDir dir(QLatin1String(":/translations"));
        QStringList fileNames = dir.entryList(QStringList(filter), QDir::Files, QDir::Name);
        for (QStringList::Iterator it = fileNames.begin(); it != fileNames.end(); ++it){
            QTranslator* translator = new QTranslator;
            translator->setObjectName(*it);
            if (translator->load(dir.filePath(*it))) {
                qApp->installTranslator(translator);
                translators.push_back(translator);
            }
            else {
                delete translator;
            }
        }
    }
}

std::string Translator::activeLanguage() const
{
    return this->activatedLanguage;
}

/**
 * This method checks for newly added (internal) .qm files which might be added at runtime. This e.g. happens if a plugin
 * gets loaded at runtime. For each newly added files that supports the currently set language a new translator object is created 
 * to load the file.
 */
void Translator::refresh()
{
    std::map<std::string, std::string>::iterator tld = mapLanguageTopLevelDomain.find(this->activatedLanguage);
    if (tld == mapLanguageTopLevelDomain.end())
        return; // no language activated
    QString filter = QString::fromAscii("*_%1.qm").arg(QLatin1String(tld->second.c_str()));
    QDir dir(QLatin1String(":/translations"));
    QStringList fileNames = dir.entryList(QStringList(filter), QDir::Files, QDir::Name);
    for (QStringList::Iterator it = fileNames.begin(); it != fileNames.end(); ++it){
        bool ok=false;
        for (std::list<QTranslator*>::const_iterator tt = translators.begin();
            tt != translators.end(); ++tt) {
            if ((*tt)->objectName() == *it) {
                ok = true; // this file is already installed
                break;
            }
        }

        // okay, we need to install this file
        if (!ok) {
            QTranslator* translator = new QTranslator;
            translator->setObjectName(*it);
            if (translator->load(dir.filePath(*it))) {
                qApp->installTranslator(translator);
                translators.push_back(translator);
            }
            else {
                delete translator;
            }
        }
    }
}

/**
 * Uninstalls all translators.
 */
void Translator::removeTranslators()
{
    for (std::list<QTranslator*>::iterator it = translators.begin(); it != translators.end(); ++it) {
        qApp->removeTranslator(*it);
        delete *it;
    }

    translators.clear();
}

#include "moc_Translator.cpp"
