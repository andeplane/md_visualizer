#include "Stdafx.h"


#include <CTextureMachine.h>

CTextureMachine::CTextureMachine() {
  HeightFields = 0;
  MaxHF = -1;
  Size = -1;
  CTexturePointer = 0;
}
CTextureMachine::~CTextureMachine() {
  if (HeightFields)
    delete[] HeightFields;
}
void CTextureMachine::Initialize(CTexture* CT, int w, int h) {
  CTexturePointer = CT;
  Width = w;
  Height = h;
}

void CTextureMachine::LoadFromFile(string fname, bool verbatim=false) { 
    CUtil::verify_file(fname);
    fstream f(fname.c_str(), ios::in);
    char s[2000];
    int line = 0;
    string er = "";
    while(!f.eof()) {
      line++;
       f.getline(s,2000);
      vector<string> tok;
      CUtil::Tokenize(s, tok," ");
      if (tok.size()!=0)
	try {
	  string cmd = "";
	  for (unsigned int i=0; i<tok.size();i++)
	    cmd = cmd+tok.at(i) + " ";
	  Parse(cmd, verbatim);      
          
	} 
	catch (string s) {
	  throw s;
	}
	catch (...) {
	  throw string("Unknown error in CTextureMachine::LoadFromFile parsing "+fname);
	}
    }
    cout << "Done!" << endl;
} 

void CTextureMachine::Stamp(CVector pos, double rot, double size, CVector color, int id, bool randomorientation) {
  glPushMatrix();
  glTranslatef(pos.x, pos.y, pos.z);
  glRotatef(rot,0,0,1);
  if (randomorientation) 
    glRotatef(rand()%360,CMath::RandomUniform(),CMath::RandomUniform(),CMath::RandomUniform());

  glColor4f(color.x, color.y, color.z,1);
  glEnable(GL_TEXTURE_2D );
  glBindTexture(GL_TEXTURE_2D, id);
    
    double c = size;
    glDisable(GL_LIGHT0);
    glBegin(GL_QUADS);

    glTexCoord2f(0,0);
    glVertex3f(-c,-c,0);

    glTexCoord2f(1,0);
    glVertex3f(c,-c,0);

    glTexCoord2f(1,1);
    glVertex3f(c,c,0);

    glTexCoord2f(0,1);
    glVertex3f(-c,c,0);

    glEnd();
    glDisable(GL_TEXTURE_2D);

  

  glPopMatrix();
}

void CTextureMachine::RenderLeaves(int no,  string leaf, string toTexture, double leaf_size, double size_spread, bool randomorientation = false) {
  int size = 512;
  glViewport(0,0,size, size);
  glClearColor(0,0,0,0);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_STENCIL_BUFFER_BIT); 
  COpenGL ogl;
  ogl.camera = CVector(00,0,250);
  ogl.target = CVector(0,0,0);
  ogl.ypr = CVector(0,1,0);
  ogl.setup_camera();
  glDisable(GL_FOG);
  glDisable(GL_CULL_FACE);
  GLuint lf = CTexturePointer->gettexture(leaf);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER,0.2);
  for (int i=0;i<no;i++) {
    int x,y;
    bool ok=false;
    while (!ok) {
      x = rand()%512 - 256;
      y = rand()%400 - 200;
      ok = true;
      if (sqrt((double)(x*x +y*y))>240) ok= false;
    }
    Stamp(CVector(x,y, 0), rand()%360, leaf_size+rand()%(int)size_spread - (size_spread/2), CVector(1,1,1), lf, randomorientation); 
  }
  glDisable(GL_ALPHA_TEST);
  
  GLuint id = CTexturePointer->generate_texture(toTexture);;
  //  ogl.buffer2texture(id, size,size, ogl.NOMIPMAP);
  ogl.buffer2texture(id, size,size, ogl.MIPMAP);
  CTexturePointer->gettexture_struct(toTexture)->has_alpha = true;

 
  glViewport(0,0,Width, Height);
  //glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_STENCIL_BUFFER_BIT); 
}


double CTextureMachine::GetNumber(int i, vector<string> tok) {
  double n=0.0;
  if (Size==-1)
    throw string("Error: cannot call function " + tok[0] + " before Intialize");

  if (i>=(int)tok.size())
    throw string("Error in CTextureMachine::Parse - cannot find parameter " + CUtil::toString(i) + " when calling " + tok[0]);
  try {
    n = strtod(tok.at(i).c_str(),0);
  } catch (...) {
    throw string("Error in CTextureMachine::Parse - out of bonds '" + tok[i] + "' when calling " + tok[0]);
  }
  return n;
}

int CTextureMachine::GetHF(int i, vector<string> tok) {
  int n = (int)GetNumber(i,tok);
 if (n>=MaxHF)
   throw string("Error in CTextureMachine::Parse - heightmap " + CUtil::toString(n) + " is out of bonds when calling " + tok[0]);
  return n;
  
}

void CTextureMachine::Parse(string s,bool verbatim) {
  vector<string> tok;
  CUtil::Tokenize(s, tok," ");
  bool ok=false;

  if (tok[0] == "Initialize") {
    Size = 0;
    MaxHF = GetNumber(1,tok);
    Size = GetNumber(2,tok);
    HeightFields = new C2DField[MaxHF];
    for (int i=0;i<MaxHF;i++)
      HeightFields[i].Create(Size, Size);
    
    BitMap.Create(Size, Size);
    for (unsigned int i=0;i<BitMap.width*BitMap.height*3;i++)
      BitMap.data[i] = 0;
    ok=true;
    if (verbatim)
      cout << "Creating texture ...";
  }

  if (tok[0] == "GenerateDiamondMap") {
    if (verbatim) cout << "[Diamond] ";
    HeightFields[GetHF(1, tok)].Diamond();
    ok=true;
  }

  if (tok[0] == "RGBNormalize") {
    if (verbatim) cout << "[RGBNormalize] ";
    HeightFields[GetHF(1,tok)].RGBScale(GetNumber(2,tok));
    ok=true;
  }

  if (tok[0] == "Normalize") {
    if (verbatim) cout << "[Normalize] ";
    HeightFields[GetHF(1,tok)].Normalize(GetNumber(2,tok));
    ok=true;
  }

  if (tok[0] == "Merge") {
    if (verbatim) cout << "[Merge] ";
    HeightFields[GetHF(1,tok)].Merge(HeightFields[GetHF(2,tok)], GetNumber(3,tok),GetNumber(4,tok));
    ok=true;
  }

  if (tok[0] == "ToBitMap") {
    if (verbatim) cout << "[ToBitMap] ";
    HeightFields[GetHF(1,tok)].ToBitMap(BitMap, GetNumber(2,tok),GetNumber(3,tok),GetNumber(4,tok), GetNumber(5,tok));
    ok=true;
  }

  if (tok[0] == "FromBitMap") {
    if (verbatim) cout << "[ToBitMap] ";
    HeightFields[GetHF(1,tok)].FromBitMap(BitMap);
    ok=true;
  }

  if (tok[0] == "CreateNormalMap") {
    if (verbatim) cout << "[CreateNormalMap] ";
    HeightFields[GetHF(1,tok)].CreateNormalMap(GetNumber(2,tok), &BitMap);
    ok=true;
  }

  if (tok[0] == "RenderLeaves") {
    if (verbatim) cout << "[RenderLeaves] ";
    bool randomorientation = true;
    if (GetNumber(6,tok)==0)
      randomorientation = false;
    RenderLeaves(GetNumber(1,tok),tok[2], tok[3],GetNumber(4,tok),GetNumber(5,tok), randomorientation);

    ok=true;
  }

  if (tok[0] == "ToBitMapForce") {
    if (verbatim) cout << "[ToBitMap] ";
    HeightFields[GetHF(1,tok)].ToBitMapForce(BitMap, GetNumber(2,tok),GetNumber(3,tok),GetNumber(4,tok), GetNumber(5,tok));
    ok=true;
  }
  if (tok[0] == "ClearBitMap") {
    if (verbatim) cout << "[ClearBitMap] ";
    for (unsigned int i=0;i<BitMap.width*BitMap.height*3;i++)
      BitMap.data[i] = (unsigned int)0;
     ok=true;
  }

  if (tok[0] == "CreateTexture") {
    if (verbatim) cout << "[Creating Texture]";
    if (!CTexturePointer)
      throw string("No CTexturePointer set when trying to use 'CreateTexture'");

    COpenGLTexture txt;
    CTexturePointer->loadtexture(&BitMap, &txt);
    cout << txt.id << endl;
    CTexturePointer->t.push_back(txt);
    CTexturePointer->names.push_back(tok[1]);
    if (verbatim) cout <<" - Texture added: '"+tok[1] +"'" << txt.id << endl;
    ok=true;
  }

  if (tok[0] == "CreateTransparentTexture") {
    if (verbatim) cout << "[Creating Transparent Texture]";
    if (!CTexturePointer)
      throw string("No CTexturePointer set when trying to use 'CreateTransparentTexture'");
    COpenGLTexture txt;
    //for (int i=0;i<BitMap.width*BitMap.height*3;i++)
    //  BitMap.data[i] = 0;

    CTexturePointer->loadtexture_transparent(&BitMap, &txt,GetNumber(2,tok));
    CTexturePointer->t.push_back(txt);
    CTexturePointer->names.push_back(tok[1]);
    if (verbatim) cout <<" - Texture added: '"+tok[1] +"'" << endl;
    ok=true;
  }


  if (tok[0] == "Exponential") {
    if (verbatim) cout << "[Exponential] ";
    HeightFields[GetHF(1,tok)].ExpVal(10.0,GetNumber(2,tok),GetNumber(3,tok));
    ok=true;
  }

  if (tok[0] == "Smooth") {
    if (verbatim) cout << "[Smooth] ";
    HeightFields[GetHF(1,tok)].Smooth(GetNumber(2,tok));
    ok=true;
  }

  if (tok[0] == "SmoothY") {
    if (verbatim) cout << "[SmoothY] ";
    HeightFields[GetHF(1,tok)].SmoothY(GetNumber(2,tok));
    ok=true;
  }

  if (tok[0] == "SmoothX") {
    if (verbatim) cout << "[SmoothX] ";
    HeightFields[GetHF(1,tok)].SmoothX(GetNumber(2,tok));
    ok=true;
  }

  if (tok[0] == "MakeSeamless") {
    if (verbatim) cout << "[MakeSeamless] ";
    HeightFields[GetHF(1,tok)].MakeSeamless(GetNumber(2,tok));
    ok=true;
  }

  if (tok[0] == "Circle") {
    if (verbatim) cout << "[Circle] ";
    HeightFields[GetHF(1,tok)].Circle((int)GetNumber(2,tok),GetNumber(3,tok),GetNumber(4,tok));
    ok=true;
  }



  if (tok[0] == "Bump") {
    if (verbatim) cout << "[Bump] ";
    HeightFields[GetHF(1,tok)].Bump(CVector(GetNumber(2,tok),GetNumber(3,tok),GetNumber(4,tok)),GetNumber(5,tok), GetNumber(6,tok), GetNumber(7,tok));

    ok=true;
  }

  if (tok[0] == "Erode") {
    if (verbatim) cout << "[Erode] ";
    HeightFields[GetHF(1,tok)].Erode(GetNumber(2,tok));

    ok=true;
  }
  if (tok[0] == "Scale") {
    if (verbatim) cout << "[Scale] ";
    HeightFields[GetHF(1,tok)].Scale(GetNumber(2,tok));

    ok=true;
  }

  if (tok[0] == "Water") {
    if (verbatim) cout << "[Water] ";
    HeightFields[GetHF(1,tok)].Water(GetNumber(2,tok),GetNumber(3,tok));

    ok=true;
  }


  if (tok[0] == "SaveBMP") {
    if (verbatim) cout << "[SaveBMP] ";
    HeightFields[GetHF(1,tok)].SaveBMP(tok[2]);

    ok=true;
  }

  if (tok[0] == "SaveBitMapBMP") {
    if (verbatim) cout << "[SaveBitMapBMP] ";
    BitMap.SaveBMP(tok[2]);

    ok=true;
  }


  /*  if (tok[0] =="GenerateVoronoiDiagram") {
    if (verbatim) cout << "[Voronoi Diagram] ";
    bool hide= true;
    if ((int)GetNumber(3,tok)==0) hide=false;
    HeightFields[GetHF(1,tok)].Voronoi(GetNumber(2,tok),hide, GetNumber(4,tok),GetNumber(5,tok) ,GetNumber(6,tok), GetNumber(7,tok));
    ok = true;
  }
  */
  if (tok[0][0]!='#')
  if (!ok) throw string("Error in CTextureMachine::Parse : Unknown command '" + tok[0] +"'");

}
