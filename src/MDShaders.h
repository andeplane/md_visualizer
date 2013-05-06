#pragma once

#include <CShaders.h>

class MDSphereShader : public CShaderParent {
 public:
  MDSphereShader();  
  void set_light_pos(const float &x, const float &y, const float &z);
  void set_color(const float &r, const float &g, const float &b);
  void Initialize(string);
  void Start();
  void End();

};