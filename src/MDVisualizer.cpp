#include <MDVisualizer.h>
#include <vector>
#include <GLUT/glut.h>
#include <mts0_io.h>
#include <COpenGL.h>
#include <math.h>
#include <string>

using std::string;

Variables MDVisualizer::var;

#define SI_TYPE 1
#define A_TYPE 2
#define H_TYPE 3
#define O_TYPE 4
#define NA_TYPE 5
#define CL_TYPE 6
#define X_TYPE 7
//                             Silicon                 Silica-O       Hydrogen Oxygen          Sodium               Chlorine
double colors[7][3] = {{1,1,1},{0,0,1},{230.0/255,230.0/255,0},{1,1,1},{1,0,0},{9.0/255,92.0/255,0},{12.0/255,240.0/255}};
double atom_radii[7] = {0, 1.11, 0.66, 0.35, 0.66, 1.86, 1.02};
const float TO_RADS = 3.141592654f / 180.0f; // The value of 1 degree in radians
GLfloat fieldOfView = 45.0f;                 // Define our field of view (i.e. how quickly foreshortening occurs)
GLfloat near        = 1.0f;                  // The near (Z Axis) point of our viewing frustrum (default 1.0f)
GLfloat far         = 1500.0f;               // The far  (Z Axis) point of our viewing frustrum (default 1500.0f)

void rotate_about_x(const double &theta, double &x, double &y, double &z) {
    double costheta = cos(theta);
    double sintheta = sin(theta);
    double y_old = y; double z_old = z;
    y = costheta*y_old - sintheta*z_old;
    z = sintheta*y_old + costheta*z_old;
}

void rotate_about_y(const double &theta, double &x, double &y, double &z) {
    double costheta = cos(theta);
    double sintheta = sin(theta);
    double x_old = x; double z_old = z;
    x = costheta*x_old + sintheta*z_old;
    z = -sintheta*x_old + costheta*z_old;
}

void rotate_about_z(const double &theta, double &x, double &y, double &z) {
    double costheta = cos(theta);
    double sintheta = sin(theta);
    double x_old = x; double y_old = y;
    x = costheta*x_old - sintheta*y_old;
    y = sintheta*x_old + costheta*y_old;
}

void MDVisualizer::passive_motion_callback(int x, int y) {
    int delta_x = x - var.last_mouse_x;
    int delta_y = y - var.last_mouse_y;

    var.last_mouse_x = x;
    var.last_mouse_y = y;

    if( delta_x == 0 && delta_y == 0 ) return;

    int windowX     = glutGet( GLUT_WINDOW_X );
    int windowY     = glutGet( GLUT_WINDOW_Y );
    int screenWidth     = glutGet( GLUT_SCREEN_WIDTH );
    int screenHeight    = glutGet( GLUT_SCREEN_HEIGHT );

    int screenLeft = -windowX;
    int screenTop = -windowY;
    int screenRight = screenWidth - windowX;
    int screenBottom = screenHeight - windowY;

    if( x <= screenLeft+10 || (y) <= screenTop+10 || x >= screenRight-10 || y >= screenBottom - 10) {
        var.last_mouse_x = var.window_size[0]/2;
        var.last_mouse_y = var.window_size[1]/2;
        glutWarpPointer( var.last_mouse_y, var.last_mouse_y );
        //  If on Mac OS X, the following will also work (and CGwarpMouseCursorPosition seems faster than glutWarpPointer).
        //  CGPoint centerPos = CGPointMake( windowX + lastX, windowY + lastY );
        //  CGWarpMouseCursorPosition( centerPos );
        // Have to re-hide if the user touched any UI element with the invisible pointer, like the Dock.
        //  CGDisplayHideCursor(kCGDirectMainDisplay);
    }

    var.current_mouse_x += delta_x;
    var.current_mouse_y -= delta_y;
    rotate_about_z(-var.phi,var.camera_target[0],var.camera_target[1],var.camera_target[2]);
    rotate_about_y(-var.theta,var.camera_target[0],var.camera_target[1],var.camera_target[2]);
    var.phi = 0.01*var.current_mouse_y;
    var.theta = 0.01*var.current_mouse_x;
    rotate_about_y(var.theta,var.camera_target[0],var.camera_target[1],var.camera_target[2]);
    rotate_about_z(var.phi,var.camera_target[0],var.camera_target[1],var.camera_target[2]);
    cout << "Camera at " << ogl.camera.x << " " << ogl.camera.y << " " << ogl.camera.z << endl;
    cout << "Target at " << ogl.target.x << " " << ogl.target.y << " " << ogl.target.z << endl;
}

void MDVisualizer::motion_callback(int x, int y) {
    // cout << "Motion: " << x << " " << y << endl;
}

void MDVisualizer::Update(void) {
    if(!Initialized) Initialize_();
    glutPostRedisplay();
    InternalUpdate();
    Events();
    load_next_frame();
    glutPassiveMotionFunc(&passive_motion_callback);
    // glutMotionFunc(&motion_callback);
    // cout << "Setting to " << var.window_size[0] / 2 << " " << var.window_size[1] / 2 << endl;
    
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
    ogl.setperspective(45);
    
    ogl.camera = CVector(var.system_center[0],var.system_center[1],0);
    ogl.target = CVector(var.camera_target[0],var.camera_target[1],var.camera_target[2]);
    ogl.setup_camera();
    if(var.draw_mode == draw_mode_points) draw_points();
    else if(var.draw_mode == draw_mode_spheres) draw_spheres();
    sprintf(var.window_title,"Molecular Dynamics - timestep %04d",var.frame);
    glutSetWindowTitle(var.window_title);
    
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
    var.sphere_shader.Start();
    for(int n=0;n<positions.size(); n++) {
        int atom_type = atom_types[n];
        var.sphere_shader.SetColor(colors[atom_type][0], colors[atom_type][1], colors[atom_type][2]);
        glPushMatrix();
        glTranslatef(positions[n][0],positions[n][1],positions[n][2]);
        glutSolidSphere(0.5*atom_radii[atom_type],3,3);
        glPopMatrix();
    }
    var.sphere_shader.End();
}

void MDVisualizer::Initialize_() {
    var.frame = 0;
    var.max_frame = ini.getint("max_frame");
    int nx = ini.getint("nx");
    int ny = ini.getint("ny");
    int nz = ini.getint("nz");
    var.state_loaded = false;
    var.window_title = new char[1000];
    var.filename = new char[5000];
    var.mts0_io = new Mts0_io(nx,ny,nz);
    load_next_frame();
    var.system_size = var.mts0_io->get_lx_ly_lz();
    var.system_center.resize(3);
    var.system_center[0] = var.system_size[0] / 2;
    var.system_center[1] = var.system_size[1] / 2;
    var.system_center[2] = var.system_size[2] / 2;
    var.draw_mode = draw_mode_points;
    var.window_size.resize(2);
    var.window_size[0] = ini.getint("screen_width");
    var.window_size[1] = ini.getint("screen_height");
    var.camera_target.resize(3);
    var.camera_target[0] = var.system_center[0];
    var.camera_target[1] = var.system_center[1];
    var.camera_target[2] = var.system_center[2];

    var.last_mouse_x = var.window_size[0]/2;
    var.last_mouse_y = var.window_size[1]/2;
    var.current_mouse_x = var.last_mouse_x;
    var.current_mouse_y = var.last_mouse_y;
    glutWarpPointer( var.last_mouse_y, var.last_mouse_y );

    rotate_about_y(var.theta,var.camera_target[0],var.camera_target[1],var.camera_target[2]);
    rotate_about_z(var.phi,var.camera_target[0],var.camera_target[1],var.camera_target[2]);

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
