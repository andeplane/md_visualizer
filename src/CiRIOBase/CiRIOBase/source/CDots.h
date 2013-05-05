#pragma once

#include <CVector.h>
#include <CTexture.h>
#include <C2DField.h>
#include <CMisc.h>

class CDot {
 public:
  CVector P, C, V, A;
  double rad;
};

class CDots {
 public:
  int NoElements;
  bool Copy;
  double Timer, Width,Rinit, Rspread,MergeValue;
  CDot* Dots;
  CVector* RandomRotations;


  vector<CDots> Targets;
  
  CDots();
  ~CDots();
 
  void Initialize(int size, double rint, double rspread, double w);
  void InitializeFromBitmap(CBitMap& bmp, int size, double rint, double rspread, double scale, double widthscale);
  void RenderPoints(double size, RenderContainer& rc);
  void RenderSpheres(double fov, RenderContainer& rc);
  void RenderCubes(RenderContainer& rc);
  void RenderBillboards(RenderContainer&rc, GLuint texture);
  void RenderBillboardsRotate(RenderContainer&rc, GLuint texture);

  void Update(int, double);
  void Merge(int no1, int no2, double scale, int type);
  void Move(const CVector& other);
  void InitializeTargets(int no);
  void Delete();
};
