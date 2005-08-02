/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Jürgen Riegel 2002                                                    *
 *                                                                         *
 ***************************************************************************/

#include "PreCompiled.h"
#ifndef _PreComp_
#	include <qaction.h>
#	include <BRepPrimAPI_MakeBox.hxx>
#	include <TopoDS_Shape.hxx>
#	include <TNaming_Builder.hxx>
# include <qfiledialog.h>
# include <qimage.h>
# include <qmessagebox.h>
#endif

#include <Base/Exception.h>
#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/BitmapFactory.h>

#include "ImageView.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

using namespace ImageGui;

DEF_STD_CMD(CmdImageOpen);

CmdImageOpen::CmdImageOpen()
	:CppCommand("Image_Open")
{
	sAppModule		= "Image";
	sGroup			  = QT_TR_NOOP("Image");
	sMenuText		  = QT_TR_NOOP("Open");
	sToolTipText	= QT_TR_NOOP("Image open image view function");
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			  = "Open";
	iAccel			  = Qt::CTRL+Qt::Key_O;
}


void CmdImageOpen::activated(int iMsg)
{
  // Reading an image
    QString s = QFileDialog::getOpenFileName(QString::null, QObject::tr("Images (*.png *.xpm *.jpg *.bmp)"), 
                                           0, QObject::tr("Open image file dialog"), 
                                           QObject::tr("Choose an image file to open"));
  if (s.isEmpty() == false)
  {
      QImage imageq(s);
      int format;
      if (imageq.isNull() == false)
      {
        if ((imageq.depth() == 8) && (imageq.isGrayscale() == true))
          format = IB_CF_GREY8;
        else if ((imageq.depth() == 16) && (imageq.isGrayscale() == true))
          format = IB_CF_GREY16;
        else if ((imageq.depth() == 32) && (imageq.isGrayscale() == false))
          format = IB_CF_BGRA32;
        else
        {
            QMessageBox::warning(0, QObject::tr("Open"), QObject::tr("Unsupported image format"));
            return;
        }
      }
      else
      {
        QMessageBox::warning(0, QObject::tr("Open"), QObject::tr("Could not load image"));
        return;
      }

      // Displaying the image in a view
      ImageView* iView = new ImageView(Gui::ApplicationWindow::Instance, "Image");
      iView->setIcon( Gui::BitmapFactory().pixmap("colors") );
      iView->setCaption(QObject::tr("Image viewer"));
      iView->resize( 400, 300 );
      Gui::ApplicationWindow::Instance->addWindow( iView );
      iView->createImageCopy((void *)(imageq.bits()), (unsigned long)imageq.width(), (unsigned long)imageq.height(), format, 0);
  }
}

void CreateImageCommands(void)
{
  Gui::CommandManager &rcCmdMgr = Gui::ApplicationWindow::Instance->commandManager();

	rcCmdMgr.addCommand(new CmdImageOpen());
}
