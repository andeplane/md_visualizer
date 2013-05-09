#include <MDShaders.h>

void MDSphereShader::Initialize(string s) {

  ((CShaderParent*)this)->Initialize(s);
  
}


void MDSphereShader::Start() {
  Shader->begin();
}

void MDSphereShader::set_light_pos(const float &x, const float &y, const float &z) {
  Shader->sendUniform3f((char*)"lightpos",x, y, z);
}

void MDSphereShader::set_color(const float &r, const float &g, const float &b) {
  Shader->sendUniform4f((char*)"color",r,g,b,1.0);
}

void MDSphereShader::End() {
  Shader->disable_multitextures();
  Shader->end();
}

MDSphereShader::MDSphereShader() {
  vert = string(
  	"uniform vec3 lightpos; \n"
	"varying vec3 normal; \n"
	"varying vec3 myPos; \n"
	"void main(void) \n"
	"{ \n"
	"	normal = gl_Normal; \n"
	"	myPos = gl_Vertex.xyz; \n"
	"   gl_TexCoord[0] = gl_MultiTexCoord0;\n"
	"   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
	"}\n");

 
  frag = string(
  	"uniform vec3 lightpos; \n"
	"varying vec3 normal; \n"
	"uniform vec4 color; \n"
	"void main(void)\n"
	"{\n "
	"  float shininess = 2.0; \n"
	"  float light = pow(clamp(dot(normalize(lightpos), normal), 0.1, 1.0),shininess); \n"
	"  gl_FragColor = color*light; \n"
	"  gl_FragColor.w = 1.0;"
	"}\n");
}