// CBitMap.cpp
#include "Stdafx.h"

#include <CBitMap.h>

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <CUtil.h>
#include <CVector.h>
#include <memory.h>



void CBitMap::Create(int w, int h) {
    if (data!=0)
      delete[] data;
    data = new unsigned char[3*w*h];
    for (int i=0;i<3*w*h;i++)
      data[i] = 0;
    width = w;
    height = h;
  }


void CBitMap::SaveBMP(string filename) {
  
  CBMPHeader b;
  
  b.type = 19778;
  b.offset = 54;
  
  b.filesize = width*height*3 + 54; // file size
  b.reserved2=0;
  b.size = 40;
  b.width = width; 
  b.height = height;
  b.planes = 1;
  b.bits=24;
  b.compression = 0; // 0-compressed
  b.imagesize=0; // 0 for compressed
  
  int winWidth = width;
  int winHeight = height;
  
  
  ofstream f(filename.c_str(), ios::out | ios::binary);
  
  int n = 0;
  
  f.write ((char *)&b.type, 2 );
  f.write ((char *)&b.filesize, 4 );
  f.write ((char *)&b.reserved2, 2 );
  f.write ((char *)&b.reserved2, 2 );
  f.write ((char *)&b.offset, 4 );
  f.write ((char *)&b.size, 4 ); // 14
  f.write ((char *)&b.width, 4 );  // 18
  f.write ((char *)&b.height, 4 );  // 22
  f.write ((char *)&b.planes, 2 );  // 24
  f.write ((char *)&b.bits, 2 );  // 26
  f.write ((char *)&b.compression, 4 ); //30
  f.write ((char *)&b.imagesize, 4 ); // 34
  f.write ((char *)&n, 4 );  //38
  f.write ((char *)&n, 4 );  // 42
  f.write ((char *)&n, 4 );  //46
  f.write ((char *)&n, 4 ); // 50
  
  
  f.write ((char *)data , winWidth*winHeight*3);
  f.write ((char *)&n, 2); // 50
  
  f.close ();
  
  return;  
  
  
}



CBitMap::CBitMap(char *fname)
	: width(0), height(0), data(0)
{
	this->LoadBMP(fname);
}

CBitMap::CBitMap()
	: width(0), height(0), data(0) {}

CBitMap::~CBitMap()
{
	if( data ) {
    	delete[] data;
    }
 }

void CBitMap::LoadBMP(const char *fname)
{
	  using namespace std;

	  unsigned short planes;	// number of planes in image (must be 1) 
	  unsigned short bpp;			// number of bits per pixel (must be 24)
	  
     CUtil::verify_file(fname);
	  ifstream fin(fname, ios::in | ios::binary);
	  
	  fin.seekg(18, ios::cur);
	  
	  fin.read((char *)&width, sizeof(unsigned));
	  fin.read((char *)&height, sizeof(unsigned));
	  //cout << "width: " << width << " height: " << height << '\n';
	  
	  fin.read((char *)&planes, sizeof(unsigned short));
	  if( planes != 1 )
	    {
	      //cout << "Planes from " << fname << " is not 1: " << planes << "\n";
	      //exit(1);
          throw string("Planes in texture is incorrect: " +string(fname));
	    }
	  
	  fin.read((char *)&bpp, sizeof(unsigned short));
	  if( bpp != 24 )
	    {
          throw string("bpp is not 24 in texture: " +string(fname));
	      //cout << "Bpp from " << fname << " is not 24: " << bpp << "\n";
	      //exit(1);
	    }
	  
	  fin.seekg(24, ios::cur);
	  
	  unsigned size(width * height * 3);				// size of the image in chars (3 is to RGB component).
	  data = new unsigned char[size];
	  fin.read((char *)data, size);
	  
	  unsigned char tmp;					// temporary color storage for bgr-rgb conversion.
	  for( unsigned int i(0); i < size; i += 3 )
	    {
	      tmp = data[i];
	      data[i] = data[i+2];
	      data[i+2] = tmp;
	    }
}

unsigned char CBitMap::pixel_elem(int x, int y, int elem)
{
	int pos = (y*width+x) * 3 + elem;
	return data[pos];
}

unsigned char *CBitMap::pixel_pos(int x, int y)
{
	int pos = (y * width + x) * 3;
	return &data[pos];
}


#ifndef NO_OPENGL  

void CBitMap::LoadTGA(char *name)
{
	GLubyte		TGAheader[12]	= {0,0,2,0,0,0,0,0,0,0,0,0};// Uncompressed TGA header
	GLubyte		TGAcompare[12];								// Used to compare TGA header
	GLubyte		header[6];									// First 6 useful chars of the header
	GLuint		charsPerPixel;								// Holds the number of chars per pixel used
	GLuint		imageSize;									// Used to store the image size
	GLuint		temp;										// Temporary variable
	GLuint		type			= GL_RGBA;					// Set the default type to RBGA (32 BPP)
	GLubyte		*imageData;									// Image data (up to 32 Bits)
	GLuint		bpp;										// Image color depth in bits per pixel.

    CUtil::verify_file(name);
	FILE *file;
#ifdef _WIN32
	fopen_s(&file, name, "rb");							// Open the TGA file
#elseif
	file = fopen(name, "rb");							// Open the TGA file
#endif 
	// Load the file and perform checks
	if(file == NULL ||														// Does file exist?
	   fread(TGAcompare,1,sizeof(TGAcompare),file) != sizeof(TGAcompare) ||	// Are there 12 chars to read?
	   memcmp(TGAheader,TGAcompare,sizeof(TGAheader)) != 0				 ||	// Is it the right format?
	   fread(header,1,sizeof(header),file) != sizeof(header))				// If so then read the next 6 header chars
	{
		if (file == NULL)									// If the file didn't exist then return
			return;
		else
		{
            throw string("Error opening tga");
			fclose(file);									// If something broke then close the file and return
			return;
		}
	}

	// Determine the TGA width and height (highchar*256+lowchar)
	width  = header[1] * 256 + header[0];
	height = header[3] * 256 + header[2];
    
	// Check to make sure the targa is valid and is 24 bit or 32 bit
	if(width	<=0	||										// Is the width less than or equal to zero
	   height	<=0	||										// Is the height less than or equal to zero
	   (header[4] != 24 && header[4] != 32))				// Is it 24 or 32 bit?
	{
		fclose(file);										// If anything didn't check out then close the file and return
		return;
	}

	bpp				= header[4];							// Grab the bits per pixel
	charsPerPixel	= bpp / 8;								// Divide by 8 to get the chars per pixel
	imageSize		= width * height * charsPerPixel;		// Calculate the memory required for the data

	// Allocate the memory for the image data
	imageData		= new GLubyte[imageSize];

	// Make sure the data is allocated write and load it
	if(imageData == NULL ||									// Does the memory storage exist?
	   fread(imageData, 1, imageSize, file) != imageSize)	// Does the image size match the memory reserved?
	{
		if(imageData != NULL)								// Was the image data loaded
			free(imageData);								// If so, then release the image data

		fclose(file);										// Close the file
		return;
	}

	// Loop through the image data and swap the 1st and 3rd chars (red and blue)
	for(GLuint i = 0; i < GLuint(imageSize); i += charsPerPixel)
	{
		temp = imageData[i];
		imageData[i] = imageData[i + 2];
		imageData[i + 2] = temp;
	}

	// We are done with the file so close it
	fclose(file);

	// Set the type
	if (bpp == 24)
		type = GL_RGB;
	
	if (data)
      delete[] data;

    data = new unsigned char[width*height*4];
    memcpy(data, imageData, width*height*4);
    CVector col = CVector(0,0,0);
    for (unsigned int i=0;i<width*height;i++) {
            if (data[4*i+3]<200) {
                 data[4*i+3] = 0;
 /*                data[4*i+0] = col.x;
                 data[4*i+1] = col.y;
                 data[4*i+2] = col.z;*/
                 
               }
            else
             {
                data[4*i+3] = 255;
                col.x = data[4*i];
                col.y = data[4*i+1];
                col.z = data[4*i+2];
                    
             }
    }
    

	free(imageData);

}
#endif
