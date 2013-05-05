#pragma once

#include <CVector.h>
#include <CMatrix.h>
#include <COpenGL.h>
#include <CMisc.h>
#include <CShaders.h>

class CTentacleNode {
 public:
  double Length, Time;
  CVector Position;
  CVector Rotation;
  CTentacleNode *Next, *Prev, *Child;
  double Lifetime;
  double Radius, CurrentRadius;
  double Attach;
  bool HasChildren;
  vector<CVector> Pos;
  CMatrix m1, m2, m3; // rotation matrices. Allocated once.
  double ypos;
  int Node, MaxNodes; // count where in node
  int Children; // count where in children 

  CTentacleNode();
  void AddNext();
  void AddChildren();
  void Update(double, double);
  void Render();
  void Calculate();
  void RotateTo(double pos);
  void RenderPixel(CVector p, CVector nx);
};


class CTentacle {
 public:
  CTentacleNode RootNode;
  double Radius;
  int MaxNodes, MaxChildren;
  CVector Position;
  static CBumpShader BumpShader;
  void Initialize(int maxnodes, int maxchildren);
  void Render(RenderContainer& rc, GLuint texture, GLuint normal);
  void Update(double, double);
  

};
