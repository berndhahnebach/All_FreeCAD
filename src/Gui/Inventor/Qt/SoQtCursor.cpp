/**************************************************************************\
 *
 *  This file is part of the Coin GUI toolkit libraries.
 *  Copyright (C) 1998-2002 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  version 2.1 as published by the Free Software Foundation.  See the
 *  file LICENSE.LGPL at the root directory of this source distribution
 *  for more details.
 *
 *  If you want to use this library with software that is incompatible
 *  licensewise with the LGPL, and / or you would like to take
 *  advantage of the additional benefits with regard to our support
 *  services, please contact Systems in Motion about acquiring a Coin
 *  Professional Edition License.  See <URL:http://www.coin3d.org> for
 *  more information.
 *
 *  Systems in Motion, Prof Brochs gate 6, 7030 Trondheim, NORWAY
 *  <URL:http://www.sim.no>, <mailto:support@sim.no>
 *
\**************************************************************************/

/*!
  \class SoQtCursor
  \brief The SoQtCursor class is used to set cursors for GUI components.
  \ingroup misc
*/
// includes the FreeCAD configuration
#include "../../../Config.h"

#include <Inventor/Qt/SoQtCursor.h>
#include <assert.h>

/*!
  \enum SoQtComponent::Shape
  For enumerating built-in shape types.
*/


/*!
  \var SoQtCursor::Shape So@Cui@Cursor::DEFAULT
  The Window system's default cursor.
*/

/*!
  \var SoQtCursor::Shape So@Cui@Cursor::WAIT
  A busy cursor.
*/

/*!
  \var SoQtCursor::Shape So@Cui@Cursor::CUSTOM_BITMAP
  A custom cursor.
*/

/*** Graphic data for the mouse pointer bitmaps. ***********************/

// Dimensions can be up to <32, 32> pixels.
//
// B=1 and M=1 == black
// B=0 and M=1 == white
// B=0 and M=0 == transparency
// B=1 and M=0 will give undefined results for some platforms, so avoid.

/*** zoom-style cursor ******/

#define ZOOM_WIDTH 16
#define ZOOM_HEIGHT 16
#define ZOOM_BYTES ((ZOOM_WIDTH + 7) / 8) * ZOOM_HEIGHT
#define ZOOM_HOT_X 5
#define ZOOM_HOT_Y 7

static unsigned char zoom_bitmap[ZOOM_BYTES] =
{
  0x00, 0x0f, 0x80, 0x1c, 0x40, 0x38, 0x20, 0x70,
  0x90, 0xe4, 0xc0, 0xcc, 0xf0, 0xfc, 0x00, 0x0c,
  0x00, 0x0c, 0xf0, 0xfc, 0xc0, 0xcc, 0x90, 0xe4,
  0x20, 0x70, 0x40, 0x38, 0x80, 0x1c, 0x00, 0x0f
};

static unsigned char zoom_mask_bitmap[ZOOM_BYTES] =
{
 0x00,0x0f,0x80,0x1f,0xc0,0x3f,0xe0,0x7f,0xf0,0xff,0xf0,0xff,0xf0,0xff,0x00,
 0x0f,0x00,0x0f,0xf0,0xff,0xf0,0xff,0xf0,0xff,0xe0,0x7f,0xc0,0x3f,0x80,0x1f,
 0x00,0x0f
};

/*** pan-style cursor *******/

#define PAN_WIDTH 16
#define PAN_HEIGHT 16
#define PAN_BYTES ((PAN_WIDTH + 7) / 8) * PAN_HEIGHT
#define PAN_HOT_X 7
#define PAN_HOT_Y 7

static unsigned char pan_bitmap[PAN_BYTES] =
{
  0xc0, 0x03, 0x60, 0x02, 0x20, 0x04, 0x10, 0x08,
  0x68, 0x16, 0x54, 0x2a, 0x73, 0xce, 0x01, 0x80,
  0x01, 0x80, 0x73, 0xce, 0x54, 0x2a, 0x68, 0x16,
  0x10, 0x08, 0x20, 0x04, 0x40, 0x02, 0xc0, 0x03
};

static unsigned char pan_mask_bitmap[PAN_BYTES] =
{
 0xc0,0x03,0xe0,0x03,0xe0,0x07,0xf0,0x0f,0xe8,0x17,0xdc,0x3b,0xff,0xff,0xff,
 0xff,0xff,0xff,0xff,0xff,0xdc,0x3b,0xe8,0x17,0xf0,0x0f,0xe0,0x07,0xc0,0x03,
 0xc0,0x03
};

/*** rotate-style cursor ****/

#define ROTATE_WIDTH 16
#define ROTATE_HEIGHT 16
#define ROTATE_BYTES ((ROTATE_WIDTH + 7) / 8) * ROTATE_HEIGHT
#define ROTATE_HOT_X 6
#define ROTATE_HOT_Y 8

static unsigned char rotate_bitmap[ROTATE_BYTES] = {
  0xf0, 0xef, 0x18, 0xb8, 0x0c, 0x90, 0xe4, 0x83,
  0x34, 0x86, 0x1c, 0x83, 0x00, 0x81, 0x00, 0xff,
  0xff, 0x00, 0x81, 0x00, 0xc1, 0x38, 0x61, 0x2c,
  0xc1, 0x27, 0x09, 0x30, 0x1d, 0x18, 0xf7, 0x0f
};

static unsigned char rotate_mask_bitmap[ROTATE_BYTES] = {
 0xf0,0xef,0xf8,0xff,0xfc,0xff,0xfc,0xff,0x3c,0xfe,0x1c,0xff,0x00,0xff,0x00,
 0xff,0xff,0x00,0xff,0x00,0xff,0x38,0x7f,0x3c,0xff,0x3f,0xff,0x3f,0xff,0x1f,
 0xf7,0x0f
};

/*** just a blank cursor ****/

#define BLANK_WIDTH 1
#define BLANK_HEIGHT 1
#define BLANK_BYTES ((BLANK_WIDTH + 7) / 8) * BLANK_HEIGHT
#define BLANK_HOT_X 0
#define BLANK_HOT_Y 0

static unsigned char blank_bitmap[BLANK_BYTES] = { 0x00 };
static unsigned char blank_mask_bitmap[BLANK_BYTES] = { 0x00 };

/***********************************************************************/


static SoQtCursor::CustomCursor zoom;
static SoQtCursor::CustomCursor pan;
static SoQtCursor::CustomCursor rotate;
static SoQtCursor::CustomCursor blank;
static SbBool first = TRUE;


/*!
  Default constructor. Creates a default cursor.
*/
SoQtCursor::SoQtCursor(void)
{
  this->commonConstructor(SoQtCursor::DEFAULT, NULL);
}

/*!
  Constructor with Shape argument.
*/
SoQtCursor::SoQtCursor(const Shape shape)
{ 
  assert(shape != CUSTOM_BITMAP && "don't use this constructor for making CUSTOM_BITMAP cursors");
  this->commonConstructor(shape, NULL);
}

/*!
  Constructs a new custom bitmap cursor from \a cc.

  Note that only a \e shallow copy will be made of the CustomCursor
  bitmap and mask references, so don't deallocate the memory they use
  until the SoQtCursor has been destructed.
*/
SoQtCursor::SoQtCursor(const CustomCursor * cc)
{ 
  this->commonConstructor(CUSTOM_BITMAP, cc);
}

// Private common constructor.
void
SoQtCursor::commonConstructor(const Shape shape, const CustomCursor * cc)
{ 
  this->shape = shape;
  this->cc = NULL;

  if (first) {
    zoom.dim = SbVec2s(ZOOM_WIDTH, ZOOM_HEIGHT);
    zoom.hotspot = SbVec2s(ZOOM_HOT_X, ZOOM_HOT_Y);
    zoom.bitmap = zoom_bitmap;
    zoom.mask = zoom_mask_bitmap;

    pan.dim = SbVec2s(PAN_WIDTH, PAN_HEIGHT);
    pan.hotspot = SbVec2s(PAN_HOT_X, PAN_HOT_Y);
    pan.bitmap = pan_bitmap;
    pan.mask = pan_mask_bitmap;

    rotate.dim = SbVec2s(ROTATE_WIDTH, ROTATE_HEIGHT);
    rotate.hotspot = SbVec2s(ROTATE_HOT_X, ROTATE_HOT_Y);
    rotate.bitmap = rotate_bitmap;
    rotate.mask = rotate_mask_bitmap;

    blank.dim = SbVec2s(BLANK_WIDTH, BLANK_HEIGHT);
    blank.hotspot = SbVec2s(BLANK_HOT_X, BLANK_HOT_Y);
    blank.bitmap = blank_bitmap;
    blank.mask = blank_mask_bitmap;

    first = FALSE;
  }

  if (cc) {
    assert(shape == CUSTOM_BITMAP);
    this->cc = new CustomCursor;
    *(this->cc) = *cc;
  }
}


/*!
  Destructor
*/
SoQtCursor::~SoQtCursor()
{
  delete this->cc;
}

/*!
  Returns the shape type.

  If the cursor has been set by the application programmer to a bitmap
  cursor, SoQtCursor::CUSTOM_BITMAP is returned.
*/
SoQtCursor::Shape 
SoQtCursor::getShape(void) const
{
  return this->shape;
}

/*!
  Sets the cursor to a predefined shape.
*/
void 
SoQtCursor::setShape(const Shape shape)
{
  this->shape = shape;
}

/*!
  Returns a reference to the current custom bitmap cursor.

  Do not call this method unless SoQtCursor::getShape() returns
  SoQtCursor::CUSTOM_BITMAP.
*/
const SoQtCursor::CustomCursor &
SoQtCursor::getCustomCursor(void) const
{
  assert(cc!=NULL && "not a custom bitmap cursor");
  return *(this->cc);
}

/*!
  Returns a "zoom" indicator cursor.
*/
const SoQtCursor &
SoQtCursor::getZoomCursor(void)
{
  static SoQtCursor * zoomcursor = NULL;
  // FIXME: memory leak. 20011120 mortene.
  if (!zoomcursor) { zoomcursor = new SoQtCursor(&zoom); }
  return *zoomcursor;
}

/*!
  Returns a cursor with "pan" graphics (ie for translation in the
  camera normal plane).
*/
const SoQtCursor &
SoQtCursor::getPanCursor(void)
{
  static SoQtCursor * pancursor = NULL;
  // FIXME: memory leak. 20011120 mortene.
  if (!pancursor) { pancursor = new SoQtCursor(&pan); }
  return *pancursor;
}

/*!
  Returns a "rotate" indicator cursor.
*/
const SoQtCursor &
SoQtCursor::getRotateCursor(void)
{
  static SoQtCursor * rotatecursor = NULL;
  // FIXME: memory leak. 20011120 mortene.
  if (!rotatecursor) { rotatecursor = new SoQtCursor(&rotate); }
  return *rotatecursor;
}

/*!
  Because all toolkits don't easily support setting up a blank cursor
  from pre-defined shapes or API functions, we also provide a simple
  completely transparent cursor.
*/
const SoQtCursor &
SoQtCursor::getBlankCursor(void)
{
  static SoQtCursor * blankcursor = NULL;
  // FIXME: memory leak. 20011120 mortene.
  if (!blankcursor) { blankcursor = new SoQtCursor(&blank); }
  return *blankcursor;
}
