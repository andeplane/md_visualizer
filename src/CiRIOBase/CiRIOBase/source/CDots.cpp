#include "Stdafx.h"

#include <CDots.h>
#include <COpenGL.h>

CDots::CDots() {
  Dots = 0;
  MergeValue = 0.0;
  Copy = false;
  RandomRotations = 0;
}


void CDots::Delete() {
  if (Copy)
    return;
  if (Dots) {
    delete[] Dots;
    Dots = 0;
  }
  if (RandomRotations)
    delete[] RandomRotations;

}


CDots::~CDots() {
  Delete();
}

void CDots::Initialize(int size, double rinit, double rspread, double w) {
  NoElements = size;
  if (Dots)
    delete[] Dots;
  Dots = new CDot[size];
  Rinit = rinit;
  Width = w;
  Rspread = rspread;
  

  for (int i=0;i<size;i++) {
    Dots[i].rad = rinit;// + CMath::RandomUniform()*rspread;
    Dots[i].C = CVector(0.4, 0.7, 1.0);// + CVector(0.2, 0.2, 0.1).RandomUniform();
    //Dots[i].C = CVector(0.4, 0.7, 1.0) + CVector(0.1, 0.1, 0.00).RandomUniform();
    bool ok=false;
    /*   while (!ok) {
      Dots[i].P = CVector(w,w,w).RandomUniform() ;// CVector(w/2,w/2,w/2)*-1;
      ok = true;
      if (Dots[i].P.Length()>w) ok = false;
    }
    */
  }
}
void CDots::RenderPoints(double size,RenderContainer& rc){
  glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glPointSize(size);
  glBegin(GL_POINTS);
  double c = 1.0;
  double d= 0.2;
  for (int i=0;i<NoElements;i++) {
    glColor4f(Dots[i].C.x*c,Dots[i].C.y*c,Dots[i].C.z*c,d);
    glVertex3f(Dots[i].P.x,Dots[i].P.y,Dots[i].P.z); 
  }
 glEnd();
  glDisable(GL_BLEND);
 
}

void CDots::RenderSpheres(double fov,RenderContainer& rc){
  double f = 1.0;
  double scale = 2E8;
  glEnable(GL_CULL_FACE);
  for (int i=0;i<NoElements;i++) {
    glPushMatrix();
    double d = pow(rc.camera.x - Dots[i].P.x,2.0) +pow(rc.camera.y - Dots[i].P.y,2.0) + pow(rc.camera.z-Dots[i].P.z,2.0);
    double p = CMath::Minmax(Dots[i].rad*scale/d/f,4,16);
    
    glTranslatef(Dots[i].P.x,Dots[i].P.y, Dots[i].P.z); 
     glColor4f(Dots[i].C.x,Dots[i].C.y,Dots[i].C.z,1);
    glutSolidSphere(Dots[i].rad, p,p);
    glPopMatrix();
  }
  glDisable(GL_CULL_FACE);
 
}

void CDots::RenderCubes(RenderContainer& rc){
  glEnable(GL_CULL_FACE);
  int cnt =0;
  for (int i=0;i<NoElements;i++) {
    glPushMatrix();
    
    glTranslatef(Dots[i].P.x,Dots[i].P.y, Dots[i].P.z); 
    //glRotatef(0,1,1,1);
    if (++cnt==10000) {
      cnt =0;
      glRotatef((i+(int)(Timer*100.0)) % 360, Dots[i].P.x,Dots[i].P.y, Dots[i].P.z);
      }
    glColor4f(Dots[i].C.x,Dots[i].C.y,Dots[i].C.z,1);
    glutSolidCube(Dots[i].rad);
    glPopMatrix();
  }
  glDisable(GL_CULL_FACE);
 
}

void CDots::Update(int t, double timestep){

  for (int i=0;i<NoElements;i++) {
    Targets[t].Dots[i].P.x +=Targets[t].Dots[i].V.x*timestep;
    Targets[t].Dots[i].P.y +=Targets[t].Dots[i].V.y*timestep;
    Targets[t].Dots[i].P.z +=Targets[t].Dots[i].V.z*timestep;
  }

}

void CDots::Move(const CVector& other){
  for (int i=0;i<NoElements;i++) {
    Dots[i].P.x +=other.x;
    Dots[i].P.y +=other.y;
    Dots[i].P.z +=other.z;
  }
}



void CDots::InitializeFromBitmap(CBitMap& bmp, int size, double rint, double rspread, double scale, double widthscale) {
  // first initialize
  Initialize(size,rint, rspread, 1);
  
  for (int i=0;i<NoElements;i++) {
    bool ok=false;
    int x,y;
    while (!ok) {
      ok = true;
      x = rand()%bmp.width;
      y = rand()%bmp.height;
      if (bmp.pixel_elem(x,y,0)==0) ok = false;
    }
    Dots[i].P.x = (x - bmp.width/2.0)/(double)bmp.width*scale;
    Dots[i].P.y = (y - bmp.height/2.0)/(double)bmp.width*scale;
    Dots[i].P.z = (CMath::RandomUniform()-0.5)*widthscale;
  }

}

void CDots::Merge(int no1, int no2, double scale, int type) {
  if (no1>(int)Targets.size() || no2>(int)Targets.size())
    throw string("Error in CDots::Merge - target index out of bonds");
  double s = CMath::Minmax(1-scale,0,1);
  if (type==1)
    s = 0.5 + 0.5*sin(CMath::pi*s - CMath::pi/2.0);//0.5 + atan(50*(s-0.5))/(CMath::pi);
  s = CMath::Minmax(s,0,1);

  double s2 = 1.0 -s;
  for (int i=0;i<NoElements;i++) {
    Dots[i].P.x = Targets[no1].Dots[i].P.x*s + Targets[no2].Dots[i].P.x*s2;
    Dots[i].P.y = Targets[no1].Dots[i].P.y*s + Targets[no2].Dots[i].P.y*s2;
    Dots[i].P.z = Targets[no1].Dots[i].P.z*s + Targets[no2].Dots[i].P.z*s2;
    Dots[i].rad = Targets[no1].Dots[i].rad*s + Targets[no2].Dots[i].rad*s2;
  }
}

void CDots::InitializeTargets(int no) {
  Targets.resize(no+1);
  for (int i=0;i<no;i++) 
    Targets[i].Initialize(NoElements, Rinit, Rspread, Width);

  for (int i=0;i<NoElements;i++) 
    Targets[0].Dots[i] = Dots[i];

  Targets[no].Dots = Targets[0].Dots;
  Targets[no] = Targets[0];
  Targets[no].Copy = true;
}

void CDots::RenderBillboards(RenderContainer& rc,GLuint texture) {

  glDisable(GL_CULL_FACE);

  if (rc.blend!=0) 
    glEnable(GL_BLEND);
  if (rc.blend==1)
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  if (rc.blend==2) 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA_SATURATE);


  glEnable( GL_TEXTURE_2D );
  glBindTexture(GL_TEXTURE_2D, texture);
  glDepthMask(rc.depthmask);
  CVector vd = (rc.camera - rc.target).Normalize();
  //if (vd.z<0) vd.z=-vd.z;
  //vd.x=max(0.0f,(float)vd.x);
  //vd.y=max(0.0f,(float)vd.y);
  //vd.z=max(0.0f,(float)vd.z);
  //vd.y = 0;

  CVector left = CVector(1,0,0);

  int S = 1.0;

  if (rc.ogl!=0) {
    left = vd.Cross(rc.ogl->Coord2Ray(0,rc.ogl->height/2.0));
    if (RandomRotations == 0) {
      RandomRotations = new CVector[S];
    }
      /*      for (int i=0;i<16;i++) {
	RandomRotations[i] = vd.Cross(rc.ogl->Coord2Ray(rand()%rc.ogl->width - rc.ogl->width/2.0,rand()%rc.ogl->height - rc.ogl->height/2.0));
	}*/
      RandomRotations[0] = vd.Cross(rc.ogl->Coord2Ray(0,rc.ogl->height/2.0));  
      //RandomRotations[1] = vd.Cross(rc.ogl->Coord2Ray(0,-rc.ogl->height/2.0));  
      //RandomRotations[2] = vd.Cross(rc.ogl->Coord2Ray(rc.ogl->width/2.0,0));  
      // RandomRotations[3] = vd.Cross(rc.ogl->Coord2Ray(-rc.ogl->width/2.0,0));  
    }
  
  CVector up = (vd.Cross(left)).Normalize();
  
  CVector r = (vd.Cross(up)).Normalize();

  CVector v0 = (r + up);
  CVector v1 = (r*-1 + up);
  CVector v2 = (r*-1 + up*-1);
  CVector v3 = (r + up*-1);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER,0.01);
  //cout << vd.x <<  "  " << vd.y <<  "  " <<vd.z << endl;
  int N = 50000;
  int cnt = N;
  glBegin(GL_QUADS);
   glNormal3f(vd.x, vd.y, vd.z);
   for (int i=0;i<NoElements;i++) {
    double s = Dots[i].rad*rc.scale;
    glColor4f(Dots[i].C.x,Dots[i].C.y,Dots[i].C.z,rc.alpha);
    
    glTexCoord2f(0,0);
    glVertex3f(Dots[i].P.x+v0.x*s,Dots[i].P.y+v0.y*s,Dots[i].P.z+v0.z*s); 
    glTexCoord2f(1,0);
    glVertex3f(Dots[i].P.x+v1.x*s,Dots[i].P.y+v1.y*s,Dots[i].P.z+v1.z*s); 
    glTexCoord2f(1,1);
    glVertex3f(Dots[i].P.x+v2.x*s,Dots[i].P.y+v2.y*s,Dots[i].P.z+v2.z*s); 
    glTexCoord2f(0,1);
    glVertex3f(Dots[i].P.x+v3.x*s,Dots[i].P.y+v3.y*s,Dots[i].P.z+v3.z*s); 
    
    /*if (cnt--==0 && RandomRotations) {
      cnt = N;
      
      up = (vd.Cross(RandomRotations[i%S])).Normalize();
      
      r = (vd.Cross(up)).Normalize();
      v0 = (r + up);
      v1 = (r*-1 + up);
      v2 = (r*-1 + up*-1);
      v3 = (r + up*-1);
      
      }*/
  }
  glEnd();
 glDisable(GL_BLEND);
 glEnable(GL_CULL_FACE);
 
 glDepthMask(true);
 glDisable( GL_TEXTURE_2D );
}

void CDots::RenderBillboardsRotate(RenderContainer& rc,GLuint texture) {

  glDisable(GL_CULL_FACE);

  if (rc.blend!=0) 
    glEnable(GL_BLEND);
  if (rc.blend==1)
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  if (rc.blend==2) 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA_SATURATE);


  glEnable( GL_TEXTURE_2D );
  glBindTexture(GL_TEXTURE_2D, texture);
  glDepthMask(rc.depthmask);
  CVector vd = (rc.camera - rc.target).Normalize();
  //if (vd.z<0) vd.z=-vd.z;
  //vd.x=max(0.0f,(float)vd.x);
  //vd.y=max(0.0f,(float)vd.y);
  //vd.z=max(0.0f,(float)vd.z);

  CVector left = CVector(1,0,0);
  CVector nup = CVector(0,-1,0);
  if (vd.z<0) left = CVector(-1,0,0);
  CVector up = (vd.Cross(left)).Normalize();
  CVector r = (vd.Cross(up)).Normalize();

  CVector v0 = (r + up);
  CVector v1 = (r*-1 + up);
  CVector v2 = (r*-1 + up*-1);
  CVector v3 = (r + up*-1);

  left = CVector(0,-1,0).Normalize();
  nup = CVector(1,0,0).Normalize();
  if (vd.z<0) left = CVector(0,1,0);
  up = (vd.Cross(left)).Normalize();
  r = (vd.Cross(up)).Normalize();

  CVector w0 = (r + up);
  CVector w1 = (r*-1 + up);
  CVector w2 = (r*-1 + up*-1);
  CVector w3 = (r + up*-1);


  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER,0.01);

  glBegin(GL_QUADS);
  for (int i=0;i<NoElements;i++) {
    double s = Dots[i].rad;
    glColor4f(Dots[i].C.x,Dots[i].C.y,Dots[i].C.z,rc.alpha);
    if (i%2==i%2) {
      glTexCoord2f(0,0);
      glVertex3f(Dots[i].P.x+v0.x*s,Dots[i].P.y+v0.y*s,Dots[i].P.z+v0.z*s); 
      glTexCoord2f(1,0);
      glVertex3f(Dots[i].P.x+v1.x*s,Dots[i].P.y+v1.y*s,Dots[i].P.z+v1.z*s); 
      glTexCoord2f(1,1);
      glVertex3f(Dots[i].P.x+v2.x*s,Dots[i].P.y+v2.y*s,Dots[i].P.z+v2.z*s); 
      glTexCoord2f(0,1);
      glVertex3f(Dots[i].P.x+v3.x*s,Dots[i].P.y+v3.y*s,Dots[i].P.z+v3.z*s); 
    }
    else
      {
      glTexCoord2f(0,0);
      glVertex3f(Dots[i].P.x+w0.x*s,Dots[i].P.y+w0.y*s,Dots[i].P.z+w0.z*s); 
      glTexCoord2f(1,0);
      glVertex3f(Dots[i].P.x+w1.x*s,Dots[i].P.y+w1.y*s,Dots[i].P.z+w1.z*s); 
      glTexCoord2f(1,1);
      glVertex3f(Dots[i].P.x+w2.x*s,Dots[i].P.y+w2.y*s,Dots[i].P.z+w2.z*s); 
      glTexCoord2f(0,1);
      glVertex3f(Dots[i].P.x+w3.x*s,Dots[i].P.y+w3.y*s,Dots[i].P.z+w3.z*s); 

    }
  }
  glEnd();
 glDisable(GL_BLEND);
 glEnable(GL_CULL_FACE);
 
 glDepthMask(true);
 glDisable( GL_TEXTURE_2D );

 COpenGL::BillboardEnd();
}
