/***************************************************************************
                          Process.h  -  description
                             -------------------
    begin                : 2002/10/19 10:47:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file Process.h
 *  \brief 
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



#ifndef __PROCESS__H__
#define __PROCESS__H__

class BaseExport FCProcess
{
  public:
    bool RunProcess (const char* psCom, ...);
    bool AppendToPath (const char* path);
    void SetEnvironment (const char* var, const char* val);
    void UnsetEnvironment (const char* var);

    static FCProcess& Instance();
    static void Destruct();

  private:
    FCProcess();
    ~FCProcess();

    static FCProcess* pclSingleton;
};

inline FCProcess &GetProcessor(void)
{
	return FCProcess::Instance();
}

#endif //__PROCESS__H__
