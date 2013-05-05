#include "Stdafx.h"


#include <COpenGL.h>
#include <CMath.h>



void COpenGL::initialize(int w, int h, string title, void display(), int argc, char** argv) {

   try {
      width = w;
      height = h;
      aspect_ratio = (GLfloat)w/GLfloat(h);
      glutInit(&argc,argv);
      glutInitWindowSize(w,h); 
      glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA);
      glutCreateWindow (title.c_str()); 
      glutDisplayFunc (display); 
   
      glShadeModel(GL_SMOOTH);	
      glEnable(GL_DEPTH_TEST);
      glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );

      glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                       GL_LINEAR_MIPMAP_NEAREST );

      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

      clip_far = 10000;
      clip_near = 1;
      setperspective(80);

            
    }
    catch (...) { throw string("Error initializing Opengl"); }
    try {
       glewInit();
    }
    catch (...) { throw string("Error initializing Glew"); }

}


void COpenGL::setperspective(double per) {
     perspective = per;
     glMatrixMode (GL_PROJECTION);
     glLoadIdentity ();
     gluPerspective (perspective, aspect_ratio, clip_near, clip_far);
     glMatrixMode (GL_MODELVIEW);
}

void COpenGL::begin_clip2plane(CVector one, CVector two, CVector three) {
    glDisable(GL_CLIP_PLANE0);
    GLdouble eq2[4];
    CVector::get_plane_equation(one, two, three, eq2);
    // eq2[3] = 1000;
    glClipPlane(GL_CLIP_PLANE0, eq2);
    glEnable(GL_CLIP_PLANE0);
    if (!glIsEnabled(GL_CLIP_PLANE0)) throw string("Opengl error: Clip2plane not enabled!");
}

void COpenGL::end_clip2plane() {
    glDisable(GL_CLIP_PLANE0);
}

// switch to a texture class.. much better
void COpenGL::buffer2texture(GLuint texture, int w, int h, int type) {

  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  if (type==MIPMAP)
    type = MIPMAPALPHA;
       
  if (type==NOMIPMAP) {
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,0,0,w,h,0);
  }
  if (type==MIPMAPALPHA) {
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,0,0,w,h,0);
    unsigned char *t = new unsigned char[w*h*4]; 
  
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    // glTexSubImage2D(GL_TEXTURE_2D,0,0,0,w,h,GL_RGBA, GL_UNSIGNED_BYTE,t);
    glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA, GL_UNSIGNED_BYTE,t);
    //  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA ,w, h, GL_RGBA, GL_UNSIGNED_BYTE, t  );
    delete[] t;
  }
 
   glDisable(GL_TEXTURE_2D);

}

void COpenGL::depth2texture(GLuint texture, int size) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
       
  glEnable(GL_TEXTURE_2D);
     
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,0,0,size/2,size/2,0);

}

void COpenGL::SetOrthographicProjection() {
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, width, 0, height);
  glScalef(1, -1, 1);
  glTranslatef(0, -height, 0);
  glMatrixMode(GL_MODELVIEW);
}

void COpenGL::ResetPerspectiveProjection() {
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

inline void COpenGL::setviewport(int x,int y) {
       glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_STENCIL_BUFFER_BIT); 
       glViewport(0,0,x,y);
 }

void COpenGL::setup_camera() {
  //  glClearColor (0,0,0,0.0); 
  //glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_STENCIL_BUFFER_BIT); /* Clear the window */  
  glLoadIdentity ();   
  gluLookAt (  camera.x, camera.y, camera.z 
	       ,target.x, target.y, target.z,
	       ypr.x, ypr.y,ypr.z );
  
}

void COpenGL::setfog(bool val, CVector col) {
  GLfloat fogcolor[4]= {col.x, col.y, col.z, 1.0f};	
   
  glFogi(GL_FOG_MODE, GL_LINEAR);		// GL_EXP, G_EXP2, GL_LINEAR
  glFogfv(GL_FOG_COLOR,  fogcolor);		       
  glFogf(GL_FOG_DENSITY, fog_density);		
  //glHint(GL_FOG_HINT, GL_DONT_CARE);			 // Fog Hint Value
  glFogf(GL_FOG_START, fog_start);				 
  glFogf(GL_FOG_END, fog_end);				
  glHint(GL_FOG_HINT, GL_NICEST);	

 
    if (val) glEnable(GL_FOG); 
    else
      glDisable(GL_FOG); 
	
}
    
void COpenGL::begin_blend(int type) {
    if (isblend==true)
      throw string("COpenGL::begin_blend(): blending already set, should disable first");

     glEnable(GL_BLEND);
     if (type==COpenGL::blend_fill) 
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
     if (type==COpenGL::blend_add) 
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

     isblend = true;

  }

void COpenGL::begin_ignoredepth() {
  if (isdepth==true)
    throw string("COpenGL::begin_ignoredepth(): already ignoring depth, should disable first");
  
  glDisable(GL_DEPTH_TEST);
  isdepth = true;
}

void COpenGL::end_ignoredepth() {
  if (!isdepth)
    throw string("COpenGL::end_ignoredepth(): blending not set, cannot end");
  isdepth = false;
  glEnable(GL_DEPTH_TEST);
}

void COpenGL::end_blend() {
  if (!isblend)
    throw string("COpenGL::end_blend(): blending not set, cannot end");
  isblend = false;
  glDisable(GL_BLEND);
}
 
/*void COpenGL::pushvertex(COpenGL_vertex v) {
  vertexbuffer.push_back(v);
}
void COpenGL::clearvertexbuffer() {
  vertexbuffer.clear();
}
void COpenGL::render_vertexbuffer(int type) {
  glBegin(GL_POLYGON); 
  for (unsigned int i=0;i<vertexbuffer.size();i++) {
    lgfx_vertex* v = &vertexbuffer[i];
    if (v->hascolor)
      glColor4f(v->color.x,v->color.y,v->color.z,v->blend);
    if (v->hastexture)
      glTexCoord2f(v->txy.x,v->txy.y);
    if (type==render_3D)
      glVertex3f(v->pos.x,v->pos.y, v->pos.z);
    else if (type==render_2D)
      glVertex2f(v->pos.x,v->pos.y);
  }
  glEnd();
}

void COpenGL::render_vertexbuffer() {
  render_vertexbuffer(0);
}
*/
void COpenGL::push() {
  glPushMatrix();
}

void COpenGL::pop() {
  glPopMatrix();
}

CVector COpenGL::coord2ray(double px, double py) {
     double P = perspective / 360.0 * 2*3.14159; // convert to radians
      float pm[16];     // to get viewport matrix
      CVector res, dir;
      double x = px + width/2;
      double y = py + height/2;
      // modifiers 
      double mmx = tan(P*0.5)*(1.0-(x*2)/(double)width)*aspect_ratio;
      double mmy = tan(P*0.5)*-(1.0-(y*2)/(double)height);
      
      glGetFloatv(GL_MODELVIEW_MATRIX, pm);
      // find position in viewspace
      dir = CVector(mmx,mmy,1);
      res = (dir.glMatMul(pm)).Normalize();
      
      return res;       
}



void COpenGL::render_billboard(CVector angle, CVector position, CVector color, double blend, CVector size) {
  CVector p;
  double x = angle.x;
  double y = angle.y;
  y+=CMath::pi/2.0;
  x+=CMath::pi/2.0;
  //GLfloat* mat= CMath::matrix_zyz(y, x, y);

  double px = 0;
  double py = 0;
  double sz = 1.0;

  //double py = 0.5;
  // double px = v->lod*0.25;

  CVector up = CVector(0,1,0);
  //CVector d = (camera-position).Cross(up).Normalize();
  CVector d = CVector(-1,0,0);
            
  glPushMatrix();
  glTranslatef(position.x, position.y, position.z);

  glRotatef (x*360.0 / (2*CMath::pi), 0, -1, 0);
  glRotatef (y*360.0 / (2*CMath::pi), -1, 0, 0);
  glBegin(GL_QUADS);            
  
  p = d*-size.x + up*-size.y;
  glColor4f(color.x, color.y, color.z, blend);
  glTexCoord2f(px, py);
  glVertex3f(p.x, p.y, p.z );
  
  p = d*size.x + up*-size.y;
  glColor4f(color.x, color.y, color.z, blend);
  glTexCoord2f(px+sz, py);
  glVertex3f(p.x, p.y, p.z );
  
  p = d*size.x + up*size.y;
  glColor4f(color.x, color.y, color.z, blend);
  glTexCoord2f(px+sz, py+sz);
  glVertex3f(p.x, p.y, p.z );
  
  p = d*-size.x + up*size.y;
  glColor4f(color.x, color.y, color.z, blend);
  glTexCoord2f(px, py+sz);
  glVertex3f(p.x, p.y, p.z );
  
  glEnd();
  glPopMatrix();
  
  //delete[] mat;


}

void COpenGL::render_normal_billboard(CVector angle, CVector color, double blend, CVector size) {

  CVector pos = angle.from_spherical();
  CVector a2 = angle + CVector(0.001,0,0);
  CVector p2 = a2.from_spherical();
  p2 = (p2 + pos*-1).Normalize();
  CVector p3 = pos.Cross(p2).Normalize();

  double px = 0;
  double py = 0;
  double sz = 1.0;


  CVector up = p2;
  CVector d = p3;

  
  glPushMatrix();
  glTranslatef(pos.x, pos.y, pos.z);
  CVector p;
  glBegin(GL_QUADS);            

  p = d*-size.x + up*-size.y;
  glColor4f(color.x, color.y, color.z, blend);
  glTexCoord2f(px, py);
  glVertex3f(p.x, p.y, p.z );
  
  p = d*size.x + up*-size.y;
  glColor4f(color.x, color.y, color.z, blend);
  glTexCoord2f(px+sz, py);
  glVertex3f(p.x, p.y, p.z );
  
  p = d*size.x + up*size.y;
  glColor4f(color.x, color.y, color.z, blend);
  glTexCoord2f(px+sz, py+sz);
  glVertex3f(p.x, p.y, p.z );
  
  p = d*-size.x + up*size.y;
  glColor4f(color.x, color.y, color.z, blend);
  glTexCoord2f(px, py+sz);
  glVertex3f(p.x, p.y, p.z );
  
  glEnd();
  glPopMatrix();
  


}


void COpenGL::projective_texturing() {
  float PS[] = {1, 0, 0, 0};
  float PT[] = {0, 1, 0, 0};
  float PR[] = {0, 0, 1, 0};
  float PQ[] = {0, 0, 0, 1};
  
  glTexGenfv(GL_S, GL_EYE_PLANE, PS);
  glTexGenfv(GL_T, GL_EYE_PLANE, PT);
  glTexGenfv(GL_R, GL_EYE_PLANE, PR);
  glTexGenfv(GL_Q, GL_EYE_PLANE, PQ);
  
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);
  glEnable(GL_TEXTURE_GEN_R);
  glEnable(GL_TEXTURE_GEN_Q);
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

}

void COpenGL::BillboardCheatSphericalBegin() {
	
	float modelview[16];

	// save the current modelview matrix
	glPushMatrix();

	// get the current modelview matrix
	glGetFloatv(GL_MODELVIEW_MATRIX , modelview);

	// undo all rotations
	// beware all scaling is lost as well 
	/*for( i=0; i<3; i++ ) 
	    for( j=0; j<3; j++ ) {
		if ( i==j )
		    modelview[i*4+j] = 1.0;
		else
		    modelview[i*4+j] = 0.0;
		    }*/
	/*for( i=0; i<3; i+=2 ) 
	    for( j=0; j<3; j++ ) {
		if ( i==j )
		    modelview[i*4+j] = 1.0;
		else
		    modelview[i*4+j] = 0.0;
	    }
	*/

	// set the modelview with no rotations
	glLoadMatrixf(modelview);
}



void COpenGL::BillboardEnd() {
	glPopMatrix();
}


CVector COpenGL::Coord2Ray(double px, double py) {
   double P = perspective / 360.0 * 2*3.14159; // convert to radians
   float pm[16];     // to get viewport matrix
   CVector res, dir;
   double WindowWidth = width;
   double WindowHeight = height;

   double x = px + WindowWidth/2;
   double y = py + WindowHeight/2;
   // modifiers 
   double mmx = tan(P*0.5)*(1.0-(x*2)/(double)WindowWidth)* (WindowWidth/(double)WindowHeight);
   double mmy = tan(P*0.5)*-(1.0-(y*2)/(double)WindowHeight);
   
   glGetFloatv(GL_MODELVIEW_MATRIX, pm);
   // find position in viewspace
   dir = CVector(mmx,mmy,1);
   res = (dir.glMatMul(pm)).Normalize();
   
   return res;       
}
