/*

	I have my model in this form. The Pixels are stored iin float format for easy usage later.
*/

#include <string>
using namespace std;


class ImageData{

private:
	unsigned char *_pPixels;
	string _sfilename;
	int _iwidth;
	int _iheight;
	int _inbChannels;

public:
	ImageData();
	ImageData(const char *isFileName);
	ImageData(ImageData &iCopy);
	~ImageData();

	/*This changes the image to an RGBA format pixels.
	  By Default the value is stored as 1.So by default every image is stored as a Background image.
	  The Pointer returned shooudl noot be released as it is already released by the image.
	*/
	unsigned char* GetPixels();

	int GetWidth();
	int GetHeight();
	int GetNbOfChannels();
	string GetFileName();

	void ReadImage(char *ipfilename);
	void WriteImage(char *ipfilename);

};