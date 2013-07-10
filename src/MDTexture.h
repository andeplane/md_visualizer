#include <GLFW/glfw3.h>      // Include OpenGL Framework library
#include <CBitMap.h>
#include <vector>
using std::vector;

class MDOpenGL;
class CVector;
#define MAX_NUM_ATOMS 1000000

// internal texture structure
class MDOpenGLTexture {
 public:
  string        category;
  GLuint		id;
  GLsizei		imgHeight, imgWidth;	
  GLsizei		txDimensions;		
  GLfloat		alpha;			
  bool          has_alpha;
};

class MDTexture {
private:
	GLuint		vertices_id;
	GLuint		indices_id;
	GLuint		normals_id;
	GLuint		colors_id;
	float        *vertices;
	float        *colors;
	float        *normals;
	int          *indices;
public:
	CBitMap bmp;
	GLuint texture_id;
	vector<MDOpenGLTexture> textures;
	vector<string> names;

	void create_sphere1(string name, int w);
	void create_sphere2(string name, int w);
	void load_texture(CBitMap* bmp, MDOpenGLTexture* texture, bool has_alpha);
	void render_billboards(MDOpenGL &opengl, vector<int> &atom_types, vector<vector<float> > &positions, bool draw_water, double color_cutoff, double dr2_max, vector<float> system_size, bool periodic_boundary_conditions, double water_dr2_max);
	void render_billboards2(MDOpenGL &opengl, vector<int> &atom_types, vector<vector<float> > &positions, bool draw_water, double color_cutoff, double dr2_max, vector<float> system_size, bool periodic_boundary_conditions);
	void render_billboards3(MDOpenGL &opengl, vector<int> &atom_types, vector<vector<float> > &positions, bool draw_water, double color_cutoff, double dr2_max, vector<float> system_size, bool periodic_boundary_conditions);
	void prepare_billboards3();
};