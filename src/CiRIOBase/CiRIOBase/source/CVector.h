#pragma once

using namespace std;

#include <math.h>
#include <CMatrix.h>
#include <CMath.h>
#include <iostream>
#include <vector>
#include <CUtil.h>

class CVector  {
 public:
  double x,y,z;

  // Constructors

  inline CVector() {
    x=y=z=0;         
  }

  inline CVector(const double& px, const double& py, const double& pz) {
    x = px;
    y = py;
    z = pz;
  }

  inline CVector(const CVector& v)
  {
    x=v.x;
    y=v.y;
    z=v.z;
  }


  // Operators 

  inline CVector operator+(const CVector& a) const {
     return CVector(a.x +x, a.y+y, a.z+z);
   }

  inline CVector operator-(const CVector& a) const {
     return CVector(a.x - x, a.y - y, a.z - z);
   }

  inline void Subtract(const CVector& a)   {
    x-=a.x;
    y-=a.y;
    z-=a.z;
  }
  inline void Add(const CVector& a)   {
    x+=a.x;
    y+=a.y;
    z+=a.z;
  }


  inline double operator[](const int i) const {
    if (i==0) return x;
    if (i==1) return y;
    return z;
  }
 
  inline CVector operator*(double scale) const {
     return CVector(x * scale, y * scale, z * scale);
   }

  inline CVector operator/(const double scale) const {
     return CVector(x / scale, y / scale, z / scale);
   }
 


  inline CVector operator/(const CVector& o) const {
     return CVector(x / o.x, y / o.y, z / o.z);
   }

  friend ostream& operator<<(ostream& os, const CVector& o) {
    os <<"[ " << o.x << " " << o.y << " " << o.z << " ";
    os << "]" << endl;
    return os;
  }

  inline void operator=(const double& v) {
     x = v; y= v; z=v;
  }
  inline bool operator==(const CVector& v) const {
       if (x==v.x && y==v.y && z==v.z)
          return true;
       return false;     
  }
  inline void operator=(const int& v) {
     x = v; y= v; z=v;
  }


  // doubleransformations

  inline void Inverse() {
     x=-x;y=-y;z=-z;
   }

  inline CVector Mul(const CVector& v) const {
     return CVector(x * v.x, y * v.y, z * v.z);
   }
 
  inline void Mul(CMatrix& m) {
    double xt = m.M[0][0] * x + m.M[1][0] * y + m.M[2][0] * z; 
    double yt = m.M[0][1] * x + m.M[1][1] * y + m.M[2][1] * z; 
    double zt = m.M[0][2] * x + m.M[1][2] * y + m.M[2][2] * z; 
    x = xt; y = yt; z = zt;
  }

  inline CVector Rotate2d(double t) const {
      return CVector(cos(t)*x - sin(t)*y, sin(t)*x+cos(t)*y,0);
  }

  inline CVector rotateY(double t) const {
   return   CVector(x * cos(t) - z * sin(t) ,y, x*sin(t) + z*cos(t));
 }

  inline CVector rotateX(double t)  const {
   return CVector(x, y * cos(t) - z * sin(t), y*sin(t) + z*cos(t));
 }

  inline CVector rotateZ(double t) const {
   return CVector(x * cos(t) - y * sin(t), x*sin(t) + y*cos(t),z);
  }
  
  inline double Length() const {
    return sqrt((x*x + y*y + z*z));
  }
  
  inline double Length2() const {
    return ((x*x + y*y + z*z));
  }
  
  inline CVector Normalize() const {
    double length = Length();
    if (length!=0)
      return *this/length;
  }
  
  inline double Dot( const CVector& o) const {
    return (double)(x*o.x + y*o.y + z*o.z);
  }
  
  
  inline CVector Cross(const CVector& o) const {
    return CVector(o.y*z - o.z*y, o.z*x - o.x*z, o.x*y - o.y*x);
  }
  
  // Return statements
  
  inline void MinMax(CVector& min, CVector& max) {
    if (x>max.x) max.x=x;
    if (y>max.y) max.y=y;
    if (z>max.z) max.z=z;
    
    if (x<min.x) min.x=x;
    if (y<min.y) min.y=y;
    if (z<min.z) min.z=z;
  }

  inline CVector xz() const {
    return CVector(x,0,z);
  }
  
  inline CVector xy() const {
    return CVector(x,y,0);
  }
  
  inline CVector yz() const {
    return CVector(0,y,z);
  }
  
  // Utilities
  
  inline void Set(const double px, const double py, const double pz) {
     x = px; y=py; z = pz;
   }

  inline void Set(const CVector& v) {
     x = v.x; y=v.y; z = v.z;
   }

  inline void toDouble(double* a) {
    if (a==0)
      throw string("CVector::todouble error: array not allocated");
    a[0] = x;
    a[1] = y;
    a[2] = z;
    a[3] = 1.0f;
  }

  inline void toFloat(float* a) {
    if (a==0)
      throw string("CVector::todouble error: array not allocated");
    a[0] = x;
    a[1] = y;
    a[2] = z;
    a[3] = 1.0f;

  }
  static CVector nearest(vector<CVector>& lst, CVector add) {
     if (lst.size()==0) return CVector(0,0,0);
     double min = 100000;
     int winner = 0;
     for (unsigned int i=0;i<lst.size();i++) {
         double d = (lst[i]-add).Length();
         if (d<min) {
            winner = i;
            min = d;   
         }    
     }
     return lst[winner];
             
  }


  CVector RandomUniform() {
    return CVector(CMath::RandomUniform()*x - x/2.0,  CMath::RandomUniform()*y - y/2.0, CMath::RandomUniform()*z - z/2.0); 
  }
  CVector RandomGaussian() {
    return CVector((CMath::RandomGauss()-0.5)*x,  (CMath::RandomGauss()-0.5)*y , (CMath::RandomGauss()-0.5)*z); 
  }

  inline double distance_from_plane(CVector& plane_normal, CVector& V) const {
       return plane_normal.Dot(*this - V);       
  }

 inline CVector from_spherical() const {
   return CVector( cos(y)*sin(x), sin(y)*sin(x), cos(x)).Normalize() * z;             
 }

  void FloorColor();
  CVector glMatMul(float* pm);
  CVector glMatMul(float* pm, float& w);
  CVector glMatMul_flip(float* pm);
  static bool get_plane_equation(CVector p0, CVector p1, CVector p2, double* eq);
  static CVector Interpolate(CVector& v1, CVector& v2, CVector& v3, const double& val);
  double distance_from_plane(CVector& plane_normal, CVector& V);
};

// Define types

