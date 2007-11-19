//---------------------------------------------------------------------------
#include "PreCompiled.h"

#include "CaptureClass.h"

//---------------------------------------------------------------------------


	/// Constructor with an Capture number, 0 will ask for one
Capturerer::Capturerer(int num)
: capture(NULL),captureImage(0),_bIsWinOn(false)
{
  capture = cvCaptureFromCAM( num );

  if( !capture )
	throw "Cant create capture device";

    hScale=0.5;
    vScale=0.5;
    lineWidth=1;
    useLabel=true;

    // Init font
    cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX/*|CV_FONT_ITALIC*/, hScale,vScale,0,lineWidth);


}
	/// Constructor with an File name. 	Object will capture from that Video file
Capturerer::Capturerer(const char* fileName)
: capture(NULL), captureImage(0),_bIsWinOn(false)
{
   capture = cvCaptureFromAVI( fileName );

   if( !capture )
	throw "Cant create capture device";

}

Capturerer::~Capturerer()
{
	if(_bIsWinOn)
		cvDestroyWindow("Capture");
    if(capture)
        cvReleaseCapture(&capture);
}


void Capturerer::setCaptureWindows(bool On)
{
	if(!_bIsWinOn && On)
	{
		cvNamedWindow( "Capture", 0 );
        _bIsWinOn = true;
	}
	if(_bIsWinOn && !On)
	{
		cvDestroyWindow("Capture");
        _bIsWinOn = false;
	}
}


char Capturerer::getOneCapture(const char *text)
{
    static int i = 0;
    // Get frame
    IplImage* frame = NULL;
	frame = cvQueryFrame( capture );

    if( !frame )
		throw "Cannot get frame";

    if(! captureImage)
        size = cvGetSize(frame);
        captureImage = cvCreateImage( size, 8, 3 );
    
     // copy memory frame to image
	cvCopy( frame, captureImage, 0 );

    // Flip
	cvFlip(captureImage, captureImage);

    // label
    if (text)
        cvPutText (captureImage,text, cvPoint(0,size.height - 5) , &font, cvScalar(0,255,0));


	if(_bIsWinOn)
	  cvShowImage( "Capture", captureImage );

    return cvWaitKey(1);

}

