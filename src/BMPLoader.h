/*BMPLoader by Chris Backhouse, April 2005
  Takes a filename, returns an array of RGB pixel data

  Loads:
  24bit bitmaps
  256 colour bitmaps
  16 colour bitmaps
  2 colour bitmaps  (Currently not working correctly)

  This code is designed for use in openGL programs, so bitmaps not correctly padded will not
  load properly, I believe this only applies to: 
  256cols if width is not a multiple of 4
  16cols if width is not a multiple of 8
  2cols if width is not a multiple of 32

  Sample code:

	BMPClass bmp;
	BMPLoad(fname,bmp);
	glTexImage2D(GL_TEXTURE_2D,0,3,bmp.width,bmp.height,0,GL_RGB,GL_UNSIGNED_BYTE,bmp.bytes);

  You may use this code in non-commercial programs, and may modify it (especially if you
  manage to fix the 2cols mode) but I would appreciate a note crediting me and (to feed my
  ego) being told if you use it in something cool: cjbackhouse@hotmail.com
*/

#ifndef BMPLOADER_H
#define BMPLOADER_H

#include <string>
typedef unsigned char BYTE;

class BMPClass
{
public:
	BMPClass();
	~BMPClass();
	BYTE& pixel(int x,int y,int c);
	void allocateMem();
	int width,height;
	BYTE* bytes;			//OpenGL formatted pixels
};

#define BMPError char
#define BMPNOTABITMAP 'b'	//Possible error flags
#define BMPNOOPEN 'o'
#define BMPFILEERROR 'f'
#define BMPNOERROR '\0'
#define BMPUNKNOWNFORMAT 'u'

//Loads the bmp in fname, and puts the data in bmp
BMPError BMPLoad(std::string fname,BMPClass& bmp);

//Translates my error codes into English	
std::string TranslateBMPError(BMPError err);	

//Load and select in OpenGL
BMPError BMPLoadGL(std::string fname);

#endif