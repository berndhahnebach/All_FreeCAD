/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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

#include "SoFCInteractiveElement.h"

using namespace Gui;

SO_ELEMENT_SOURCE(SoFCInteractiveElement);

void SoFCInteractiveElement::initClass(void)
{
  SO_ELEMENT_INIT_CLASS(SoFCInteractiveElement, inherited);
  SO_ENABLE(SoGLRenderAction, SoFCInteractiveElement);
}

void SoFCInteractiveElement::init(SoState * state)
{
  this->interactiveMode = false;
}

SoFCInteractiveElement::~SoFCInteractiveElement()
{
}

void SoFCInteractiveElement::set(SoState * const state, SoNode * const node, SbBool mode)
{
  SoFCInteractiveElement * elem = (SoFCInteractiveElement *)
    SoReplacedElement::getElement(state, classStackIndex, node);
  elem->setElt(mode);
}

SbBool SoFCInteractiveElement::get(SoState * const state)
{
  return SoFCInteractiveElement::getInstance(state)->interactiveMode;
}

void SoFCInteractiveElement::setElt(SbBool mode)
{
  this->interactiveMode = mode;
}

const SoFCInteractiveElement * SoFCInteractiveElement::getInstance(SoState * state)
{
  return (const SoFCInteractiveElement *) SoElement::getConstElement(state, classStackIndex);
}
