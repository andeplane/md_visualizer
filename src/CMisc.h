#pragma once

#include <CShaders.h>
#include <COpenGL.h>

class RenderContainer {
 public:
  CVector lightsource;
  bool use_light, shadow, is_reflection, enable_shader, depthmask;
  CVector camera, target;
  CVector ambientlight, specularlight, diffuselight;
  double global_ambience, alpha, blend;
  CVector color;
  double scale;

  CShaderParent* shader; 
  COpenGL* ogl; 
  
  
  RenderContainer() {
    shader = 0;
    ogl = 0;
  }
    


};


