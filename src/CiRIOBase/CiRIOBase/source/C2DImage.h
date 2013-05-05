#pragma once

#ifndef NO_OPENGL
#include <CShaders.h>
#endif
#include <CCounter.h>
#include <CBitMap.h>
#include <CUtil.h>
#include <CMath.h>
#include <CVector.h>
#include <complex>
#include <fftw3.h>
#include <CParticles.h>
#include <fitsio.h>
#include <perlin.h>
#include <C2DField.h>
#include <stdio.h>
//#include <CGalaxy.h>


#ifndef _WIN32
extern "C" {
#endif

#include <fits.h>

#ifndef _WIN32
}
#endif


struct C2DImage_data { 
  complex<double> height;
  float z;
};


class C2DImage {
 public:
  int Width, Height;
  C2DImage_data* Data;
  C2DImage_data zero;
  C2DImage_data* Get(int, int);

  float** toFitsData();
  Perlin* perlin;

  // FFT stuff
  bool PlanFFTInitialized, PlanFFTInvInitialized;
  fftw_plan PlanFFT, PlanFFTInv;
  fftw_complex* in;
  fftw_complex* out;


  void FFT(C2DImage& o);
  void FFTInv(C2DImage& o);

  void ResolveFFT(fftw_complex* out, C2DImage* o);
  void SetupFFT(fftw_complex **in, fftw_complex** out, double scale);

  void Convolve(float B); 
  void putStar(int x, int y, int n, float s, float max, float rotation, float rotation2);
  


  // yeah
  void putSmallCircle(int x, int y, int size, float s);

  //void RenderGalaxy(int x, int y, int size, float amplitude);
  //void RenderGalaxy2(int x, int y, int size, float amplitude);


  float galaxyTanH(float r_in, float r_out, float theta_out, float r);

  void Lens(C2DImage& lens, C2DImage& result, float strength, float radius);

  void RenderStars(int n, int spread, int base, float rotation, float rotation2, bool);

  void RenderParticles(CParticles& p, float w, float dx, float dy);


  void SaveFits(string filename, string header);
  void LoadFits(string filename);


  void Erode();
  void Sphere(double mul, bool typ, double dec);
  void Create(int, int);

  /*  void ToBitMap(CBitMap& , float , float , float, float);
      void ToBitMapForce(CBitMap& , float , float , float, float);*/
  void RGBScale(double );
  void Normalize(double scale );
  void ExpVal(double a, double b, double c);

  void Max(double val );
  void SubScalar(double val );


  void Line(int x0, int y0, int x1, int y1, double color, bool erase, int size);
  void LineErase(int x0, int y0, int x1, int y1, int size);
  void Fill(int bx, int by, int x, int y, float base, float c, int type);
  void Merge(C2DImage& , double weight, int type);
  void Smooth(int n);
  void Bump(CVector sun, double s, double flip, double minval);
  void MakeSeamless(double scale);
  void Circle(int type, double val, double elipse);
  void Circle3D();

  void Stamp(int x, int y, C2DImage& target, float amplitude); 


  void CreateNormalMap(double s, CBitMap* dest);

  void CreateNormals(double scale); 


  void SaveBMP(string file); 
  //C2DField operator*(float scale);
 
  void Add(C2DImage& o);
  void Subtract(C2DImage& o);
  void Multiply(C2DImage& o);
  void Divide(C2DImage& o);
  void Scale(double v);
  void Dot(int x, int y, double h, int size);
  CVector CalculateNormal(int x, int y, double s, CVector* t);
  void Gaussian(double B);
  void GaussianWithCross(double B);


  void AddNoise(float val);
  void AddPerlinNoise(float perlinscale, float scale);
  void ApplyFlatField(C2DImage& flat);

  C2DImage();
  ~C2DImage();


 private:
  


};
