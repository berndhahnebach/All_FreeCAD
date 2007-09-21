//---------------------------------------------------------------------------

#ifndef CaptureClassH
#define CaptureClassH


#include <cv.h>
#include <highgui.h>

//---------------------------------------------------------------------------

class Capturerer
{
	/// Constructor with an Capture number, 0 will ask for one
	Capturerer(int num = 0);
	/// Constructor with an File name. 	Object will capture from that Video file
	Capturerer(const char* fileName);

	void setCaptureWindows(bool On);

	void getOneCapture(void);

private:
  CvCapture* capture;
  IplImage *captureImage;
  bool _bIsWinOn;

};






#endif
