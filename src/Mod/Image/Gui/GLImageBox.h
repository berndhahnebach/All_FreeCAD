/***************************************************************************
 *                                                                         *
 *   This is a QGLWidget displaying an image or portion of an image in a   *
 *   box.                                                                  *
 *                                                                         *
 *   Author:    Graeme van der Vlugt                                       *
 *   Copyright: Imetric 3D GmbH                                            *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Jürgen Riegel 2002                                                    *
 *                                                                         *
 ***************************************************************************/

#ifndef GLIMAGEBOX_H
#define GLIMAGEBOX_H

#include <qgl.h>
#include "../App/ImageBase.h"


namespace ImageGui
{

class GLImageBox : public QGLWidget
{
    Q_OBJECT

public:

    GLImageBox(QWidget* parent, const char* name, const QGLWidget* shareWidget=0);
    ~GLImageBox();

    Image::ImageBase *getImageBasePtr() { return &_image; }

    void redraw();

    int getImageSample(int x, int y, unsigned short sampleIndex, double &value);
    unsigned short getImageNumSamplesPerPix();
    int getImageFormat();

    void fixBasePosCurr();
    double getZoomFactor() { return _zoomFactor; }
    void setZoomFactor(double zoomFactor, bool useCentrePt = false, int ICx = 0, int ICy = 0);
    void zoom(int power, bool useCentrePt = false, int ICx = 0, int ICy = 0);
    void stretchToFit();
    void setNormal();
    void getCentrePoint(int &ICx, int &ICy);
    void relMoveWC(int WCdx, int WCdy);

    double WCToIC_X(double WidgetX);
    double WCToIC_Y(double WidgetY);
    double ICToWC_X(double ImageX);
    double ICToWC_Y(double ImageY);

    void clearImage();
    int createImageCopy(void* pSrcPixelData, unsigned long width, unsigned long height, int format);
    int pointImageTo(void* pSrcPixelData, unsigned long width, unsigned long height, int format, bool takeOwnership);

    void clearIntensityMap();
    int createIntensityMap(int numEntriesReq = 0, bool Initialise = true);
    int getNumIntensityMapEntries() const { return _numMapEntries; }
    int setIntensityMapValue(int index, float value);

signals:
  void drawGraphics();

private:

    void initializeGL();
    void paintGL();
    void resizeGL( int w, int h );

    void drawImage();
    void getDisplayedImageAreaSize(int &dx, int &dy);

    void getPixFormat(GLenum &pixFormat, GLenum &pixType);
    void limitCurrPos();
    void limitZoomFactor();
    void setCurrPos(int x0, int y0);
    void resetDisplay();
    int calcNumColorMapEntries();

    Image::ImageBase _image;   // the image data

    int _x0;            // image x-coordinate of top-left widget pixel
    int _y0;            // image y-coordinate of top-left widget pixel
    double _zoomFactor; // zoom factor = (num_widget_pixels / num_image_pixels)

    int _base_x0;       // defines a fixed position of x0
    int _base_y0;       // defines a fixed position of y0

    float* _pIntensityMap;  // an intensity map (to alter the brightness and contrast)
    int _numMapEntries;     // number of entries in intensity map

};


} // namespace ImageGui

#endif // GLIMAGEBOX_H
