#pragma once

//using namespace std;


#include <CIniFile.h>
#include <COpenGL.h>
#include <CShaders.h>
#include <CMisc.h>
#include <CTexture.h>
#include <CTextureMachine.h>
#include <string>

#ifdef _WIN32
#include <tchar.h>
#endif

class CApplication {
 public:
  static CIniFile ini;
  static CVector Mouse, MouseDiff;
  static int rightmousebutton, leftmousebutton;
  static CVector ClearColor;
  static CTime Time;
  static double TimeScale;
  static unsigned char key;
  static bool Pause, Initialized;
  
  static COpenGL ogl;
  static RenderContainer Rendercontainer;
  static CTexture Textures;
  static CTextureMachine TextureMachine;
 
  static void Clear (void);
  static void InternalUpdate(void);
 

  
  CApplication();

  static void Initialize(string inifile);
  static void Loop(void d(), void u(), string, int, char**);
  static void InternalKeyboard (unsigned char k, int  x, int y);
  static void InternalKeyboardUp (unsigned char k, int  x, int y);


  static void InitializeGUI();

  static void mouse(int x, int y);
  static void mousebutton(int button, int state, int x, int y);
  static void dragmouse(int x, int y);

  static void QuitError(string error);
  
};

