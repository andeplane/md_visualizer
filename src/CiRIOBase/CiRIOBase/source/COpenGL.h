#pragma once

#include <OGLShader.h>
#include <CVector.h>


class COpenGL {
 public:  
  enum  {
    blend_add = 1,
    blend_fill = 2,
    render_3D = 0,
    render_2D = 1
  };

  static const int MIPMAP = 0;
  static const int NOMIPMAP = 1;
  static const int MIPMAPALPHA = 2;
  
  bool initialized, isblend, isdepth;          
  int width, height; 
  double aspect_ratio, perspective;
  double clip_near, clip_far, clip_sky;
  double fog_density, fog_start, fog_end; 
  
  CVector camera, target, ypr;
  
  // all shaders
  CShaderContainer shadercontainer;


   void initialize(int w, int h, string title, void display(), int, char**);
   CVector coord2ray(double px, double py);
   void setperspective(double per);
   void begin_clip2plane(CVector one, CVector two, CVector three);
   void end_clip2plane();
   void setviewport(int,int);    
   void setup_camera();
   void setfog(bool, CVector);
   void begin_blend(int type);
   void end_blend();
   void buffer2texture(GLuint texture, int w, int h, int mipmap);
   void depth2texture(GLuint texture, int size);

   void SetOrthographicProjection();
   void ResetPerspectiveProjection();
   CVector Coord2Ray(double px, double py);

    

   /*void pushvertex(lgfx_vertex v);
   void clearvertexbuffer();
   void render_vertexbuffer();
   void render_vertexbuffer(int type);*/
   void begin_ignoredepth();
   void end_ignoredepth();

   void pop();
   void push();

   void render_billboard(CVector camera, CVector pos, CVector color, double blend, CVector sizex);
   void render_normal_billboard(CVector angle, CVector color, double blend, CVector size);

   void projective_texturing();

   static void BillboardCheatSphericalBegin();
   static void BillboardEnd();


   COpenGL() {
     isblend = false;
     isdepth = false;
     ypr = CVector(0,1,0);

   }

   ~COpenGL() {

   }
  
};
class CTime {
   int prevtim, diff;
   int nr;
   int* a;
   public:
   double time;
   CTime() {
      nr = 8;
      a = new int[nr*10];        
      for (int i=0;i<nr;i++) {a[i]=1;}
   }
   ~CTime() {
      delete[] a;         
   }

   void reset() {
#ifndef _WIN32		
	   prevtim = glutGet(GLUT_ELAPSED_TIME);
#else
	   prevtim = glutGet(GLUT_ELAPSED_TIME);

#endif
   }

   void update() {
#ifndef _WIN32		
        diff = glutGet(GLUT_ELAPSED_TIME) - prevtim;
        prevtim = glutGet(GLUT_ELAPSED_TIME);
#else
        diff = glutGet(GLUT_ELAPSED_TIME) - prevtim;
        prevtim = glutGet(GLUT_ELAPSED_TIME);

#endif
        for (int i=0;i<(nr-1);i++)
          a[i] = a[i+1];

        a[nr-1] = diff;
        
   }   

   double avg() {
      double av = 0;
      for (int i=0;i<nr; i++) {
        av+=a[i];    
      }       
      return av/(double)nr;
   }


   double fps() {
        double t =  avg();
        if (t<0.001) t=0.001;
        return 1000.0/t; 
   }

   double scale() {
        double t = avg();
        if (t<0.001) t=0.001;
        if (t>200) t = 5;
        return t/ 5.0;
   }

}; 
