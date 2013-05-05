#include "Stdafx.h"


#ifndef _WIN32
#ifdef OS_X
#include <Carbon/Carbon.h>
#endif
#else
#endif



#include <CApplication.h>
#include <string>


CIniFile CApplication::ini;
COpenGL CApplication::ogl;
CVector CApplication::Mouse, CApplication::MouseDiff;
double CApplication::TimeScale; // mouse coordinates
int CApplication::rightmousebutton, CApplication::leftmousebutton;
unsigned char CApplication::key;
CVector CApplication::ClearColor; 
CTime CApplication::Time;
bool CApplication::Pause, CApplication::Initialized;
RenderContainer CApplication::Rendercontainer;
CTexture CApplication::Textures;
CTextureMachine CApplication::TextureMachine;

void CApplication::Clear (void) {
  glClearColor(ClearColor.x,ClearColor.y,ClearColor.z,1);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_STENCIL_BUFFER_BIT); 
}

void CApplication::InternalKeyboard (unsigned char k, int  x, int y) {
    key = k;

    if (key==27) 
        exit(1);

    if (key=='p') 
        Pause = !Pause;

    //    key = 0;

}

void CApplication::InternalKeyboardUp (unsigned char k, int  x, int y)  {
    
}


void CApplication::InternalUpdate(void) {

  //Update();
  TimeScale = Time.scale()*0.01;
  if (Pause)
    TimeScale = 0;
  //glutPostRedisplay();
  Time.update();

}

// mouse move
void CApplication::mouse(int x, int y) {
  Mouse.x = x;
  Mouse.y = y;
#ifdef WINDOWS
  //x = (x - ogl.width/2.0)/10.0;
  //y = (y - ogl.height/2.0)/10.0;
#endif
#ifdef OS_X
  CGGetLastMouseDelta(&x,&y);
#endif
  MouseDiff.x = x;
  MouseDiff.y = y;

}


// Mouse buttpn pressed
void CApplication::mousebutton(int button, int state, int x, int y) {
  int m = glutGetModifiers();
  
  if (button==GLUT_LEFT_BUTTON)          
    leftmousebutton=1;
  
  if (state == GLUT_UP) {
    leftmousebutton=0;
    //    fex.drag = false;
  }

  cout << button << endl;
  
  mouse(x,y);
  dragmouse(x,y);

}


CApplication::CApplication() {
  ClearColor = CVector(0,0,0);
  CShaderParent::COpenGLPointer = &ogl;
}

void CApplication::dragmouse(int x, int y) {
  mouse(x,y);
  
}



void CApplication::Loop(void Display(), void Update(), string name,int argc, char** argv) {
 
  ogl.initialize((int)ini.getdouble("screen_width"),(int)ini.getdouble("screen_height"),name.c_str(),Display, argc, argv);
  glutMouseFunc(mousebutton);
  glutPassiveMotionFunc(mouse);
  glutMotionFunc(dragmouse);
  glutIdleFunc(Update);
  glutKeyboardFunc(InternalKeyboard);
  glutKeyboardUpFunc(InternalKeyboardUp);

  glutMainLoop (); 
}

void CApplication::Initialize(string ifile) {
  Initialized = false;
  ini.load(ifile);
  InitializeGUI();
  Time.reset();
  TextureMachine.Initialize(&Textures, ogl.width, ogl.height);
}


void CApplication::InitializeGUI() {
  ogl.width = (int)ini.getdouble("screen_width");
  ogl.height = (int)ini.getdouble("screen_height");

}

void CApplication::QuitError(string error) {
#ifndef _WIN32 // Unix-style
	cout << error << endl;
#else // Windows specific messagebox
	TCHAR *param=new TCHAR[error.size()+1];	
	param[error.size()]=0;
	std::copy(error.begin(),error.end(),param);
	LPCTSTR myStr = param;	
	MessageBox(NULL, myStr,myStr, MB_OK | MB_ICONERROR);
#endif
	exit(1);
}

