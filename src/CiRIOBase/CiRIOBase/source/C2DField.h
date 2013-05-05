#pragma once

#ifndef NO_OPENGL
#include <CShaders.h>
#include <CMisc.h>
#endif
#include <CBitMap.h>
#include <CUtil.h>
#include <CMath.h>
#include <CVector.h>

struct C2DField_data{
  float height;
  float water;
  float sedment;
  CVector normal, tangent;
};


class C2DField {
 public:
  int Width, Height;
  C2DField_data* Data;

  C2DField_data* Get(int, int);
  
  void Perlin();
  void Diamond();
  void Erode();
  void Sphere(double mul, bool typ, double dec);
  void Create(int, int);

  void ToBitMap(CBitMap& , float , float , float, float);
  void ToBitMapForce(CBitMap& , float , float , float, float);
  void RGBScale(double );
  void Normalize(double scale );
  void ExpVal(double a, double b, double c);
  void Erode(int);
  void Water(int watercnt, int cnt);
  //void Voronoi(int, bool, int, double, double, int );
  void Line(int x0, int y0, int x1, int y1, double color, bool erase, int size);
  void LineErase(int x0, int y0, int x1, int y1, int size);
  void Fill(int bx, int by, int x, int y, float base, float c, int type);
  void Merge(C2DField& , double weight, int type);
  void Erase(int x,int y);
  void Smooth(int n);
  void SmoothY(int n);
  void SmoothX(int n);
  void Bump(CVector sun, double s, double flip, double minval);
  void MakeSeamless(double scale);
  void Circle(int type, double val, double elipse);
  void CreateNormalMap(double s, CBitMap* dest);
  void FromBitMap(CBitMap& map);
  void CreateNormals(double scale); 

  static void CreateWindows(CBitMap& map, CVector size, CVector dist, CVector stat);

  void SaveBMP(string file); 
  //C2DField operator*(float scale);
 
  void Add(C2DField& o);
  void Subtract(C2DField& o);
  void Multiply(C2DField& o);
  void Scale(double v);
  void Dot(int x, int y, double h, int size);
  CVector CalculateNormal(int x, int y, double s, CVector* t);
  #ifndef NO_OPENGL
  void RenderTerrainSimple(RenderContainer* rc, double scale, double texturescale, GLuint texture, GLuint normaltexture);
  #endif NO_OPENGL

  C2DField();
  ~C2DField();


  #ifndef NO_OPENGL
  CBumpShader *BumpShader;
  #endif NO_OPENGL

 private:
  void DiamondStep(int x, int y, int add);
  double GetDiamondVal(double scale);
  void ErodeVertex(C2DField_data* v, C2DField_data* u, double t, double s);

  void FlowWater(C2DField_data* v, C2DField_data* u);
  


};
