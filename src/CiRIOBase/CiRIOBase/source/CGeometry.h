#pragma once

#include <CVector.h>
#include <CUtil.h>

class CVertex {
 public: 
  CVector P, N, nrot, col, binormal, tangent;
  double tx, ty;
  double disp;
  CVertex(){
  }
};


class CPlane {
 public:

  CVertex V[3];
  CVector N, binormal, tangent;

  CPlane() {
  }

  /*  const CVertex& operator[]( int j) {
    return V[j];
  }
  */

  ~CPlane() {
  }


  // tests whether ray intersects with polygon
  bool intersectray(CVector origin, CVector ray, double len, CVector* isp_ret);
  double force_distance_to_plane(CVector& P, double max, CVector pos, CVector rot);  
  //double force_distance_to_index_double(CVector& P, double max, CVector pos, CVector rot);  

  bool rayintersectplane(CVector planeOrigin, CVector planeNormal, CVector rayOrigin, CVector rayCVector, CVector* isp);
  bool intersectray_triangle(CVector v1, CVector v2, CVector v3, const CVector& isp);

  bool PlaneSide(CVector point);
  double distance_to_plane(CVector point);

  
  static bool ClipFast(vector<CPlane>* p, const CVector& trans);



};

