



#include <string>
using namespace std;

class ImageData{

public:
	ImageData();
	ImageData(const char *isFileName);
	~ImageData();

	/*This changes the image to an RGBA format pixels.
	  By Default the value is stored as 1.So by default every image is stored as a Background image.
	  The Pointer returned shooudl noot be released as it is already released by the image.
	*/
	unsigned char*GetPixelsinRGBA();
	//int UpdateImage(ImageData *pImage); //We shoudl not use it I feel
	int GetWidth();
	int GetHeight();
	int GetNbOfChannels();
	string GetFileName();
	
	/* this method reads the pixels the image and changes the value of alpha based upon the value
	 of HSV value or the Green color.
	 This method has to release the pixels that it has which is indicated by a prefix of r_
	*/
	unsigned char *r_GetAlphaMask();

	unsigned char*r_GetAssociatedImage();

	unsigned char*r_Compose(ImageData *pBGImage, int &oFinalWidth, int &oFinalheight);

	void WriteImage(char *ipfilename);

private:
	unsigned char *_prgbapixels;
	string _sfilename;
	int _iwidth;
	int _iheight;
	int _inbChannels;

};