//---------------------------------------------------------------------------

#ifdef _MSC_VER
#pragma hdrstop
#endif

#include "CaptureClass.h"

//---------------------------------------------------------------------------

#ifdef _MSC_VER
#pragma package(smart_init)
#endif


	/// Constructor with an Capture number, 0 will ask for one
Capturerer::Capturerer(int num)
: capture(NULL),captureImage(0),_bIsWinOn(false)
{
  capture = cvCaptureFromCAM( num );

  if( !capture )
	throw "Cant create capture device";


}
	/// Constructor with an File name. 	Object will capture from that Video file
Capturerer::Capturerer(const char* fileName)
: capture(NULL), captureImage(0),_bIsWinOn(false)
{
   capture = cvCaptureFromAVI( fileName );

   if( !capture )
	throw "Cant create capture device";

}

void Capturerer::setCaptureWindows(bool On)
{
	if(!_bIsWinOn && On)
	{
		cvNamedWindow( "Capture", 0 );
	}
	if(_bIsWinOn && !On)
	{
		cvDestroyWindow("Capture");
	}
}


void Capturerer::getOneCapture(void)
{
    // Get frame
    IplImage* frame = NULL;
	frame = cvQueryFrame( capture );
  
    if( !frame )
		throw "Cannot get frame";

     // copy memory frame to image
	cvCopy( frame, captureImage, 0 );

    // Flip
	cvFlip(captureImage, captureImage);

	if(_bIsWinOn)
	  cvShowImage( "Capture", captureImage );

}

