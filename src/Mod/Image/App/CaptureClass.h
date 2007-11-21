//---------------------------------------------------------------------------

#ifndef CaptureClassH
#define CaptureClassH


#include <cv.h>
#include <highgui.h>
//---------------------------------------------------------------------------

class ImageAppExport Capturerer
{
public:
	/// Constructor with an Capture number, 0 will ask for one
	Capturerer(int num = 0);
	/// Constructor with an File name. 	Object will capture from that Video file
	Capturerer(const char* fileName);

    ~Capturerer();

    static int chooseCamNum(void);

	void setCaptureWindows(bool On);

	char getOneCapture(const char *text=0);

private:
  CvCapture* capture;
  IplImage *captureImage;
  bool _bIsWinOn;

  CvSize size;

    // font stuff
    CvFont font;
    double hScale;
    double vScale;
    int    lineWidth;
    bool   useLabel;
    char   buff[100];


};






#endif
