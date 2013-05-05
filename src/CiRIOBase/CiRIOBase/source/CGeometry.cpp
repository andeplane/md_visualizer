
#include "Stdafx.h"

#include <CGeometry.h>
#include <OGLShader.h>

double CPlane::distance_to_plane(CVector point) {
   return N.Dot(point - V[0].P);
} 
  
bool CPlane::rayintersectplane(CVector planeOrigin, CVector planeNormal, CVector rayOrigin, CVector rayCVector, CVector* isp) {
   
  CVector SuperPos = rayCVector.Normalize();
  
  SuperPos=SuperPos*10000.0;
  SuperPos=SuperPos+rayOrigin;
  
  CVector Pos1 = rayOrigin - planeOrigin;
  CVector Pos2 = SuperPos - planeOrigin;
  CVector p3 = SuperPos-rayOrigin;
  
  double sc1 = Pos1.Dot(planeNormal);
  double sc2 = Pos2.Dot(planeNormal);
  if ((sc1-sc2)==0) return false;
  
  double Scale = (sc1 / (sc1-sc2));
  *isp = rayOrigin + (p3)*-Scale;
  return true;
	
  }     
        
   
// used for collision detection: force a distance to a plane
double CPlane::force_distance_to_plane( CVector& P, double max, CVector pos, CVector rot) {
        
  float pm[16];
  CVector p1, p2, p3, n1;
  glPushMatrix();
  // First we need to rotate the vertices normals. These fuckers won't rotate themselves
  glLoadIdentity();
  glRotatef((GLfloat)-rot.x,1,0,0);
  glRotatef((GLfloat)-rot.y,0,1,0);
  glRotatef((GLfloat)-rot.z,0,0,1);
  glGetFloatv(GL_MODELVIEW_MATRIX, pm);
  n1 = N.glMatMul(pm);
  glPopMatrix();
  glPushMatrix();
  glLoadIdentity();
  glRotatef((GLfloat)-rot.x,1,0,0);
  glRotatef((GLfloat)-rot.y,0,1,0);
  glRotatef((GLfloat)-rot.z,0,0,1);
  glGetFloatv(GL_MODELVIEW_MATRIX, pm);
  
  p1 = V[0].P.glMatMul(pm);
  p2 = V[1].P.glMatMul(pm);
  p3 = V[2].P.glMatMul(pm);
  p1 = p1 + pos;
  p2 = p2 + pos;
  p3 = p3 + pos;
  glPopMatrix();
  
  double dist = fabs(P.distance_from_plane(n1, p2));
  if (dist>max) 
    return -1;
  CVector isp = P + n1*dist;
  if (intersectray_triangle(p1, p2, p3, isp)) {
    
    /*          glBegin(GL_LINE_LOOP);
		glColor4f(1,1,1,1);
		glVertex3f(p1.x, p1.y, p1.z);
		glVertex3f(p2.x, p2.y, p2.z);
		glVertex3f(p3.x, p3.y, p3.z);
		glEnd();
    */
    P = P + n1*(dist - max*1);
    return dist;
  }
  return -1;
}
/*
double CPlane::force_distance_to_index_double(vector<vertex>& vertices, CVector& P, double max, CVector pos, CVector rot) {
        
        float pm[16];
        CVector p1, p2, p3, n1;
       glPushMatrix();
          // First we need to rotate the vertices normals. These fuckers won't rotate themselves
       glLoadIdentity();
       glRotatef(-rot.x,1,0,0);
       glRotatef(-rot.y,0,1,0);
       glRotatef(-rot.z,0,0,1);
       glGetFloatv(GL_MODELVIEW_MATRIX, pm);
       n1 = n.glMatMul(pm);
       glPopMatrix();
       glPushMatrix();
       glLoadIdentity();
       glRotatef(-rot.x,1,0,0);
       glRotatef(-rot.y,0,1,0);
       glRotatef(-rot.z,0,0,1);
       glGetFloatv(GL_MODELVIEW_MATRIX, pm);
       
       p1 = vertices[i[0]].v.glMatMul(pm);
       p2 = vertices[i[1]].v.glMatMul(pm);
       p3 = vertices[i[2]].v.glMatMul(pm);
       p1 = p1 + pos;
       p2 = p2 + pos;
       p3 = p3 + pos;
       glPopMatrix();

       double dist = fabs(P.distance_from_plane(n1, p2));
       if (dist>max) 
          return -1;
       CVector isp = P + n1*dist;
       if (intersectray_triangle(vertices, p1, p2, p3, isp)) {
          P = P + n1*(dist - max*1);
          return dist;
      }
      n1 = n1*-1;
       dist = fabs(P.distance_from_plane(n1, p2));
       if (dist>max) 
          return -1;
       isp = P + n1*dist;
       if (intersectray_triangle(vertices, p1, p2, p3, isp)) {
          P = P + n1*(dist - max*1);
          return dist;
      }


      return -1;
}
*/
  
bool CPlane::intersectray(CVector origin, CVector ray, double len, CVector* isp_ret) {
        
        CVector isp;
        bool intersect = false;

        CVector n1 = (V[1].P-V[0].P).Cross(V[2].P-V[0].P).Normalize();
        
        rayintersectplane(V[0].P,n1, origin, ray, &isp);
        if ((isp-origin).Length()>len) 
           return false; 
        *isp_ret=CVector(0,0,0);
        if (intersectray_triangle(V[0].P, V[1].P, V[2].P, isp)) {
           intersect = true;
           *isp_ret = isp;
        }

        return intersect;
   }
  
  
bool CPlane::intersectray_triangle(CVector v1, CVector v2, CVector v3, const CVector& isp) {
        
  //if ((isp-origin).Length()<len)
  {
    float dAngle = 0;
    v1 = v1 - isp;
    v1 = v1/-v1.Length();
    v2 = v2 - isp;
    v2 = v2/-v2.Length();
    v3 = v3 - isp;
    v3 = v3/-v3.Length();
    
    dAngle = (float)acos( (float)(v1.Dot(v2)) + acos(v2.Dot(v3))+ acos(v3.Dot(v1)));
    dAngle = (float)fabs( dAngle - 2.0*3.14159265 );
    
    if (dAngle<0.01) return true;
  }
  
  return false;
}
  
  
bool CPlane::ClipFast(vector<CPlane>* p, const CVector& trans) {

   bool ok=false;
   for (unsigned int j=0;j<p->size();j++) {
         bool allwithin = true;

	 if (!p->at(j).PlaneSide((CVector)(trans)))
	   allwithin=false;  

         if (allwithin) ok = true;
   }
   
     
   return !ok;
}

bool CPlane::PlaneSide(CVector point) {
  double dist = distance_to_plane(point);
  if (dist>=0) return true;
  return false;     
}
