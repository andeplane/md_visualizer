#pragma once
#include <COpenGL.h>
#include <CBitMap.h>
#include <CUtil.h>
#include <CVector.h>


// internal texture structure
class COpenGLTexture {
 public:
  string        category;
  GLuint	id;		
  GLsizei	imgHeight, imgWidth;	
  GLsizei	txDimensions;		
  GLfloat	alpha;			
  bool          has_alpha;
};


/*
*
*  class texture: maintains, loads and generates textures for polly
*
*/

class CTexture {
   public:
          
      CTexture() {
      }
          
      // adds textures
      COpenGLTexture* addtexture(string fname, string name);
      COpenGLTexture* addtexture(string fname, string name, bool transparent);
      void addtga(string fname, string name);

      // generates a texture sphere
      void create_sphere(string name, int w, CVector col1, CVector col2, double mul, bool typ, double dec);
      // generates terrain height texture
      // void generate_cloud(double val);
      // returns textures
  
      COpenGLTexture* gettexture_struct(string name);
      GLuint gettexture(string name);
      GLuint generate_texture(string name);
      vector<COpenGLTexture> t;	
      vector<string> names;

      //private:
      
      void loadtexture(CBitMap* bmp, COpenGLTexture* txt);
      void loadtexture_transparent(CBitMap* bmp, COpenGLTexture* txt, double max);
      void loadtexture_tga(CBitMap* bmp, COpenGLTexture* txt);

      void noise1(int* texture, int width);
      //void texture2bitmap(GLuint id, bitmap& bm);
};

