#pragma once

#include <CShaders.h>

class MDSphereShader : public CShaderParent {
 public:
  MDSphereShader();  

  CVector lightpos; 
  void SetColor(const float &r, const float &g, const float &b);
  void Initialize(string);
  void Start();
  void End();

};