#include <MDOpenGL.h>
#include <CMath.h>

// Frame counter and window settings variables
int redBits    = 8, greenBits = 8,    blueBits    = 8;
int alphaBits  = 8, depthBits = 24,   stencilBits = 0;

void MDOpenGL::initialize(int w, int h, GLFWkeyfun handle_keypress, GLFWmouseposfun handle_mouse_move) {
    running = true;
    window_title = "Molecular Dynamics Visualizer (MDV)";

    window_width = w;
    window_height = h;
    mid_window_x = window_width/2;
    mid_window_y = window_height/2;
    aspect_ratio = (GLfloat)w/GLfloat(h);

    field_of_view = 45.0f;            // Define our field of view (i.e. how quickly foreshortening occurs)
    near        = 2.0f;             // The near (Z Axis) point of our viewing frustum (default 2.0f)
    far         = 1500.0f;          // The far  (Z Axis) point of our viewing frustum (default 1500.0f)

    // Initialise GLFW
    if (!glfwInit() )
    {
        std::cout << "Failed to initialise GLFW!" << endl;
        glfwTerminate();
        exit(1);
    }

    // Create a window
    if( !glfwOpenWindow(window_width, window_height, redBits, greenBits, blueBits, alphaBits, depthBits, stencilBits, GLFW_WINDOW))
    {
        std::cout << "Failed to open window!" << std::endl;
        glfwTerminate();
        exit(1);
    }

    // Call our initGL function to set up our OpenGL options
    init_GL();

    // Instantiate our pointer to a Camera object providing it the size of the window
    cam = new Camera(window_width, window_height);

    // Set the mouse cursor to the centre of our window
    glfwSetMousePos(mid_window_x, mid_window_y);

    // // Specify the function which should execute when a key is pressed or released
    glfwSetKeyCallback(handle_keypress);

    // // Specify the function which should execute when the mouse is moved
    glfwSetMousePosCallback(handle_mouse_move);
}

void MDOpenGL::init_GL() {
    // ----- GLFW Settings -----
 
    glfwDisable(GLFW_MOUSE_CURSOR); // Hide the mouse cursor
 
    glfwSwapInterval(0);            // Disable vsync
 
    // ----- Window and Projection Settings -----
 
    // Set the window title
    glfwSetWindowTitle("Solar System FPS Controls Mk2| r3dux.org | Dec 2012");
 
    // Setup our viewport to be the entire size of the window
    glViewport(0, 0, (GLsizei)window_width, (GLsizei)window_height);
 
    // Change to the projection matrix, reset the matrix and set up our projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
 
    // The following code is a fancy bit of math that is eqivilant to calling:
    // gluPerspective(fieldOfView / 2.0f, width / height, near, far);
    // We do it this way simply to avoid requiring glu.h
    // GLfloat aspectRatio = (window_width > window_height)? float(window_width)/float(window_height) : float(window_height)/float(window_width);
    GLfloat fH = tan( float(field_of_view / 360.0f * 3.14159f) ) * near;
    GLfloat fW = fH * aspect_ratio;
    glFrustum(-fW, fW, -fH, fH, near, far);
 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
 
    // ----- OpenGL settings -----
 
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);              // Set out clear colour to black, full alpha
    glEnable(GL_DEPTH_TEST);                           // Enable the depth buffer
    glClearDepth(1.0f);                                // Clear the entire depth of the depth buffer
    glDepthFunc(GL_LEQUAL);                            // Set our depth function to overwrite if new value less than or equal to current value
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Ask for nicest perspective correction
    glLineWidth(2.0f);                                 // Set a 'chunky' line width
}

// void MDOpenGL::setperspective(double per) {
//      perspective = per;
//      glMatrixMode (GL_PROJECTION);
//      glLoadIdentity ();
//      gluPerspective (perspective, aspect_ratio, clip_near, clip_far);
//      glMatrixMode (GL_MODELVIEW);
// }

// void MDOpenGL::begin_clip2plane(CVector one, CVector two, CVector three) {
//     glDisable(GL_CLIP_PLANE0);
//     GLdouble eq2[4];
//     CVector::get_plane_equation(one, two, three, eq2);
//     // eq2[3] = 1000;
//     glClipPlane(GL_CLIP_PLANE0, eq2);
//     glEnable(GL_CLIP_PLANE0);
//     if (!glIsEnabled(GL_CLIP_PLANE0)) throw string("Opengl error: Clip2plane not enabled!");
// }

// void MDOpenGL::end_clip2plane() {
//     glDisable(GL_CLIP_PLANE0);
// }

// // switch to a texture class.. much better
// void MDOpenGL::buffer2texture(GLuint texture, int w, int h, int type) {

//   glEnable(GL_TEXTURE_2D);
//   glActiveTexture(GL_TEXTURE0);
//   glBindTexture(GL_TEXTURE_2D, texture);
//   if (type==MIPMAP)
//     type = MIPMAPALPHA;
       
//   if (type==NOMIPMAP) {
//     glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
//     glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
//     glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,0,0,w,h,0);
//   }
//   if (type==MIPMAPALPHA) {
//     glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
//     glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,0,0,w,h,0);
//     unsigned char *t = new unsigned char[w*h*4]; 
  
//     glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
//     glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
//     // glTexSubImage2D(GL_TEXTURE_2D,0,0,0,w,h,GL_RGBA, GL_UNSIGNED_BYTE,t);
//     glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA, GL_UNSIGNED_BYTE,t);
//     //  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
//     gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA ,w, h, GL_RGBA, GL_UNSIGNED_BYTE, t  );
//     delete[] t;
//   }
 
//    glDisable(GL_TEXTURE_2D);

// }

// void MDOpenGL::depth2texture(GLuint texture, int size) {
//     glActiveTexture(GL_TEXTURE0);
//     glBindTexture(GL_TEXTURE_2D, texture);
       
//     glEnable(GL_TEXTURE_2D);
     
//     glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
//     glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

//     glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,0,0,size/2,size/2,0);

// }

// void MDOpenGL::SetOrthographicProjection() {
//     glMatrixMode(GL_PROJECTION);
//     glPushMatrix();
//     glLoadIdentity();
//     gluOrtho2D(0, width, 0, height);
//     glScalef(1, -1, 1);
//     glTranslatef(0, -height, 0);
//     glMatrixMode(GL_MODELVIEW);
// }

// void MDOpenGL::ResetPerspectiveProjection() {
//     glMatrixMode(GL_PROJECTION);
//     glPopMatrix();
//     glMatrixMode(GL_MODELVIEW);
// }

// inline void MDOpenGL::setviewport(int x,int y) {
//    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_STENCIL_BUFFER_BIT); 
//    glViewport(0,0,x,y);
// }

// void MDOpenGL::setfog(bool val, CVector col) {
//   GLfloat fogcolor[4]= {col.x, col.y, col.z, 1.0f};	
   
//   glFogi(GL_FOG_MODE, GL_LINEAR);		// GL_EXP, G_EXP2, GL_LINEAR
//   glFogfv(GL_FOG_COLOR,  fogcolor);		       
//   glFogf(GL_FOG_DENSITY, fog_density);		
//   //glHint(GL_FOG_HINT, GL_DONT_CARE);			 // Fog Hint Value
//   glFogf(GL_FOG_START, fog_start);				 
//   glFogf(GL_FOG_END, fog_end);				
//   glHint(GL_FOG_HINT, GL_NICEST);	

 
//     if (val) glEnable(GL_FOG); 
//     else glDisable(GL_FOG); 
// }
    
// void MDOpenGL::begin_blend(int type) {
//     if (isblend==true)
//       throw string("COpenGL::begin_blend(): blending already set, should disable first");

//      glEnable(GL_BLEND);
//      if (type==COpenGL::blend_fill) 
//         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//      if (type==COpenGL::blend_add) 
//         glBlendFunc(GL_SRC_ALPHA, GL_ONE);

//      isblend = true;
// }

// void MDOpenGL::begin_ignoredepth() {
//   if (isdepth==true)
//     throw string("COpenGL::begin_ignoredepth(): already ignoring depth, should disable first");
  
//   glDisable(GL_DEPTH_TEST);
//   isdepth = true;
// }

// void MDOpenGL::end_ignoredepth() {
//   if (!isdepth)
//     throw string("COpenGL::end_ignoredepth(): blending not set, cannot end");
//   isdepth = false;
//   glEnable(GL_DEPTH_TEST);
// }

// void MDOpenGL::end_blend() {
//   if (!isblend)
//     throw string("COpenGL::end_blend(): blending not set, cannot end");
//   isblend = false;
//   glDisable(GL_BLEND);
// }
 
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
void MDOpenGL::push() {
  glPushMatrix();
}

void MDOpenGL::pop() {
  glPopMatrix();
}

// CVector MDOpenGL::coord2ray(double px, double py) {
//     double P = perspective / 360.0 * 2*3.14159; // convert to radians
//     float pm[16];     // to get viewport matrix
//     CVector res, dir;
//     double x = px + width/2;
//     double y = py + height/2;
//     // modifiers 
//     double mmx = tan(P*0.5)*(1.0-(x*2)/(double)width)*aspect_ratio;
//     double mmy = tan(P*0.5)*-(1.0-(y*2)/(double)height);

//     glGetFloatv(GL_MODELVIEW_MATRIX, pm);
//     // find position in viewspace
//     dir = CVector(mmx,mmy,1);
//     res = (dir.glMatMul(pm)).Normalize();

//     return res;       
// }



// void MDOpenGL::render_billboard(CVector angle, CVector position, CVector color, double blend, CVector size) {
//   CVector p;
//   double x = angle.x;
//   double y = angle.y;
//   y+=CMath::pi/2.0;
//   x+=CMath::pi/2.0;
//   //GLfloat* mat= CMath::matrix_zyz(y, x, y);

//   double px = 0;
//   double py = 0;
//   double sz = 1.0;

//   //double py = 0.5;
//   // double px = v->lod*0.25;

//   CVector up = CVector(0,1,0);
//   //CVector d = (camera-position).Cross(up).Normalize();
//   CVector d = CVector(-1,0,0);
            
//   glPushMatrix();
//   glTranslatef(position.x, position.y, position.z);

//   glRotatef (x*360.0 / (2*CMath::pi), 0, -1, 0);
//   glRotatef (y*360.0 / (2*CMath::pi), -1, 0, 0);
//   glBegin(GL_QUADS);            
  
//   p = d*-size.x + up*-size.y;
//   glColor4f(color.x, color.y, color.z, blend);
//   glTexCoord2f(px, py);
//   glVertex3f(p.x, p.y, p.z );
  
//   p = d*size.x + up*-size.y;
//   glColor4f(color.x, color.y, color.z, blend);
//   glTexCoord2f(px+sz, py);
//   glVertex3f(p.x, p.y, p.z );
  
//   p = d*size.x + up*size.y;
//   glColor4f(color.x, color.y, color.z, blend);
//   glTexCoord2f(px+sz, py+sz);
//   glVertex3f(p.x, p.y, p.z );
  
//   p = d*-size.x + up*size.y;
//   glColor4f(color.x, color.y, color.z, blend);
//   glTexCoord2f(px, py+sz);
//   glVertex3f(p.x, p.y, p.z );
  
//   glEnd();
//   glPopMatrix();
  
//   //delete[] mat;


// }

// void MDOpenGL::render_normal_billboard(CVector angle, CVector color, double blend, CVector size) {

//   CVector pos = angle.from_spherical();
//   CVector a2 = angle + CVector(0.001,0,0);
//   CVector p2 = a2.from_spherical();
//   p2 = (p2 + pos*-1).Normalize();
//   CVector p3 = pos.Cross(p2).Normalize();

//   double px = 0;
//   double py = 0;
//   double sz = 1.0;


//   CVector up = p2;
//   CVector d = p3;

  
//   glPushMatrix();
//   glTranslatef(pos.x, pos.y, pos.z);
//   CVector p;
//   glBegin(GL_QUADS);            

//   p = d*-size.x + up*-size.y;
//   glColor4f(color.x, color.y, color.z, blend);
//   glTexCoord2f(px, py);
//   glVertex3f(p.x, p.y, p.z );
  
//   p = d*size.x + up*-size.y;
//   glColor4f(color.x, color.y, color.z, blend);
//   glTexCoord2f(px+sz, py);
//   glVertex3f(p.x, p.y, p.z );
  
//   p = d*size.x + up*size.y;
//   glColor4f(color.x, color.y, color.z, blend);
//   glTexCoord2f(px+sz, py+sz);
//   glVertex3f(p.x, p.y, p.z );
  
//   p = d*-size.x + up*size.y;
//   glColor4f(color.x, color.y, color.z, blend);
//   glTexCoord2f(px, py+sz);
//   glVertex3f(p.x, p.y, p.z );
  
//   glEnd();
//   glPopMatrix();
  


// }


// void MDOpenGL::projective_texturing() {
//   float PS[] = {1, 0, 0, 0};
//   float PT[] = {0, 1, 0, 0};
//   float PR[] = {0, 0, 1, 0};
//   float PQ[] = {0, 0, 0, 1};
  
//   glTexGenfv(GL_S, GL_EYE_PLANE, PS);
//   glTexGenfv(GL_T, GL_EYE_PLANE, PT);
//   glTexGenfv(GL_R, GL_EYE_PLANE, PR);
//   glTexGenfv(GL_Q, GL_EYE_PLANE, PQ);
  
//   glEnable(GL_TEXTURE_GEN_S);
//   glEnable(GL_TEXTURE_GEN_T);
//   glEnable(GL_TEXTURE_GEN_R);
//   glEnable(GL_TEXTURE_GEN_Q);
//   glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
//   glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
//   glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
//   glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

// }

// void MDOpenGL::BillboardCheatSphericalBegin() {
	
// 	float modelview[16];

// 	// save the current modelview matrix
// 	glPushMatrix();

// 	// get the current modelview matrix
// 	glGetFloatv(GL_MODELVIEW_MATRIX , modelview);

// 	// undo all rotations
// 	// beware all scaling is lost as well 
// 	/*for( i=0; i<3; i++ ) 
// 	    for( j=0; j<3; j++ ) {
// 		if ( i==j )
// 		    modelview[i*4+j] = 1.0;
// 		else
// 		    modelview[i*4+j] = 0.0;
// 		    }*/
// 	/*for( i=0; i<3; i+=2 ) 
// 	    for( j=0; j<3; j++ ) {
// 		if ( i==j )
// 		    modelview[i*4+j] = 1.0;
// 		else
// 		    modelview[i*4+j] = 0.0;
// 	    }
// 	*/

// 	// set the modelview with no rotations
// 	glLoadMatrixf(modelview);
// }



// void MDOpenGL::BillboardEnd() {
// 	glPopMatrix();
// }


// CVector MDOpenGL::Coord2Ray(double px, double py) {
//    double P = perspective / 360.0 * 2*3.14159; // convert to radians
//    float pm[16];     // to get viewport matrix
//    CVector res, dir;
//    double WindowWidth = width;
//    double WindowHeight = height;

//    double x = px + WindowWidth/2;
//    double y = py + WindowHeight/2;
//    // modifiers 
//    double mmx = tan(P*0.5)*(1.0-(x*2)/(double)WindowWidth)* (WindowWidth/(double)WindowHeight);
//    double mmy = tan(P*0.5)*-(1.0-(y*2)/(double)WindowHeight);
   
//    glGetFloatv(GL_MODELVIEW_MATRIX, pm);
//    // find position in viewspace
//    dir = CVector(mmx,mmy,1);
//    res = (dir.glMatMul(pm)).Normalize();
   
//    return res;       
// }
