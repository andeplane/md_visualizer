#include "Stdafx.h"

#include <C2DField.h>
//#include <VoronoiDiagramGenerator.h>

#include <time.h>

C2DField::C2DField() {
  Data = 0;
#ifndef NO_OPENGL
  BumpShader = 0;
#endif
}

C2DField::~C2DField() {
  if (Data)
    delete[] Data;
#ifndef NO_OPENGL
  if (BumpShader)
    delete BumpShader;

  BumpShader = 0;
#endif
  Data = 0;
}


void C2DField::Erase(int x,int y) {
  double v = Get(x,y)->height;
  double n = 0;
  double h = 0;
  C2DField_data* v1 = Get(x-1,y);
  C2DField_data* v2 = Get(x,y-1);
  C2DField_data* v3 = Get(x+1,y);
  C2DField_data* v4 = Get(x,y+1);
  if (v1 && v1->height!=v)
    {h+=v1->height;n++; }
  if (v2 && v2->height!=v)
    {h+=v2->height;n++; }
  if (v3 && v3->height!=v)
    {h+=v3->height;n++; }
  if (v4 && v4->height!=v)
    {h+=v4->height;n++; }

  Get(x,y)->height = (h/(double)n);


}
void C2DField::Line(int x0, int y0, int x1, int y1, double color, bool erase, int size)
    {
         int dy = y1 - y0;
        int dx = x1 - x0;
        float t = (float) 0.5;                      // offset for rounding

        //raster.setPixel(pix, x0, y0);
	if (!erase)
	  Dot(x0, y0, color, size);
	else
	  Erase(x0,y0);
        if (abs(dx) > abs(dy)) {          // slope < 1
            float m = (float) dy / (float) dx;      // compute slope
            t += y0;
            dx = (dx < 0) ? -1 : 1;
            m *= dx;
            while (x0 != x1) {
                x0 += dx;                           // step to next x value
                t += m;                             // add slope to y value
		if (!erase)
		  Dot(x0,t,color,size);
		else
		  Erase(x0,t);
            }
        } else {                                    // slope >= 1
            float m = (float) dx / (float) dy;      // compute slope
            t += x0;
            dy = (dy < 0) ? -1 : 1;
            m *= dy;
            while (y0 != y1) {
                y0 += dy;                           // step to next y value
                t += m;                             // add slope to x value
		if (!erase)
		  Dot(t, y0, color,size);
		else Erase(t,y0);
            }
        }
    }




double C2DField::GetDiamondVal(double scale) {

	double d = (CMath::RandomUniform() -0.5);
  double d2 = (CMath::RandomUniform() - 0.5);
  double d3 = (CMath::RandomUniform() - 0.5);
  double val = 0;
  val = val+   scale*4.0*d;
  /*  val = val + abs(exp(-0.005*pow(scale-64.0,2.0)))*1000.0*(d2);

  val = val + abs(exp(-0.005*pow(scale-32.0,2.0)))*400.0*(d3);
  */
  //val = val  + exp(-0.005*pow(scale-16.0,2))*15.0*d2;

  //val = val  + exp(-0.0005*pow(scale-200.0,2))*1000.0*d3;


  //  val = val  + exp(-0.000025*pow(scale-400.0,2))*600.0*d2;
    //Util::randgauss()*scale;;
  return val;
}


void C2DField::DiamondStep(int x, int y, int add) {
  int n=0;
  double yh =0;
  C2DField_data* v1 = Get(x-add ,y);
  C2DField_data* v2 = Get(x+add ,y);
  C2DField_data* v3 = Get(x ,y -add);
  C2DField_data* v4 = Get(x ,y +add);

  if (v1&&v2) {
    yh+=v1->height + v2->height;n+=2;
  }
  if (v3&&v4) {
    yh+=v3->height + v4->height;n+=2;
  }
  if (n!=0)
    
    yh/=(double)n;
  else
    yh = 0;
  
  v1 = Get(x ,y);
  if (v1) 
    v1->height = yh + GetDiamondVal(add*2);

}

void C2DField::RGBScale(double scale ) {
  Normalize(255*scale);
}

void C2DField::Normalize(double scale ) {
  
  float max = 1E-20;
  float min = 1E20;
  for (int i=0;i<Width*Height;i++) {
    if (Data[i].height>max)
      max = Data[i].height;
    if (Data[i].height<min)
      min = Data[i].height;
  }

  for (int i=0;i<Width*Height;i++) {
    Data[i].height = scale*(Data[i].height-min)*1.0/(max-min);
  }


}

void C2DField::Diamond() {

  // verify that size is correct
  if (Width!=Height)
    throw string("Error: width is not equal height in C2DField::Diamond");
  double mm = -1;
  for (int i=1;i<=10;i++) {
    if (pow(2.0,(double)i)==Width)
      mm = i+1;
  }
  //cout << mm << endl;
	srand(time(NULL));
  double hh = 1;
  Get(0,0)->height = CMath::RandomGauss()*hh;
  Get(Width-1,0)->height = CMath::RandomGauss()*hh;
  Get(Width-1,Width-1)->height = CMath::RandomGauss()*hh;
  Get(0,Width-1)->height = CMath::RandomGauss()*hh;

	for (int i=0;i<mm;i++) {
    int w = pow(2.0,i) ;

    int add = Width / w;
    int h = add/2;
    //cout << add << " h:" << h << endl;
    for (int x=0;x<w;x++) {
      for (int y=0;y<w;y++) {
	// diamond step
	double val = GetDiamondVal(add);
	C2DField_data* v = Get(x*add + h, y*add + h);
	C2DField_data* v1 = Get(x*add , y*add );
	C2DField_data* v2 = Get(x*add + add, y*add );
	C2DField_data* v3 = Get(x*add + add, y*add + add);
	C2DField_data* v4 = Get(x*add, y*add + add);
	v->height =0; 
	int n=0;
	if (v1) {v->height+=v1->height;n++;}
	if (v2) {v->height+=v2->height;n++;}
	if (v3) {v->height+=v3->height;n++;}
	if (v4) {v->height+=v4->height;n++;}
	
	v->height=v->height/(double)n;
	v->height+=val;
	//if (i<5) 
	//   cout << w << "  add:" << add  << " Y: " << v->v.y << endl;
	if (i!=mm-1) {
	  DiamondStep(x*add +h, y*add,h);
	  DiamondStep(x*add,    y*add+h,h);
	  DiamondStep(x*add +h, y*add+2*h,h);
	  DiamondStep(x*add +2*h, y*add+h,h);
	
	}
      }
    }
  }

}

void C2DField::ExpVal(double a, double b, double c) {
  for (int i=0;i<Width*Height;i++)
    Data[i].height = a*exp((Data[i].height+c)*b);
}

void C2DField::ToBitMap(CBitMap& map, float r, float g, float b, float s) {
  if (!map.data)
    map.Create(Width, Height);
  //for (int i=0;i<3*Width*Height;i++)
  //  map.data[i] = 128;

  for (int i=0;i<Width;i++)
    for (int j=0;j<Height;j++) {
      map.data[3*i  + j*Width*3 ] = (unsigned char)CMath::Minmax(map.data[3*i  + j*Width*3 ] +r*(Get(i,j)->height)*s,0,255) ;//+ rand()%255 ;
      map.data[3*(i + j*Width) + 1] = (unsigned char)CMath::Minmax(map.data[3*(i + j*Width) + 1]+g*(Get(i,j)->height)*s,0,255);
      map.data[3*(i + j*Width) + 2] = (unsigned char)CMath::Minmax(map.data[3*(i + j*Width) + 2]+b*(Get(i,j)->height)*s,0,255);
    }
  
}


void C2DField::CreateWindows(CBitMap& map, CVector size, CVector dist, CVector color) {
  bool ok = true;
  int px =0 ;
  int py=0;
  for (unsigned int x=0;x<map.width;x++) 
    for (unsigned int y=0;y<map.height;y++) {
      CVector C(0,0,0);
      double posx = x/size.x;
      double posy = y/size.y;
      double xx = (x/(double)map.width)*100.0;
      double yy = (y/(double)map.height)*100.0;
      int X = (int)xx %((int)size.x);
      int Y = (int)yy %((int)size.y);
      double d = CMath::Minmax(0.3*sqrt((X-xx)*(X-xx) +(Y-yy)*(Y-yy)),0,1);
      float b= (1.0-d) +  d*(rand()%1000/1000.0);
      if ((X > dist.x) && (Y > dist.y) && ok)
	C = color*255.0*b;
     

      ok = true;
      if ((int)posx%11>8) ok=false;
      if ((int)posy%21>16) ok=false;

      
      //if (rand()%10==9) ok=!ok;
      
      //ok = true;

      map.data[3*(x+y*map.height)+0]=C.x;
      map.data[3*(x+y*map.height)+1]=C.y;
      map.data[3*(x+y*map.height)+2]=C.z;
    }
  

}


void C2DField::FromBitMap(CBitMap& map) {
  for (int i=0;i<Width*Height;i++) {
    Data[i].height = map.data[3*i   +0] + map.data[3*i  +1] + map.data[3*i  +2];
  }
  
  Normalize(1.0);
 
}


void C2DField::ToBitMapForce(CBitMap& map, float r, float g, float b, float s) {
  if (!map.data)
    map.Create(Width, Height);
  //for (int i=0;i<3*Width*Height;i++)
  //  map.data[i] = 128;

  for (int i=0;i<Width;i++)
    for (int j=0;j<Height;j++) {
      map.data[3*i  + j*Width*3 +0] = (unsigned char)max((float)map.data[3*i  + j*Width*3 +0],r*(Get(i,j)->height)*s) ;
      map.data[3*i  + j*Width*3 +1] = (unsigned char)max((float)map.data[3*i  + j*Width*3 +1],g*(Get(i,j)->height)*s) ;
      map.data[3*i  + j*Width*3 +2] = (unsigned char)max((float)map.data[3*i  + j*Width*3 +2],b*(Get(i,j)->height)*s) ;
    }
  
}


void C2DField::Perlin() {

}

void C2DField::Create(int w, int h) {
  Width = w;
  Height = h;
  if (Data)
    delete[] Data;

  Data = new C2DField_data[Width*Height];
  for (int i=0;i<Width*Height;i++)
    Data[i].height = 0.0;
}

C2DField_data* C2DField::Get(int x, int y) {
  if (x<0)
    x+=Width;
  if (x>=Width)
    x-=Width;

  if (y>=Height)
    y-=Height;

  if (y<0)
    y+=Height;
  
  //if ((x>=0) && (x<Width) && (y>=0) && (y<Height))
    return &Data[x + y*Width];
  //  return 0;
}


void C2DField::ErodeVertex(C2DField_data* v, C2DField_data* u, double t, double s) {
  double d = u->height - v->height;
  if (d>t) {
    u->height-=s;
    v->height+=s;
  }
}


void C2DField::Erode(int cnt) {
  double t = 0.5;
  double s = 0.2;
  for (int i=0;i<cnt;i++)
  for (int x=0;x<Width;x++) {
    for (int y=0;y<Height;y++) {
      C2DField_data* v = Get(x,y);
      ErodeVertex(v, Get(x+1,y),t,s);
      ErodeVertex(v, Get(x-1,y),t,s);
      ErodeVertex(v, Get(x,y+1),t,s);
      ErodeVertex(v, Get(x,y-1),t,s);

      ErodeVertex(v, Get(x-1,y-1),t,s);
      ErodeVertex(v, Get(x+1,y-1),t,s);
      ErodeVertex(v, Get(x-1,y+1),t,s);
      ErodeVertex(v, Get(x+1,y+1),t,s);
    }
  }

}


void C2DField::FlowWater(C2DField_data* v, C2DField_data* u) {
  if (!u) return;
  double waterscale = 0.01;
  //double heightscale = 100000; // denne var kul
  double heightscale = 1;

  double wv = v->water*waterscale;
  double wu = u->water*waterscale;

  double au = u->height*heightscale;
  double av = v->height*heightscale;
  
  double dw = min(wv, (wv+av) - (wu+au));
  double chunk = 1.0;

  if (dw<=0.0) {
    // stop sedments
    v->height+=v->sedment;
    v->sedment=0;
    //v->water-=0.1;
    if (v->water<0) v->water=0;
  }
  else
    {
      double h = 10;// + abs(v->v.y - u->v.y);
      if (v->water<h)
	h = v->water;
      v->water-=h;
      u->water+=h;
      
      u->sedment += chunk;
      v->height-=chunk;
    }

}


void C2DField::Fill(int bx, int by, int x, int y, float base, float c, int type) {
  C2DField_data* d = Get(x,y);
   if (!d) return;
  double val = d->height;
  if (x<0) return;
  if (x>=Width) return;
  if (y<0) return;
  if (y>=Height) return;
  
  //float xd[9] = {-1,0,1,-1,0,1,-1,0,1};
  //float yd[9] = {-1,-1,-1,0,0,0,1,1,1};
  float xd[4] = {-1,1, 0 ,0};
  float yd[4] = {0,0,-1,1};
  
  if (c == d->height)
    //return;
    c = c*1.0001;
  
  if (base==-1)  {
    base = val;
  }
  if ((float)val == (float)base && base == c)// == (float)c)
    return;

  //cout << "(" << x  << ","<< y <<": " << d->height <<", " << base << " ," << c << ") ";
 
  if (val==base) {
    d->height = c;
    for (int i=0;i<4;i++) {
      if (x+xd[i]>=0 && x+xd[i]<Width && y+yd[i]>=0 && y+yd[i]<Height) {
	
      }
      if (Get(x + xd[i],y + yd[i])->height==base) {
	double dist = c;
	if (type==0 || type==1)
	  dist = 50.0 + sqrt((double)(bx-x- xd[i])*(bx-x- xd[i]) + (double)(by-y- yd[i])*(by-y- yd[i]))*5000.0 / (double)Width;
	if (type==3 )												        dist = pow((double)(bx-x- xd[i])*(bx-x- xd[i]) + (double)(by-y- yd[i])*(by-y- yd[i]),1.8)*5.0 / (double)Width;
	if (dist>5000) dist = 5000;
	Fill(bx,by, x+xd[i],y + yd[i],base, dist, type);
      }
    }    
  }
}

void C2DField::Smooth(int n) {
  C2DField tmp;
  tmp.Create(Width, Height);
  for (int cnt = 0;cnt<n;cnt++) {
    for (int i=0;i<Width;i++)
      for (int j=0;j<Height;j++) {
	int x[10] = {-1,0,1, -1,0,1, -1,0,1,0};
	int y[10] = {-1,-1,-1, 0,0,0, 1,1,1,0};
	double h = 0;
	for (int k=0;k<10;k++) {
	  h+=Get(i+x[k], j+y[k])->height;
	}
	tmp.Get(i,j)->height = h/10.0;
      }
    for (int i=0;i<Width*Height;i++)
      Data[i] = tmp.Data[i];
  }
}

void C2DField::SmoothY(int n) {
  C2DField tmp;
  tmp.Create(Width, Height);
  for (int cnt = 0;cnt<n;cnt++)  {
  for (int i=0;i<Width;i++)
    for (int j=0;j<Height;j++) {
      //int x[10] = {-1,0,1, -1,0,1, -1,0,1,0};
      //int y[10] = {-1,-1,-1, 0,0,0, 1,1,1,0};
      int d = 7;
      int y[7] = {-3,-2,-1,0,1,2,3}; 
      double h = 0;
      for (int k=0;k<d;k++) {
	h+=Get(i, j+y[k]*3)->height;
      }
      tmp.Get(i,j)->height = h/d;
    }
  for (int i=0;i<Width*Height;i++)
    Data[i] = tmp.Data[i];
  }
}

void C2DField::SmoothX(int n) {
  C2DField tmp;
  tmp.Create(Width, Height);
  for (int cnt = 0;cnt<n;cnt++)  {
  for (int i=0;i<Width;i++)
    for (int j=0;j<Height;j++) {
      //int x[10] = {-1,0,1, -1,0,1, -1,0,1,0};
      //int y[10] = {-1,-1,-1, 0,0,0, 1,1,1,0};
      int d = 7;
      int y[7] = {-3,-2,-1,0,1,2,3}; 
      double h = 0;
      for (int k=0;k<d;k++) {
	h+=Get(i+y[k]*3, j)->height;
      }
      tmp.Get(i,j)->height = h/d;
    }
  for (int i=0;i<Width*Height;i++)
    Data[i] = tmp.Data[i];
  }
}


void C2DField::Dot(int x, int y, double h, int size) {
  for (int i=x-size;i<=x+size;i++) 
    for (int j=y-size;j<=y+size;j++) 
      Get(i,j)->height = h;

}


/*void C2DField::Voronoi(int n, bool removelines, int type, double xx, double yy, int size) {
  VoronoiDiagramGenerator vg;
  float* x = new float[n];
  float* y = new float[n];
  srand(time(0));
  for (int i=0; i<n;i++) {
    bool nok = true;
    while (nok) {
      x[i] = rand()%Width;
      y[i] = rand()%Height;
      nok = false;
      for (int j=0;j<i;j++) {
	double d = sqrt ( pow((double)(x[i]-x[j]),2.0) + pow((double)(y[i]-y[j]),2.0));
	if (d<5)
	  nok=true;
      }
           if (xx!=0.0) 
	if (sin((double)x[i]/xx)>rand()%1000/10000.0)
	  nok = true;

      if (yy!=0.0) 
	if (sin((double)y[i]/yy)>rand()%1000/10000.0)
	  nok = true;
      
    }
  }

  vg.generateVoronoi(x,y,n,-1,Width+1,-1,Height+1);
  vg.resetIterator();
  bool ok=true;
  while (ok) {
    float x1,y1,x2,y2;
    ok = vg.getNext(x1,y1,x2,y2);
    Line(x1,y1,x2,y2,500,false,size);
  }

  double val = 0.0000001;
  if (type!=4)
  for (int i=0; i<n;i++) {
    if (type==2) val = (0.5 + rand()%100/100.0)*1000;
     Fill(x[i],y[i],x[i],y[i],-1, val, type); //(0.5 + rand()%100/100.0) );
   }
 
  if (removelines) 
    {
    vg.resetIterator();
    ok=true;
    while (ok) {
      float x1,y1,x2,y2;
      ok = vg.getNext(x1,y1,x2,y2);
      Line(x1,y1,x2,y2,500,true, size);
    }
  }
}

*/
CVector C2DField::CalculateNormal(int x, int y, double scale, CVector* tangent = 0) {
  int xx[8] = {-1, 0, 1, 1 ,1,0, -1,-1};
  int yy[8] = {-1,-1,-1,0,1,1,1,0};


  CVector c = CVector(x*scale,Get(x,y)->height,y*scale);
  CVector v(0,0,0);
  CVector t(0,0,0);
  CVector v1,v2; 

  for (int i=0;i<8;i++) {
    int j =i+1; 
    if (j==8) j=0;


    v1 = CVector((x + xx[i])*scale,Get(x+xx[i],y+yy[i])->height,(y+yy[i])*scale);
    v2 = CVector((x + xx[j])*scale,Get(x+xx[j],y+yy[j])->height,(y+yy[j])*scale);

    v = v + (c - v2).Cross(c-v1).Normalize();
    t = t + v1;
  }
  if (tangent)
    *tangent = t*1.0/8.0;
  return v*1.0/8.0;

}


void C2DField::SaveBMP(string file){
  CBitMap bmp;
  C2DField hf;
  hf.Create(Width, Height);
  for (int i=0;i<Width*Height;i++)
    hf.Data[i] = Data[i];
  hf.Normalize(255.0);
  bmp.Create(Width, Height);
  hf.ToBitMap(bmp, 1.0,1.0,1.0,1.0);
  bmp.SaveBMP(file);
}


void C2DField::Bump(CVector sun, double s, double flip, double minval) {
  Normalize(1);
  C2DField tmp;
  tmp.Create(Width, Height);
  sun.Normalize();
  for (int i=0;i<Width;i++)
    for (int j=0;j<Height;j++) {
      CVector n = CalculateNormal(i,j,s);
      float val = flip*n.Dot(sun);
      double d = pow((double)val,1.00);
      //if (val<0) d=-d;
      if (d<minval) d=minval;
     
      //cout << Get(i,j)->height << endl;
      
      tmp.Get(i,j)->height += d;
    }
  for (int i=0;i<Width*Height;i++) {
    Data[i] = tmp.Data[i];
  }
}

void C2DField::CreateNormalMap(double s, CBitMap* dest) {
  Normalize(1);
  dest->Create(Width, Height);
  for (int i=0;i<Width;i++)
    for (int j=0;j<Height;j++) {
      CVector n = (CalculateNormal(i,j,s)).Normalize();
      
      dest->data[3*i  + j*Width*3 +2 ] = (unsigned char)(0.5*(-n.y+1.0)*255.0); 
      dest->data[3*i  + j*Width*3 +1 ] = (unsigned char)(0.5*(n.z+1.0)*255.0); 
      dest->data[3*i  + j*Width*3 +0 ] = (unsigned char)(0.5*(-n.x+1.0)*255.0); 
     
    }
}

void C2DField::CreateNormals(double s) {
  CVector t;
  for (int i=0;i<Width;i++)
    for (int j=0;j<Height;j++) {
      
      CVector n = (CalculateNormal(i,j,s,&t)).Normalize();
      Data[i + j*Width].normal = n;
      Data[i + j*Width].tangent = t;
    }
}



void C2DField::Merge(C2DField& o, double weight, int type){
  if (Width!=o.Width || Height!=o.Height)
    throw string("Maps not of same size in C2DField::Merge"); 
  
  for (int i=0;i<Width*Height;i++)  {
    if (type==0) Data[i].height = (1.0-weight)*Data[i].height + weight*o.Data[i].height;
    if (type==1) Data[i].height = Data[i].height*o.Data[i].height*weight;
  }
}


void C2DField::Water(int watercnt, int cnt) {

  //vertex* tmp = new vertex[width*width];
  for (int i=0;i<Width*Height;i++) {
    Data[i].water = watercnt;
    Data[i].sedment = 0;

  }
  C2DField hf;
  hf.Create(Width,Height);
  for (int i=0;i<Width*Height;i++)
    hf.Data[i] = Data[i];
  
  for (int i=0;i<cnt;i++) {
    
    for (int x=0;x<Width;x++) {
      for (int y=0;y<Height;y++) {
	C2DField_data* v = Get(x,y);
	FlowWater(v, Get(x+1,y));
	FlowWater(v, Get(x-1,y));
	FlowWater(v, Get(x,y+1));
	FlowWater(v, Get(x,y-1));
	
	FlowWater(v, Get(x-1,y-1));
	FlowWater(v, Get(x+1,y-1));
	FlowWater(v, Get(x-1,y+1));
	FlowWater(v, Get(x+1,y+1));
    }
    
  }
    /*  for (int i=0;i<Width*Height;i++)
    hf.Data[i] = Data[i];
    */
  }
}


void C2DField::Sphere(double mul, bool typ, double dec) {
  double val;
  
  for (int i=0;i<Width; i++) {
    for (int j=0;j<Height; j++) {
      double x = (i-Width/2)/(double)Width;
      double y = (j-Height/2)/(double)Height;
      
      if (typ) 
	val = mul/sqrt(x*x + y*y);            
      else
	val = mul*sqrt(x*x + y*y);            
      
      
      if (typ) {
	val = (val + 0.9*(1-sqrt(x*x + y*y)))*0.7;
      }
      val = val - 4*dec;
      
      if (val>1.0) val=1.0;
      if (val<0.00) val=0.0;

      Get(i,j)->height = val;
      //bmp.data[3*i + 3*w*j  +0] = (unsigned char)(255.0*(col1.x * val + col2.x*(1-val)));
      //bmp.data[3*i + 3*w*j  +1] = (unsigned char)(255.0*(col1.y * val + col2.y*(1-val)));
      // bmp.data[3*i + 3*w*j  +2] = (unsigned char)(255.0*(col1.z * val + col2.z*(1-val)));
    }   
  }
  
   
}


void C2DField::Add(C2DField& o) {
  if (Width!=o.Width || Height!=o.Height)
    throw string("Maps not of same size in C2DField::Add"); 
  for (int i=0;i<Width*Height;i++)
    Data[i].height +=o.Data[i].height;
}

void C2DField::Subtract(C2DField& o) {
  if (Width!=o.Width || Height!=o.Height)
    throw string("Maps not of same size in C2DField::Subtract"); 
  for (int i=0;i<Width*Height;i++)
    Data[i].height -=o.Data[i].height;
}

void C2DField::Multiply(C2DField& o) {
  if (Width!=o.Width || Height!=o.Height)
    throw string("Maps not of same size in C2DField::Multiply"); 
  for (int i=0;i<Width*Height;i++)
    Data[i].height *=o.Data[i].height;
}

void C2DField::Scale(double o) {
  for (int i=0;i<Width*Height;i++)
    Data[i].height *=o;
}


void C2DField::MakeSeamless(double scale) {
  C2DField tmp, tmp2;
  tmp.Create(Width, Width);
  tmp2.Create(Width, Width);
  for (int i=0;i<Width;i++)
    for (int j=0;j<Height;j++) {
      tmp.Get(i,j)->height = Get(i+Width/2, j+Height/2)->height;
      tmp2.Get(i,j)->height = Get(i,j)->height;
    }

  for (int i=0;i<Width;i++)
    for (int j=0;j<Height;j++) {
      double w1 = (double)abs(i-Width/2.0)/(double)Width*scale;
      double w2 = (double)abs(j-Height/2.0)/(double)Height*scale;
      double w = min(w1,w2);
      // double w = sqrt(w1*w1 + w2*w2);
      w = CMath::Minmax(w,0,1);
      Get(i,j)->height = tmp.Get(i,j)->height*(w) + tmp2.Get(i,j)->height*(1-w);
    }
  

}

void C2DField::Circle(int type, double val, double elipse) {
  for (int i=0;i<Width;i++)
    for (int j=0;j<Height;j++) {
      double x = (i-Width/2.0)/(double)Width;
      double y = (j-Height/2.0)/(double)Height;
      double r = sqrt(x*x + y*y*elipse);
      //if (r<0.001) r=0.001;
      double d = 0;
      if (type==0)
	d = CMath::Minmax(val * r,0,255);
      if (type==1) {
	d = CMath::Minmax(0.01*val / r,0,255);
	//cout << d << " " << endl;
      }
      Get(i,j)->height = d;
    }
  
}

#ifndef NO_OPENGL

void C2DField::RenderTerrainSimple(RenderContainer* rc, double scale, double ts, GLuint texture, GLuint normaltexture) {

  if (BumpShader==0) {
    BumpShader = new CBumpShader();
    BumpShader->Initialize("C2DFieldBumpShader");

  }
  //cout << normaltexture  << "    " << texture << endl;
  glPushMatrix();
  glEnable( GL_TEXTURE_2D );
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  glDisable(GL_ALPHA_TEST);
  glDisable(GL_CULL_FACE);
  glDisable(GL_FOG);
  BumpShader->NormalMap = normaltexture;
  BumpShader->ColorMap = texture;

  BumpShader->Start();
  glTranslatef(0,-25 -Data[Width/2+(Height/2)*Width].height,0);
  glBegin(GL_QUADS);
  CVector v1,v2,v3,v4;
  CVector t1,t2,t3,t4;
  CVector n1,n2,n3,n4;
  glColor4f(1,1,1,1);

  for (int x=0;x<Width-1;x++) {
    for (int y=0;y<Height-1;y++) {
      v1 = CVector((x-Width/2.0)*scale, Data[x+y*Width].height, (y-Width/2.0)*scale);
      v2 = CVector((x+1-Width/2.0)*scale, Data[x +1+y*Width].height, (y-Width/2.0)*scale);
      v3 = CVector((x+1-Width/2.0)*scale, Data[x +1+(y+1)*Width].height, (y+1-Width/2.0)*scale);
      v4 = CVector((x-Width/2.0)*scale, Data[x+(y+1)*Width].height, (y+1-Width/2.0)*scale);

      n1 = Data[x+y*Width].normal*-1;
      n2 = Data[x+1+y*Width].normal*-1;
      n3 = Data[x+1+(y+1)*Width].normal*-1;
      n4 = Data[x+(y+1)*Width].normal*-1;

      t1 = Data[x+y*Width].tangent;
      t2 = Data[x+1+y*Width].tangent;
      t3 = Data[x+1+(y+1)*Width].tangent;
      t4 = Data[x+(y+1)*Width].tangent;


      glTexCoord2f(v1.x*ts,v1.z*ts);
      glNormal3f(n1.x, n1.y, n1.z);
      BumpShader->Tangent(t1);
      glVertex3f(v1.x, v1.y, v1.z);

      glTexCoord2f(v2.x*ts,v2.z*ts);
      glNormal3f(n2.x, n2.y, n2.z);
      BumpShader->Tangent(t2);
      glVertex3f(v2.x, v2.y, v2.z);

      glTexCoord2f(v3.x*ts,v3.z*ts);
      glNormal3f(n3.x, n3.y, n3.z);
      BumpShader->Tangent(t3);
      glVertex3f(v3.x, v3.y, v3.z);



      glTexCoord2f(v4.x*ts,v4.z*ts);
      glNormal3f(n4.x, n4.y, n4.z);
      BumpShader->Tangent(t4);
      glVertex3f(v4.x, v4.y, v4.z);


    }
  }
  glEnd();
  BumpShader->End();
  glPopMatrix();

  glDisable( GL_TEXTURE_2D );
  glDisable(GL_LIGHTING);
  glDisable(GL_LIGHT0);

}
#endif
