#include "Stdafx.h"


#include <CTentacle.h>

CBumpShader CTentacle::BumpShader;


CTentacleNode::CTentacleNode() {
  Prev = Next = Child = 0;
  Next = 0;
  Lifetime = 0.0;
  Radius = 1.0 + (rand()%1000/1000.0)*0.3;
  CurrentRadius = 0;
  Length = 2.0 + (rand()%1000/1000.0)*2.0;
  Rotation=CVector(1.0, 1.0, 1.0).RandomUniform();
  HasChildren = false;
  //if (rand()%5 > )
  //HasChildren = true;
  Pos.resize(6);
}

void CTentacleNode::Update(double t, double b) {
  
  Rotation.x +=sin(Time*Node*0.2)*b;
  Rotation.y -=sin(Time*Node*0.2)*b;
  Rotation.z +=cos(Time*Node*0.2)*b;
  

  Lifetime+=t;
  Time+=t;
  if (Lifetime>1.0) 
    Lifetime = 1.0;

  if (Lifetime>0.7 && Next ==0 && Node!=0)
    AddNext();

     if (Lifetime>0.6 && Child == 0 && HasChildren && Children!=0)
    AddChildren();
  
  if (Prev)
    ypos = Lifetime + Prev->ypos;
  else
    ypos = Lifetime;
 
  if (Next) 
    Next->Update(t,b);

  if (Child) 
    Child->Update(t,b);

  
}

void CTentacleNode::AddNext() {
  Next = new CTentacleNode();
  Next->Prev = this;
  Next->Attach = 1;
  Next->Node = Node-1;
  Next->MaxNodes = MaxNodes;
  if (Node % 1==0) Next->HasChildren = true;

  double l = 1.3;
  Next->Rotation=Rotation + CVector(l, l, l).RandomUniform();

}
void CTentacleNode::AddChildren() {
  Child = new CTentacleNode();
  Child->Prev = this;
  Child->Attach = (rand()%1000/1000.0) * Lifetime;
  Child->Children = Children-1;
  Child->Node = MaxNodes;
  Child->MaxNodes = MaxNodes;
  double l = 3;
  Child->Rotation=Rotation + CVector(l, l, l).RandomUniform();
}

void CTentacleNode::RotateTo(double pos) {
  CVector rot = Rotation;        //CVector::Interpolate(disp1, disp2, endrot, pos);
  m1.RotateXY(rot.x);
  m3.RotateXZ(rot.z);
  m2.RotateYZ(rot.y);
  m1.Mul(m2);
  m1.Mul(m3);
}



void CTentacleNode::Calculate() {
  double d = min(Lifetime, 1.0);

  CVector v = CVector(0,d*Length,0);
  if (Next)
    CurrentRadius +=1.0/(200.0*CurrentRadius+100.0);

  CurrentRadius=min(CurrentRadius, Radius);

  RotateTo(1);
  v.Mul(m1);

  if (Prev) 
    Position = Prev->Position + v;

  double t2 = 0.0;//Time*0.1*Lifetime;
  for (unsigned int i=0;i<Pos.size();i++) {
    double t1 = ((2.0*CMath::pi/(double)Pos.size()))*i;
    double b = CurrentRadius;
    if (Next==0)
      b = 0.05;
    Pos[i] = CVector(b*cos(t1+t2), 0, b*sin(t1+t2));
    Pos[i].Mul(m1);
    Pos[i] = Pos[i] + Position;
  }

  if (Next)
    Next->Calculate();
  if (Child)
    Child->Calculate();

}

void CTentacleNode::RenderPixel(CVector p, CVector n) {
  glNormal3f(n.x, n.y, n.z);
  glVertex3f(p.x, p.y, p.z);
}

void CTentacleNode::Render() {
  
  //glPushMatrix();
  //glRotate3f(Rotation.x*Lifetime, Rotation.y*Lifetime, Rotation.z*Lifetime);
  //glTranslate3f(0,Lifetime*Length);

  /*  if (Next==0) {
    //cout << Lifetime << endl;
    }*/
  if (Prev) 
    {
      glBegin(GL_QUADS);
      for (unsigned int i=0;i<Pos.size();i++) {
	int j = (i+1) % Pos.size();

	
	CVector n1 = (Position - Pos[i]);
	CVector n2 = (Prev->Position - Prev->Pos[i]);
	CVector n3 = (Prev->Position - Prev->Pos[j]);
	CVector n4 = (Position - Pos[j]);
	double x=1.0;
	double y=10.0;

	double tx1 = (1.0/(double)Pos.size()*i)*x - Time*0.01;
	double tx2 = (1.0/(double)Pos.size()*(i+1)) *x - Time*0.01;
	/*	double ty1 = Pos[i].y/(double)50.0*y;
		double ty2 = Prev->Pos[i].y/(double)50.0*y;*/
	double ty1 = Node*0.3;
	double ty2 = (Node+1)*0.3;



	CTentacle::BumpShader.Tangent(n1.Cross(Pos[i]));
	glTexCoord2f(tx1, ty1);
	RenderPixel(Pos[i],n1);

	CTentacle::BumpShader.Tangent(n2.Cross(Prev->Pos[i]));
	glTexCoord2f(tx1, ty2);
	RenderPixel(Prev->Pos[i],n2);

	CTentacle::BumpShader.Tangent(n3.Cross(Prev->Pos[j]));
	glTexCoord2f(tx2, ty2);
	RenderPixel(Prev->Pos[j],n3);

	CTentacle::BumpShader.Tangent(n4.Cross(Pos[j]));
	glTexCoord2f(tx2, ty1);
	RenderPixel(Pos[j],n4);
      }
      glEnd();
    }

  if (Next)
    Next->Render();
  if (Child)
    Child->Render();

  //glPopMatrix();

}


void CTentacle::Initialize(int m, int c) {
  RootNode.AddNext();
  RootNode.Rotation = CVector(0,0,0);
  BumpShader.Initialize("bumpshader");
  RootNode.Children = c;
  RootNode.Next->Children = c;
  RootNode.Next->Node = m;
  RootNode.Next->MaxNodes = m;
}
void CTentacle::Render(RenderContainer& rc, GLuint texture, GLuint normal) {

  //glEnable(GL_LIGHT1);
  glEnable(GL_TEXTURE);
  
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  CVector l = (rc.lightsource).Normalize()*1000.0;
  float tmp[4];
  rc.ambientlight.toFloat(tmp);
  glLightfv(GL_LIGHT0, GL_AMBIENT, tmp);
  rc.diffuselight.toFloat(tmp);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, tmp);
  rc.specularlight.toFloat(tmp);
  glLightfv(GL_LIGHT0, GL_SPECULAR, tmp);
  float t = 18.0;
  glMaterialfv(GL_FRONT, GL_SHININESS, &t);
  glMaterialfv(GL_FRONT, GL_SPECULAR, tmp);
 

  l.toFloat(tmp);
  glLightfv(GL_LIGHT0, GL_POSITION, tmp);
  

 
  /*  double a1 = 0.3; //ambient
  double s1 = 0.5; // specular
  double d1 = 0.5; //diffuse
  float ambient1[] = { a1, a1, a1, 1.0f };
  float specular1[] = {s1, s1, s1 , 1.0f};
  float diffuse1[] = { d1, d1, d1, 1.0f };
  
  
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
  glLightfv(GL_LIGHT1, GL_POSITION, tmp);
  */
  //float spec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  // glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
  
  
  glEnable( GL_TEXTURE_2D );
  glBindTexture(GL_TEXTURE_2D, texture);
  glDisable(GL_CULL_FACE);
  glEnable(GL_FOG);
  glFogf(GL_FOG_DENSITY, 0);		
  glFogf(GL_FOG_START, 7000);				 
  glFogf(GL_FOG_END, 8000);				
  glDisable(GL_FOG);
  //glEnable(GL_FOG);
  //BumpShader.InvertRadius = -100;
 
  glPushMatrix();
  glTranslatef(Position.x, Position.y, Position.z);

  RootNode.Calculate();
  glColor4f(1,1,1,1);
  BumpShader.NormalMap = normal;
  BumpShader.ColorMap = texture;

  
  
  BumpShader.Start();

  RootNode.Render();

  BumpShader.End();

  glPopMatrix();

  glDisable(GL_LIGHTING);
	
}

void CTentacle::Update(double t,double b) {
  RootNode.Update(t,b );
}
