#include "Stdafx.h"


#include <CTexture.h>

COpenGLTexture* CTexture::addtexture(string fname, string name) {
  return addtexture(fname, name, false);
}

void CTexture::addtga(string fname, string name) {
  COpenGLTexture txt;
  CBitMap bmp;
  bmp.LoadTGA((char*)fname.c_str());
  loadtexture_tga(&bmp, &txt);
  t.push_back(txt);
  names.push_back(name);
  //txt.has_alpha = false;
}


COpenGLTexture* CTexture::addtexture(string fname, string name, bool transparent) {
  COpenGLTexture txt;
  
  CBitMap bmp;
  bmp.LoadBMP(fname.c_str());
  txt.has_alpha = false;
  if (transparent)
    loadtexture_transparent(&bmp, &txt,0);
  else
    loadtexture(&bmp, &txt);
  t.push_back(txt);
  names.push_back(name);
  return &(t[t.size()-1]);
}

GLuint CTexture::generate_texture(string name) {
       
  COpenGLTexture txt;
  glGenTextures(1, &txt.id); 
  
  txt.has_alpha = false;
  glBindTexture(GL_TEXTURE_2D, txt.id);
  
  //         glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
  
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  
  //         glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
  //         glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  t.push_back(txt);
  names.push_back(name);
  return txt.id;
}

void CTexture::create_sphere(string name, int w, CVector col1, CVector col2, double mul, bool typ, double dec) {
  CBitMap bmp;
  COpenGLTexture txt;
  double val;
  
  bmp.width = w;
  bmp.height= w;
  bmp.data = new unsigned char[3*w*w];
  
  for (int i=0;i<w; i++) {
    for (int j=0;j<w; j++) {
      double x = (i-w/2)/(double)w;
      double y = (j-w/2)/(double)w;
      
      if (typ) 
	val = mul/sqrt(x*x + y*y);            
      else
	val = mul*sqrt(x*x + y*y);            
      
      
      if (typ) {
	val = (val + 0.9*(1-sqrt(x*x + y*y)))*0.7;
      }
      val = val - 4*dec;
      
      if (val>1.0) val=1.0;
      if (val<0.00) val=0.0;
      bmp.data[3*i + 3*w*j  +0] = (unsigned char)(255.0*(col1.x * val + col2.x*(1-val)));
      bmp.data[3*i + 3*w*j  +1] = (unsigned char)(255.0*(col1.y * val + col2.y*(1-val)));
      bmp.data[3*i + 3*w*j  +2] = (unsigned char)(255.0*(col1.z * val + col2.z*(1-val)));
    }   
  }
  
  loadtexture(&bmp, &txt);
  t.push_back(txt);
  names.push_back(name);
  
}


void CTexture::loadtexture(CBitMap* bmp, COpenGLTexture* txt) {
  
  
  //if (txt->id!=0)
    glGenTextures(1, &txt->id); 


    //cout << "\n\nGenerating new texture with id : " << txt->id << "\n" << endl;

  
  txt->has_alpha = false;
  glBindTexture(GL_TEXTURE_2D, txt->id);
  
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
  
  
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  
  
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  
  
  gluBuild2DMipmaps( GL_TEXTURE_2D, 3,bmp->width, bmp->height, GL_RGB, GL_UNSIGNED_BYTE, bmp->data  );
}


void CTexture::loadtexture_transparent(CBitMap* bmp, COpenGLTexture* txt, double max) {
	
    txt->has_alpha = true;
    glGenTextures(1, &txt->id); 
    
    glBindTexture(GL_TEXTURE_2D, txt->id);
    
    //glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND );
    
    
    CVector keep = CVector(0,0,0);
    unsigned char p1,p2,p3;
	unsigned char* tmp = new unsigned char[4*bmp->width*bmp->height];
	for (unsigned int i=0;i<bmp->width;i++) {
	  for (unsigned int j=0;j<bmp->height;j++) {

	    p1 = bmp->data[3*i+ 3*j*bmp->width + 0];
	    p2 = bmp->data[3*i+ 3*j*bmp->width + 1];
	    p3 = bmp->data[3*i+ 3*j*bmp->width + 2];
	    
	    tmp[4*i + 4*j*bmp->width + 0] = p1;
	    tmp[4*i + 4*j*bmp->width + 1] = p2;
	    tmp[4*i + 4*j*bmp->width + 2] = p3;
	    if (p1+p2+p3 !=0) keep= CVector(p1,p2,p3);
	    tmp[4*i + 4*j*bmp->width + 3] = 255;
	    
	    unsigned int v = 1;
	    if (i>bmp->width-v) tmp[4*i + 4*j*bmp->width + 3] = 0;
	    if (i<v) tmp[4*i + 4*j*bmp->width + 3] = 0;
	    if (j>bmp->height-v) tmp[4*i + 4*j*bmp->width + 3] = 0;
	    if (j<v) tmp[4*i + 4*j*bmp->width + 3] = 0;
	    
	    
   	    if ((p1+p2+p3)<max) {
	      tmp[4*i + 4*j*bmp->width + 0] = (int)keep.x;
	      tmp[4*i + 4*j*bmp->width + 1] = (int)keep.y;
	      tmp[4*i + 4*j*bmp->width + 2] = (int)keep.z;
	      tmp[4*i + 4*j*bmp->width +3] = 0;
	      
	    }
	    //tmp[4*i + 4*j*bmp->width +3] = 0;
          
	  
	  }
	}

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	
	gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA ,bmp->width, bmp->height, GL_RGBA, GL_UNSIGNED_BYTE, tmp  );
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmp->width, bmp->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp);
	//    glGenerateMipmapEXT(GL_TEXTURE_2D);  
	
	/*    for (int i=0;i<15; i++)
	      manual_mipmap(tmp, bmp->width, bmp->height, i);
	*/  
  	delete[] tmp;
}

void CTexture::loadtexture_tga(CBitMap* bmp, COpenGLTexture* txt) {
	
	
  glGenTextures(1, &txt->id); 
  txt->has_alpha = true;	
  glBindTexture(GL_TEXTURE_2D, txt->id);
  
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

  /*  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
  */

	
  gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA ,bmp->width, bmp->height, GL_RGBA, GL_UNSIGNED_BYTE, bmp->data  );
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmp->width, bmp->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bmp->data);

 }




void CTexture::noise1(int* texture, int width) {
     int* to = new int[width*width];
     for (int i=0;i<width*width;i++)
         to[i] = texture[i];
 
     for (int i=0;i<width*width;i++) {
        int val = 0;
         for (int j=1;j<8; j++) {
             int p = (int)(pow(2.0,j-1.0)*i) % (width*width);
             val = val + (int)(double)(1.0/(double)j*(double)to[p]); 
         }    
 //        texture[i] = 50 + 50*sin( 0.01*(i % width) + val/60.0);    
     }
     delete[] to;
}
      
      
GLuint CTexture::gettexture(string name) {
  for (unsigned int i=0;i<names.size();i++) {
    if (names[i]==name) return t[i].id;
  } 
  return (GLuint)0;
}

COpenGLTexture* CTexture::gettexture_struct(string name) {
  for (unsigned int i=0;i<names.size();i++) {
    if (names[i]==name) return &t[i];
  } 
  throw string("Error: could not find texure '"+name + "'");
  return 0;

}
 
/*void CTexture::texture2bitmap(GLuint id, bitmap& bm) {


//  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, bm.data);
  
  for(int a = 0; a < lightmapChunkSize; a++)
    {
      for(int b = 0; b < lightmapChunkSize; b++)
	{
	  int a2 = a + lightmapChunkSize * terrainRow;
	  int b2 = b + lightmapChunkSize * terrainCol;
	  
	  lightmap[(a2 * lightmapSize + b2) * 3 + 0] = pixels[(a * lightmapChunkSize + b) * 3 + 0];
	  lightmap[(a2 * lightmapSize + b2) * 3 + 1] = pixels[(a * lightmapChunkSize + b) * 3 + 1];
	  lightmap[(a2 * lightmapSize + b2) * 3 + 2] = pixels[(a * lightmapChunkSize + b) * 3 + 2];
	}
	}
}
*/
