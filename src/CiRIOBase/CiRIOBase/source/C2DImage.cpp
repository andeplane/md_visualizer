#include <memory.h>

#include "Stdafx.h"

#include <C2DImage.h>
//#include <VoronoiDiagramGenerator.h>


#include <time.h>

C2DImage::C2DImage() {
  Data = 0;
  zero.height = 0;
  PlanFFTInitialized = false;
  PlanFFTInvInitialized = false;
  in = 0;
  out = 0;
  perlin = new Perlin(8,1.0,1.0,rand()%10000);
 }

C2DImage::~C2DImage() {
  if (Data)
    delete[] Data;

  if (PlanFFTInitialized)
    fftw_destroy_plan(PlanFFT);

  if (PlanFFTInvInitialized)
    fftw_destroy_plan(PlanFFTInv);

  Data = 0;
}


void C2DImage::SetupFFT(fftw_complex **in, fftw_complex** out, double scale) {
 
  double B = Width*Width;
  if (!*out) *out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * B);
  if (!*in) *in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * B);

  for (int i=0;i<B;i++)  {
    complex<double> d = Data[i].height*scale; 
    (*in)[i][0] = d.real();
    (*in)[i][1] = d.imag();
  }
  
  for (int i=0;i<B;i++) {
    (*out)[i][0] = 0;
    (*out)[i][1] = 0;
  }

}


void C2DImage::Gaussian(double B) {
  int x[8] = {0,1,0,1,0,1,0,1};
  int y[8] = {0,0,1,1,0,0,1,1};

  int z[8] = {0,0,0,0,1,1,1,1};

  double sum = 0;
  CCounter cnt(Width*Width, "Setting up convolution");
  for (int i=0;i<Width; i++)
    for (int j=0;j<Width; j++)
      //for (int k=0;k<Width; k++) 
	{

	  cnt.Tick();
	  double dist = 1E10;
	  double val = 0;
	  for (int l=0;l<4;l++) {
	    double d = (CVector(x[l],y[l],0)*Width - CVector(i,j,0)).Length();
	    d/=Width;
	    dist = min(dist,d);
	    val += exp(-dist*dist/(2.0*B*B));

          }

        
	  sum+=val;
	  Get(i,j)->height = val;

      }
  //  sum/=2;
    for (int i=0;i<Width*Width;i++)
      Data[i].height/=sum;
  //cout << val << "  ";
  
}

void C2DImage::GaussianWithCross(double B) {
  int x[8] = {0,1,0,1,0,1,0,1};
  int y[8] = {0,0,1,1,0,0,1,1};

  int z[8] = {0,0,0,0,1,1,1,1};

  double sum = 0;
  CCounter cnt(Width*Width, "Setting up convolution");
  for (int i=0;i<Width; i++)
    for (int j=0;j<Width; j++)
      //for (int k=0;k<Width; k++) 
	{
	  cnt.Tick();
	double dist = 1E10;
	double val = 0;
	//double dist2 = (CVector(Width/2,Width/2,0) - CVector(i,j,0)).Length()/(float)Width;
	float b = 0.5;
	double dist2 = sqrt(pow((abs(Width/2-i))/(float)Width,b) + pow((abs(Width/2 -j))/(float)Width,b));

	for (int l=0;l<4;l++) {
	  double d = (CVector(x[l],y[l],0)*(Width) - CVector(i,j,0)).Length();
	  d/=Width;
	  dist = min(dist,d);
	  
	  }
	float C = 0.1;
	val = exp(-pow(dist2,2.0)/(2.0*C*C));
	float val2 = exp(-pow(dist,2)/(2.0*B*B));
	val +=4.0*val2*val;


	
	sum+=val;
	Get(i,j)->height = val;
      }
  //  sum/=2;
    for (int i=0;i<Width*Width;i++)
      Data[i].height/=sum;
  //cout << val << "  ";
  
}


void C2DImage::AddNoise(float val) {
  CCounter cnt(Width*Width, "Adding noise");
  for (int i=0;i<Width;i++) 
    for (int j=0;j<Width;j++) {

    float dx = 2.0*(i-Width/2)/(float)Width;
    float dy = 2.0*(j-Width/2)/(float)Width;

    float s = 1.0 - 0.25*pow(dx*dx*dx*dx + dy*dy*dy*dy, 4.0f)+ 0.3*dy;
    float ps = 5.0;
    Data[i + j*Width].height += CMath::RandomGauss()*val*s + 0.25*val*CMath::RandomGauss()*perlin->Get(ps*(i/(float)Width),ps*(j/(float)Width));
    cnt.Tick();
  }
}


void C2DImage::FFT(C2DImage& o) {

    SetupFFT(&in, &out, 1);
    int B = Width*Width;

    if (!PlanFFTInitialized) {
      PlanFFT = fftw_plan_dft_2d(Width, Width,
				 in, out, FFTW_FORWARD,
				 FFTW_MEASURE);
      SetupFFT(&in, &out, 1);
      PlanFFTInitialized = true;
    }

    fftw_execute(PlanFFT); /* repeat as needed */

    o.Create(Width, Height);

    ResolveFFT(out, &o);

    
    
}

void C2DImage::FFTInv(C2DImage& o) {
    
    double scale = 1.0/(double)(Width*Width);


    SetupFFT(&in, &out, scale);
    
    if (!PlanFFTInvInitialized) {
      PlanFFTInv = fftw_plan_dft_2d(Width, Width,
			 in, out, FFTW_BACKWARD,
			     FFTW_MEASURE);
      SetupFFT(&in, &out, scale);
      PlanFFTInvInitialized = true;
    }


    fftw_execute(PlanFFTInv); /* repeat as needed */
    
    o.Create(Width, Height);
 
    ResolveFFT(out, &o);
    
}



void C2DImage::ResolveFFT(fftw_complex* out, C2DImage* o) {
  for (int i=0;i<Width*Width;i++) 
    o->Data[i].height = complex<double>(out[i][0], out[i][1]);
  
}



float** C2DImage::toFitsData() {

  int N = Width;
  float** f = new float*[N];
  for (int i=0;i<N;i++)
    f[i] = new float[N];

  for (int i=0;i<N;i++) {
    for (int j=0;j<N;j++) {
      f[i][j] = 0;
    }
  }
  for (int i=0;i<Width;i++)
       for (int j=0;j<Width;j++) 
	 f[i][j] = Get(i,j)->height.real();
       
  return f;
}


void C2DImage::Stamp(int x, int y, C2DImage& target, float amplitude) {
  for (int i=0;i<Width;i++)
    for (int j=0;j<Height;j++) {
      target.Get(i+x-Width/2,j+y-Height/2)->height += Get(i,j)->height.real()*amplitude;
      //target.Get(i+x,j+y)->height += Get(i,j)->height.real()*amplitude;
    }

}


void C2DImage::putSmallCircle(int x, int y, int n, float s) {
  
  float a = 1;
  // n = 4 usually
  for (int i=0;i<n;i++)
    for (int j=0;j<n;j++) {
      float val = (0.5*n - sqrt( (i-n/2)*(i-n/2) + (j-n/2)*(j-n/2)))*a;
      Get(x +i -n/2 , y +j -n/2)->height+=val*s;
    }
}


void C2DImage::AddPerlinNoise(float perlinScale, float scale) {
  for (int i=0;i<Width;i++)
    for (int j=0;j<Height;j++)
      Get(i,j)->height+=perlin->Get((i/(float)Width)*perlinScale,(j/(float)Width)*perlinScale)*scale; 
}

void C2DImage::RenderParticles(CParticles& p, float w, float dx, float dy) {
  
  CCounter cnt(p.objects.size(), "Rendering particles");
  int N = Width;
  for (int i=0;i<p.objects.size();i++) {
    cnt.Tick();
    int x = (N/2) + ((p.objects[i].P.x - dx)*N) * w;
    int y = (N/2) + ((p.objects[i].P.y - dy)*N) * w;
    if ( x >= 0 && x < N && y >= 0 && y < N ) 
      {
	putSmallCircle(x,y, 40, 0.5);
      }
   }
      


}



void C2DImage::RenderStars(int n, int spread, int base, float rotation1, float rotation2, bool pointStar) {
  // render starsS
  CCounter cnt(n, "Rendering stars");
  for (int i=0;i<25;i++) {
    cnt.Tick();
    int x = rand()%Width;
    int y = rand()%Width;
    int size = abs(CMath::RandomGauss())*spread + base;
    if (rand()%8==1)
      size*=1.5;
    if (pointStar==false)
      putStar(x,y,size, 80 + rand()%20, 1.0, rotation1, rotation2);
    else {
      Get(x , y )->height=20000;
      Get(x+1 , y )->height=20000;
      Get(x-1 , y )->height=20000;
      Get(x , y+1 )->height=20000;
      Get(x , y-1 )->height=20000;
    }
      
  }
}


void C2DImage::putStar(int x, int y, int n, float s, float max, float rotation1, float rotation2) {
  
  float a = 1;
  // n = 4 usually
  float b = 1.5 + rand()%1000 / 1000.0;
  float c = 1.5 + rand()%1000 / 1000.0 * 1.0;

  float d = 0.3;// + rand()%1000/1000.0*0.1;

  
  CMatrix rot1, rot2;

  rot1.Identity();
  rot1.RotateXY(rotation1);
  rot2.Identity();
  rot2.RotateXY(rotation2);

  for (int i=0;i<n;i++)
    for (int j=0;j<n;j++) {
      float dx = (i-n/2)/(float)n;
      float dy = (j-n/2)/(float)n;
      float ddy = 0.7*dy + 0.3*dx;

      for (int k=0;k<1;k++) {
	CVector p(dx,dy,0);
	CVector p2(dx,ddy,0);
	if (k==1) {
	  p.Mul(rot1);
	  p2.Mul(rot1);
	}
	else {
	  p.Mul(rot2);
	  p2.Mul(rot2);
	}
	  
	float val = exp(-150*( p.x*p.x + p.y*p.y))*a;

	val += (c*(1.0f-sqrt(pow((float)abs(b*p2.x),(float)d) + pow((float)abs(b*p2.y),(float)d)))); 
	//float val = exp(-40*( dx*(dx) - dy*(dy)))*a;
	
	
	if (val>max) val = max;
	if (val<0.0) val = 0.0;
	val = val + CMath::RandomGauss()*sqrt(val);
	
	
	Get(x +i -n/2 , y +j -n/2)->height+=val*s;
      }
    }
}



void C2DImage::Convolve(float B) {
  C2DImage fftImage;
  C2DImage PSF, PSFF;
  PSF.Create(Width,Width);
  PSF.Gaussian(B);


  PSF.FFT(PSFF);
  FFT(fftImage);
  fftImage.Multiply(PSFF);
  fftImage.FFTInv(*this);

  PSF.Scale(112000);
  SaveFits("PSFtemp.fits","leuats superfits");
  PSF.SaveFits("PSF.fits","leuats superfits");


}


void C2DImage::SaveBMP(string file){
  CBitMap bmp;
  C2DField hf;
  hf.Create(Width, Height);
  for (int i=0;i<Width*Height;i++) {
    float d = Data[i].height.real();
    hf.Data[i].height = d;
  }
  hf.Normalize(255.0);
  bmp.Create(Width, Height);
  hf.ToBitMap(bmp, 1.0,1.0,1.0,1.0);
  bmp.SaveBMP(file);
}


void C2DImage::SaveFits(string filename, string header) {
   float** f = toFitsData();
   FILE *fp = fopen(filename.c_str(), "wb");
   set_fits_opf(fp);
   
   string s = header;
   char *a=new char[s.size()+1];
   a[s.size()]=0;
   memcpy(a,s.c_str(),s.size());
   
   fwrite_fits(f, Width, Width, 1, &a);
   
   fclose(fp);
   delete[] f;
   //return(status);
 }

void C2DImage::ApplyFlatField(C2DImage& flat) {
  for (int i=0;i<Width;i++)
    for (int j=0;j<Height;j++) {
      float v = flat.Get(i,j)->height.real();
      Get(i,j)->height += 1.0*v;
    }


}


void C2DImage::LoadFits(string filename) {
   FILE *fp = fopen(filename.c_str(), "rb");
   set_fits_ipf(fp);
   set_fits_opf(fp);
   
   float **f; 
   int N1;
   int N2;
   int comc;
   char* com;
   cout <<"Loading fits" <<endl;
   fread_fits(&f, &N1, &N2, &comc,&com);
   
   fclose(fp);

   cout <<"Loading fits of size: " << N1 << " x " <<N2 << endl;
   int N = 1024;
   Create(N, N);
   for (int i=0;i<Width;i++)
     for (int j=0;j<Height;j++) {
       if (j<N1 && i<N2) { 
	 Get(i,j)->height = f[i][j];///256.0;// + 65526/2;
       }
     }

   delete[] f;
   //return(status);
 }


/* void C2DImage::RenderRandomBackgroundGalaxies(int NO) {

   CCounter cnt(NO, "Rendering background galaxies");
    for (int i=0;i<NO;i++) {
      cnt.Tick();
      int x = rand()%Width;
      int y = rand()%Width;
      int size = abs(CMath::RandomGauss())*20 + 5;
      size*=1.8;
      if (i%50==0)
	size*=1.8;
      
      float f = abs(CMath::RandomGauss())*size*0.1 + 0.1;
      RenderGalaxy(x,y,size,f);
    }

 }
*/


void C2DImage::Lens(C2DImage& lens, C2DImage& result, float strength, float radius) {

  result.Create(Width, Width);

  float Ds = 1.0;
  float Dd = 0.5;
  float Dds = Dd + Ds; 
  CCounter cnt(Width*Width,"Lensing");

  for (int x=0;x<Width;x++) {
    //cout << (int)(x*100/Width) << "%" << endl;
    for (int y=0;y<Width;y++) {

      cnt.Tick();
      CVector alpha(0,0,0);
      CVector eta(x,y,0);
      int w = 20; 
      float xx = 10.0/float(w);///Dd;
      int n = 0;
      for (int i=0;i<w;i++)
	for (int j=0;j<w;j++)  {

	  CVector etam = CVector(float(i-w/2),float(j-w/2), 0);
	  etam.Add(eta);
	  CVector diff = eta;
	  diff.Subtract(etam);
	  if (diff.Length2()>0.01)
	    alpha.Add(diff * 1.0/diff.Length2() * lens.Get(etam.x, etam.y)->height.real()*strength*1.5);

	  n+=1.0; 
	}
      
      alpha = alpha*1.0/(float)n * Dds/Ds;
      
      CVector pos = CVector(x,y,0);// + alpha;
      pos.Subtract(alpha);
      result.Get(x,y)->height = Get(pos.x, pos.y)->height;
      
      
    }
  }
}


/*void C2DImage::RenderGalaxy(int x, int y, int size, float amplitude) {
  CGalaxy galaxy;
  galaxy.RenderGalaxy(size);
  galaxy.image.Stamp(x,y, this, 0.5*amplitude);


}
*/

float C2DImage::galaxyTanH(float r_in, float r_out, float theta_out, float r) {
  float CDEF = 0.23;
  float A = 2*CDEF / (abs(theta_out) + CDEF) - 1.00001;
  float B = (2-atanh(A))*(r_out)/(r_out - r_in);

  return 0.5*(tanh(B*(r/r_out -1) + 2.0) +1 );

}











void C2DImage::Line(int x0, int y0, int x1, int y1, double color, bool erase, int size)
    {
         int dy = y1 - y0;
        int dx = x1 - x0;
        float t = (float) 0.5;                      // offset for rounding

        //raster.setPixel(pix, x0, y0);
	  Dot(x0, y0, color, size);
        if (abs(dx) > abs(dy)) {          // slope < 1
            float m = (float) dy / (float) dx;      // compute slope
            t += y0;
            dx = (dx < 0) ? -1 : 1;
            m *= dx;
            while (x0 != x1) {
                x0 += dx;                           // step to next x value
                t += m;                             // add slope to y value
		Dot(x0,t,color,size);
            }
        } else {                                    // slope >= 1
            float m = (float) dx / (float) dy;      // compute slope
            t += x0;
            dy = (dy < 0) ? -1 : 1;
            m *= dy;
            while (y0 != y1) {
                y0 += dy;                           // step to next y value
                t += m;                             // add slope to x value
		  Dot(t, y0, color,size);
            }
        }
    }



void C2DImage::Max(double val ) {
  for (int i=0;i<Width*Width;i++)
    if (Data[i].height.real()>val) 
      Data[i].height = val;

}
void C2DImage::SubScalar(double val ) {
  for (int i=0;i<Width*Width;i++)
      {
      Data[i].height -= val;
      if (Data[i].height.real()<0) Data[i].height = 0;
    }

}


void C2DImage::Normalize(double scale ) {
  
  float max = 1E-20;
  float min = 1E20;
  for (int i=0;i<Width*Height;i++) {
    if ((Data[i].height.real())>(max))
      max = Data[i].height.real();
    if ((Data[i].height.real())<(min))
      min = Data[i].height.real();
  }
  for (int i=0;i<Width*Height;i++) {
    Data[i].height = scale*(Data[i].height.real()+min)*1.0/(max-min);
  }


}


void C2DImage::ExpVal(double a, double b, double c) {
  for (int i=0;i<Width*Height;i++)
    Data[i].height = a*exp((Data[i].height+c)*b);
}



void C2DImage::Create(int w, int h) {
  Width = w;
  Height = h;
  zero.height = 0;
  if (Data)
    delete[] Data;

  Data = new C2DImage_data[Width*Height];
  for (int i=0;i<Width*Height;i++)
    Data[i].height = 0.0;
}

C2DImage_data* C2DImage::Get(int x, int y) {
  /*if (x<0)
    x+=Width;
  if (x>=Width)
    x-=Width;

  if (y>=Height)
    y-=Height;

  if (y<0)
    y+=Height;
  */
  x = (x + Width*100) % Width;
  y = (y + Width*100) % Width;
  
  //if ((x>=0) && (x<Width) && (y>=0) && (y<Height))
  
  /*  if (x<0 || x>=Width || y < 0 || y >= Width)
      return &zero;
  */
  return &Data[x + y*Width];
  //  return 0;
}

  


void C2DImage::Smooth(int n) {
  C2DImage tmp;
  tmp.Create(Width, Height);
  int x[11] = {-1, 0, 1, -1,0,1, -1,0,1, 0, 0};
  int y[11] = {-1,-1,-1,  0,0,0,  1,1,1, 0,0 };
  CCounter tick(Width*Height*n, "Smoothing");

  for (int cnt = 0;cnt<n;cnt++) {
    for (int i=0;i<Width;i++)
      for (int j=0;j<Height;j++) {
	tick.Tick();
	double h = 0;
	for (int k=0;k<10;k++) {
	  h+=Get(i+x[k], j+y[k])->height.real();
	}
	tmp.Get(i,j)->height = h/10.0;
      }
    for (int i = 0;i<Width*Height;i++)
      Data[i] = tmp.Data[i];
  }
  cout << endl  <<"Done!" << endl;
}

void C2DImage::Dot(int x, int y, double h, int size) {
  for (int i=x-size;i<=x+size;i++) 
    for (int j=y-size;j<=y+size;j++) 
      Get(i,j)->height = h;

}



CVector C2DImage::CalculateNormal(int x, int y, double scale, CVector* tangent = 0) {
  int xx[8] = {-1, 0, 1, 1 ,1,0, -1,-1};
  int yy[8] = {-1,-1,-1,0,1,1,1,0};


  CVector c = CVector(x*scale,Get(x,y)->height.real(),y*scale);
  CVector v(0,0,0);
  CVector t(0,0,0);
  CVector v1,v2; 

  for (int i=0;i<8;i++) {
    int j =i+1; 
    if (j==8) j=0;


    v1 = CVector((x + xx[i])*scale,Get(x+xx[i],y+yy[i])->height.real(),(y+yy[i])*scale);
    v2 = CVector((x + xx[j])*scale,Get(x+xx[j],y+yy[j])->height.real(),(y+yy[j])*scale);

    v = v + (c - v2).Cross(c-v1).Normalize();
    t = t + v1;
  }
  if (tangent)
    *tangent = t*1.0/8.0;
  return v*1.0/8.0;

}






void C2DImage::Merge(C2DImage& o, double weight, int type){
  if (Width!=o.Width || Height!=o.Height)
    throw string("Maps not of same size in C2DImage::Merge"); 
  
  for (int i=0;i<Width*Height;i++)  {
    if (type==0) Data[i].height = (1.0-weight)*Data[i].height + weight*o.Data[i].height;
    if (type==1) Data[i].height = Data[i].height*o.Data[i].height*weight;
  }
}



void C2DImage::Add(C2DImage& o) {
  if (Width!=o.Width || Height!=o.Height)
    throw string("Maps not of same size in C2DImage::Add"); 
  for (int i=0;i<Width*Height;i++)
    Data[i].height = Data[i].height + o.Data[i].height;
}

void C2DImage::Subtract(C2DImage& o) {
  if (Width!=o.Width || Height!=o.Height)
    throw string("Maps not of same size in C2DImage::Subtract"); 
  for (int i=0;i<Width*Height;i++)
    Data[i].height -=o.Data[i].height;
}

void C2DImage::Multiply(C2DImage& o) {
  if (Width!=o.Width || Height!=o.Height) {
    cout << Width << " " << Height << endl;
    cout << o.Width << " " << o.Height << endl;
    throw string("Maps not of same size in C2DImage::Multiply : "); 
  }

  for (int i=0;i<Width*Height;i++)
    Data[i].height *=o.Data[i].height;
}
void C2DImage::Divide(C2DImage& o) {
  if (Width!=o.Width || Height!=o.Height)
    throw string("Maps not of same size in C2DImage::Divide"); 
  for (int i=0;i<Width*Height;i++)
    Data[i].height /=o.Data[i].height;
}

void C2DImage::Scale(double o) {
  for (int i=0;i<Width*Height;i++)
    Data[i].height =Data[i].height*o;
}


void C2DImage::MakeSeamless(double scale) {
  C2DImage tmp, tmp2;
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

void C2DImage::Circle(int type, double val, double elipse) {
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


