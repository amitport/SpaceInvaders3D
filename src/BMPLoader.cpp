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

#include "BMPLoader.h"

BMPClass::BMPClass(){bytes=0;}
BMPClass::~BMPClass(){delete[] bytes;}
BYTE& BMPClass::pixel(int x,int y,int c){return bytes[(y*width+x)*3+c];}
void BMPClass::allocateMem(){delete[] bytes;bytes=new BYTE[width*height*3];}

std::string TranslateBMPError(BMPError err)
{
	switch(err)
	{
	case(BMPNOTABITMAP):
		return "This file is not a bitmap, specifically it doesn't start 'BM'";
	case(BMPNOOPEN):
		return "Failed to open the file, suspect it doesn't exist";
	case(BMPFILEERROR):
		return "Some kind of error while physically reading the file";
	case(BMPNOERROR):
		return "No errors detected";
	case(BMPUNKNOWNFORMAT):
		return "Unknown bmp format, ie not 24bit, 256,16 or 2 colour";
	default:
		return "Not a valid error code";
	}
}

BMPError BMPLoad(std::string fname,BMPClass& bmp)
{
	FILE * f=fopen(fname.c_str(),"rb");		//open for reading in binary mode
	if(!f) return BMPNOOPEN;
	char header[54];
	fread(header,54,1,f);			//read the 54bit main header

	if(header[0]!='B' || header[1]!='M') 
	{
		fclose(f);
		return BMPNOTABITMAP;		//all bitmaps should start "BM"
	}

	if(sizeof(int)==4)	//Annoyingly I can't just assume this
	{
		bmp.width=*(int*)(header+18);
		bmp.height=*(int*)(header+22);
	}
	else
	{
		bmp.width=header[18]+256*header[19]+256*256*header[20]+256*256*256*header[21];
		bmp.height=header[22]+256*header[23]+256*256*header[24]+256*256*256*header[25];
	}
	bmp.allocateMem();

	int bits=int(header[28]);		//colourdepth

	int x,y,c;
	BYTE cols[256*4];
	switch(bits)
	{
	case(24):
		fread(bmp.bytes,bmp.width*bmp.height*3,1,f);	//24bit is easy
		for(x=0;x<bmp.width*bmp.height*3;x+=3)			//except the format is BGR, grr
		{
			BYTE temp=bmp.bytes[x];
			bmp.bytes[x]=bmp.bytes[x+2];
			bmp.bytes[x+2]=temp;
		}
		break;

	case(8):
		fread(cols,256*4,1,f);							//read colortable
		for(y=0;y<bmp.height;++y)						//(Notice 4bytes/col for some reason)
			for(x=0;x<bmp.width;++x)
			{
				BYTE byte;			
				fread(&byte,1,1,f);						//just read byte					
				for(int c=0;c<3;++c)
					bmp.pixel(x,y,c)=cols[byte*4+2-c];	//and look up in the table
			}
		break;

	case(4):
		fread(cols,16*4,1,f);
		for(y=0;y<256;++y)
			for(x=0;x<256;x+=2)
			{
				BYTE byte;
				fread(&byte,1,1,f);						//as above, but need to exract two
				for(c=0;c<3;++c)						//pixels from each byte
					bmp.pixel(x,y,c)=cols[byte/16*4+2-c];
				for(c=0;c<3;++c)
					bmp.pixel(x+1,y,c)=cols[byte%16*4+2-c];
			}
		break;

	//NOT CURRENTLY WORKING PROPERLY
	//case(1):
	//	fread(cols,8,1,f);		//colourtable
	//	for(y=0;y<256;++y)
	//		for(x=0;x<256;x+=8)
	//		{
	//			BYTE byte;
	//			fread(&byte,1,1,f);
	//			//The idea here is that every byte is eight pixels
	//			//so I'm shifting the byte to the relevant position, then masking out
	//			//all but the lowest bit in order to get the index into the colourtable.
	//			for(int x2=0;x2<8;++x2)
	//			{
	//				for(int c=0;c<3;++c)
	//					bmp.pixel(x+x2,y,c)=cols[((byte>>(7-x2))&1)*4+2-c];
	//			}
	//		}
	//	break;
	default:
		fclose(f);
		return BMPUNKNOWNFORMAT;
	}

	fclose(f);
	if(ferror(f)) return BMPFILEERROR;

	return BMPNOERROR;
}

BMPError BMPLoad(std::string fname){BMPClass bmp;return BMPLoad(fname,bmp);}

#ifdef __gl_h
BMPError BMPLoadGL(std::string fname)
{
	BMPClass bmp;
	BMPError e=BMPLoad(fname,bmp);
	if(e!=BMPNOERROR) return e;
		
	glEnable(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,3,bmp.width,bmp.height,0,GL_RGB,GL_UNSIGNED_BYTE,bmp.bytes);

	return BMPNOERROR;
}
#endif