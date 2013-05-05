#pragma once

#include <CTexture.h>
#include <CBitMap.h>
#include <C2DField.h>
#include <vector>
#include <string>
#include <stdio.h>
#include <COpenGL.h>

//using namespace std;

class CTextureMachine  {
 public:
  int MaxHF;

  C2DField* HeightFields;
  CBitMap BitMap;
  vector<string> Instructions;
  int Size,  Width,  Height;
  CTexture* CTexturePointer;
  
  CTextureMachine();
  ~CTextureMachine();

  void Initialize(CTexture* CT, int w, int h);

  void LoadFromFile(string, bool);
  void Execute();
  void Parse(string, bool);
  void RenderLeaves(int no, string leaf, string toTexture, double size, double size_spread, bool orientation);
 private:
  double GetNumber(int i, vector<string> tok); 
  int GetHF(int i, vector<string> tok);
  void Stamp(CVector pos, double rot, double size, CVector color, int id, bool randomorientation);

};
