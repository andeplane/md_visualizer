#include <MDShaders.h>

void MDSphereShader::Initialize(string s) {

  ((CShaderParent*)this)->Initialize(s);
  
}


void MDSphereShader::Start() {
  Shader->begin();

  Shader->sendUniform3f((char*)"lightpos",lightpos.x, lightpos.y, lightpos.z);

}

void MDSphereShader::SetColor(const float &r, const float &g, const float &b) {
  Shader->sendUniform4f((char*)"color",r,g,b,1.0);
}

void MDSphereShader::End() {
  Shader->disable_multitextures();
  Shader->end();
}

// MDSphereShader::MDSphereShader() {
//   vert = string(
//   	"uniform vec3 lightpos; \n"
//   	"uniform vec3 targetdir; \n"
// 	"varying vec3 normal; \n"
// 	"varying vec3 myPos; \n"
// 	"void main(void) \n"
// 	"{ \n"
// 	"	normal = gl_Normal; \n"
// 	"	myPos = gl_Vertex.xyz; \n"
// 	"   gl_TexCoord[0] = gl_MultiTexCoord0;\n"
// 	"   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
// 	"}\n");

 
//   frag = string(
//   	"uniform vec3 lightpos; \n"
//   	"uniform vec3 targetdir; \n"
// 	"varying vec3 normal; \n"
// 	"varying vec3 myPos; \n"
// 	"uniform vec4 color; \n"
// 	"void main(void)\n"
// 	"{\n "
// 	// "  vec4 val = vec4(0.2,myPos.z*10.0,1.0,1.0);"
// 	"  float light = clamp(dot(normalize(lightpos), normal), 0.0, 1.0);"
// 	"  float shininess = 40.0;"
// 	"  float specular = pow(clamp(dot(reflect(-normalize(lightpos), normal), targetdir), 0.0, 1.0), shininess);"
// 	"  gl_FragColor = color*light + specular*vec4(1,1,1,1); \n"
// 	"  gl_FragColor.w = 0.7;"
// 	"}\n");
// }

MDSphereShader::MDSphereShader() {
  vert = string(
  	"uniform vec3 lightpos; \n"
  	"uniform vec3 targetdir; \n"
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
	// "  vec4 val = vec4(0.2,myPos.z*10.0,1.0,1.0);"
	"  float light = clamp(dot(normalize(lightpos), normal), 0.2, 1.0);"
	"  gl_FragColor = color*light; \n"
	"  gl_FragColor.w = 1.0;"
	"}\n");
}