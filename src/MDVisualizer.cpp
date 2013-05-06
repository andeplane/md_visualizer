#include <MDVisualizer.h>
#include <vector>
#include <GLUT/glut.h>
#include <mts0_io.h>
#include <COpenGL.h>

Variables MDVisualizer::var;

#define SI_TYPE 1
#define A_TYPE 2
#define H_TYPE 3
#define O_TYPE 4
#define NA_TYPE 5
#define CL_TYPE 6
#define X_TYPE 7
//                             Silicon                 Silica-O       Hydrogen Oxygen          Sodium               Chlorine
double colors[7][3] = {{1,1,1},{110.0/255,127.0/255,1},{0,0,107.0/255},{1,1,1},{100.0/255,1,0},{9.0/255,92.0/255,0},{12.0/255,240.0/255}};
double atom_radii[7] = {0, 1.11, 0.66, 0.35, 0.66, 1.86, 1.02};

void MDVisualizer::Update(void) {
    if(!Initialized) Initialize_();
    glutPostRedisplay();
    InternalUpdate();
    Events();
    load_next_frame();
}

void MDVisualizer::load_next_frame() {
    sprintf(var.filename,"%s/%06d/mts0/",ini.getstring("folder_name_base").c_str(), var.frame++);
    if(var.frame>var.max_frame) var.frame = 0;

    var.mts0_io->load_atoms(string(var.filename));
}

void MDVisualizer::Display (void) {
    if (!Initialized)
    return;

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); 

    ogl.clip_near = 0.01;
    ogl.clip_far = 1000;
    ogl.setperspective(60);

    ogl.camera = CVector(var.system_center[0],var.system_center[1],var.system_size[2]*1.5);
    
    ogl.target = CVector(var.system_center[0],var.system_center[1],var.system_center[2]);
    ogl.setup_camera();
    if(var.draw_mode == draw_mode_points) draw_points();
    else if(var.draw_mode == draw_mode_spheres) draw_spheres();

    glutSwapBuffers(); 
    Events();
}

void MDVisualizer::draw_points() {
    vector<vector<double> > positions = var.mts0_io->positions;
    vector<int> atom_types = var.mts0_io->atom_types;
    glBegin(GL_POINTS);
    for(int n=0;n<positions.size(); n++) {
        int atom_type = atom_types[n];
        glColor4f(colors[atom_type][0],colors[atom_type][1],colors[atom_type][2],1.0);
        glVertex3f(positions[n][0],positions[n][1],positions[n][2]);
    }
    glEnd();
}

void MDVisualizer::draw_spheres() {
    vector<vector<double> > positions = var.mts0_io->positions;
    vector<int> atom_types = var.mts0_io->atom_types;
    var.sphere_shader.lightpos.Set(10,10,10);
    var.sphere_shader.targetdir = (ogl.target-ogl.camera).Normalize();
    var.sphere_shader.Start();
    for(int n=0;n<positions.size(); n++) {
        int atom_type = atom_types[n];
        var.sphere_shader.SetColor(colors[atom_type][0], colors[atom_type][1], colors[atom_type][2]);
        glPushMatrix();
        glTranslatef(positions[n][0],positions[n][1],positions[n][2]);
        glutSolidSphere(0.5*atom_radii[atom_type],20,20);
        glPopMatrix();
    }
    var.sphere_shader.End();
}

void MDVisualizer::Initialize_() {
    var.frame = 0;
    var.max_frame = 999;
    int nx = ini.getint("nx");
    int ny = ini.getint("ny");
    int nz = ini.getint("nz");
    var.state_loaded = false;
    var.filename = new char[5000];
    var.mts0_io = new Mts0_io(nx,ny,nz);
    load_next_frame();
    var.system_size = var.mts0_io->get_lx_ly_lz();
    var.system_center.resize(3);
    var.system_center[0] = var.system_size[0] / 2;
    var.system_center[1] = var.system_size[1] / 2;
    var.system_center[2] = var.system_size[2] / 2;
    var.draw_mode = draw_mode_points;

    var.sphere_shader.Initialize("sphere_shader");
    Initialized = true;
}  

void MDVisualizer::Events ()  {
    if (key==27)
        exit(1);
    if(key=='0') var.draw_mode = 0;
    if(key=='1') var.draw_mode = 1;
    key = '<';
}
