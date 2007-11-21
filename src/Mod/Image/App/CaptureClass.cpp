//---------------------------------------------------------------------------
#include "PreCompiled.h"

#include "CaptureClass.h"

#include <cvcam.h>
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

int Capturerer::chooseCamNum(void)
{
    int ncams = cvcamGetCamerasCount( );//returns the number of available cameras in the system
    //printf("Number of Cams: %d\n",ncams);
    int* out;
    if(ncams >1){
        int nselected = cvcamSelectCamera(&out);
        if(nselected>0)
            printf("the 1-st selected camera is camera number %d", out[0]);
        if(nselected == 2)
            printf("the 2-nd selected camera is camera number %d", out[1]);
    }else if (ncams < 1){
        printf("No camara in system! Terminating.\n");
        return -1;
    }else
        out = new int(0);

    return *out;
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

