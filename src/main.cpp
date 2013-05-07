#include <MDOpenGL.h>
#include <iostream>
#include <string>
#include <GLUT/glut.h>
#include <Camera.h>       // Include our Camera header so we can work with Camera objects
#include <FpsManager.hpp> // Include our FpsManager class
#include <Vec3.hpp>       // Include our Vec3 class
#include <mts0_io.h>
#include <MDShaders.h>
#include <CIniFile.h>

// Specify default namespace for commonly used elements
using std::string;
using std::cout;
using std::endl;
 
double colors[7][3] = {{1,1,1},{0,0,1},{230.0/255,230.0/255,0},{1,1,1},{1,0,0},{9.0/255,92.0/255,0},{12.0/255,240.0/255}};
double atom_radii[7] = {0, 1.11, 0.66, 0.35, 0.66, 1.86, 1.02};

bool stop_loading = true;
int draw_mode = 1;

MDSphereShader sphere_shader;

// Function to draw a grid of lines
void draw_box(float length)
{
    glColor3ub(255, 255, 255);
 
    // Draw our ground grid
    glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(length,0,0);

    glVertex3f(length,0,0);
    glVertex3f(length,length,0);

    glVertex3f(length,length,0);
    glVertex3f(0,length,0);

    glVertex3f(0,length,0);
    glVertex3f(0,0,0);

    glVertex3f(0,0,0);
    glVertex3f(0,0,length);

    glVertex3f(0,0,length);
    glVertex3f(0,length,length);

    glVertex3f(0,length,length);
    glVertex3f(0,length,0);

    glVertex3f(0,length,length);
    glVertex3f(length,length,length);

    glVertex3f(length,length,length);
    glVertex3f(length,0,length);

    glVertex3f(length,0,length);
    glVertex3f(0,0,length);

    glVertex3f(length,length,length);
    glVertex3f(length,length,0);

    glVertex3f(length,0,length);
    glVertex3f(length,0,0);

    glEnd();
}

void draw_points(Mts0_io *mts0_io, Timestep *timestep) {
    vector<vector<double> > &positions = timestep->positions;
    vector<int> &atom_types = timestep->atom_types;
    
    glBegin(GL_POINTS);
    for(int n=0;n<positions.size(); n++) {
        int atom_type = atom_types[n];
        glColor4f(colors[atom_type][0],colors[atom_type][1],colors[atom_type][2],1.0);
        glVertex3f(positions[n][0],positions[n][1],positions[n][2]);
    }
    glEnd();
}

void draw_spheres(Mts0_io *mts0_io, MDOpenGL &mdopengl, Timestep *timestep) {
    vector<vector<double> > &positions = timestep->positions;
    vector<int> &atom_types = timestep->atom_types;
    double cam_x = mdopengl.cam->getXPos();
    double cam_y = mdopengl.cam->getYPos();
    double cam_z = mdopengl.cam->getZPos();

    sphere_shader.Start();
    double one_over_1500 = 1.0/1500;
    for(int n=0;n<positions.size(); n++) {
        int atom_type = atom_types[n];
        double dx = positions[n][0]-cam_x;
        double dy = positions[n][1]-cam_y;
        double dz = positions[n][2]-cam_z;
        double dr2 = dx*dx + dy*dy + dz*dz;
        double dr2_times_one_over_1500 = dr2*one_over_1500;
        if(dr2<1500) {
            double size_factor = 1.0 - dr2_times_one_over_1500;
            glPushMatrix();
            glTranslatef(positions[n][0],positions[n][1],positions[n][2]);
            sphere_shader.set_light_pos(-dx,-dy,-dz);
            sphere_shader.set_color(colors[atom_type][0],colors[atom_type][1],colors[atom_type][2]);
            int quality = max( (10 - 10*dr2_times_one_over_1500),3.0);
            glutSolidSphere(size_factor*0.3*atom_radii[atom_type],quality,quality);
            glPopMatrix();
        }
    }
    sphere_shader.End();

    for(int n=0;n<positions.size(); n++) {
        int atom_type = atom_types[n];
        double dx = positions[n][0]-cam_x;
        double dy = positions[n][1]-cam_y;
        double dz = positions[n][2]-cam_z;
        double dr2 = dx*dx + dy*dy + dz*dz;
        if(dr2>=1500) {
            glBegin(GL_POINTS);
            glColor4f(colors[atom_type][0],colors[atom_type][1],colors[atom_type][2],1.0);
            glVertex3f(positions[n][0],positions[n][1],positions[n][2]);
            glEnd();
        }
    }
}
 
// Function to draw our scene
void drawScene(Mts0_io *mts0_io, MDOpenGL &mdopengl, Timestep *timestep)
{
    // Clear the screen and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
    // Reset the matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
 
    // Move the camera to our location in space
    glRotatef(mdopengl.cam->getXRot(), 1.0f, 0.0f, 0.0f); // Rotate our camera on the x-axis (looking up and down)
    glRotatef(mdopengl.cam->getYRot(), 0.0f, 1.0f, 0.0f); // Rotate our camera on the  y-axis (looking left and right)

    // Translate the ModelView matrix to the position of our camera - everything should now be drawn relative
    // to this position!
    glTranslatef( -mdopengl.cam->getXPos(), -mdopengl.cam->getYPos(), -mdopengl.cam->getZPos() );
    draw_box(6.327983E+01);

    if(draw_mode==1) draw_points(mts0_io,timestep);
    else if(draw_mode==2) draw_spheres(mts0_io,mdopengl,timestep);

    glBegin(GL_LINES);
    glColor4f(1.0,0.0,0.0,1.0);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(1.0,0.0,0.0);

    glColor4f(0.0,1.0,0.0,1.0);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.0,1.0,0.0);

    glColor4f(0.0,0.0,1.0,1.0);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.0,0.0,1.0);
    glEnd();
 
    // ----- Stop Drawing Stuff! ------
 
    glfwSwapBuffers(); // Swap the buffers to display the scene (so we don't have to watch it being drawn!)
}

MDOpenGL mdopengl;

// Callback function to handle mouse movements
void handle_mouse_move(int mouseX, int mouseY) {
    mdopengl.cam->handleMouseMove(mouseX, mouseY);
}

// Callback function to handle keypresses
void handle_keypress(int theKey, int theAction) {
    // If a key is pressed, toggle the relevant key-press flag
    if (theAction == GLFW_PRESS)
    {
        switch (theKey)
        {
        case 'W':
            mdopengl.cam->holdingForward = true;
            break;
        case 'S':
            mdopengl.cam->holdingBackward = true;
            break;
        case 'A':
            mdopengl.cam->holdingLeftStrafe = true;
            break;
        case 'D':
            mdopengl.cam->holdingRightStrafe = true;
            break;
        case '[':
            mdopengl.fps_manager.setTargetFps(mdopengl.fps_manager.getTargetFps() - 10);
            break;
        case ']':
            mdopengl.fps_manager.setTargetFps(mdopengl.fps_manager.getTargetFps() + 10);
            break;
        case ' ':
            stop_loading = !stop_loading;
            break;
        case '1':
            draw_mode = 1;
            break;
        case '2':
            draw_mode = 2;
            break;
        default:
            // Do nothing...
            break;
        }
    }
    else // If a key is released, toggle the relevant key-release flag
    {
        switch (theKey)
        {
        case 'W':
            mdopengl.cam->holdingForward = false;
            break;
        case 'S':
            mdopengl.cam->holdingBackward = false;
            break;
        case 'A':
            mdopengl.cam->holdingLeftStrafe = false;
            break;
        case 'D':
            mdopengl.cam->holdingRightStrafe = false;
            break;
        default:
            // Do nothing...
            break;
        }
    }
}
 
// Fire it up...
int main(int argc, char **argv)
{
    cout << "Controls: Use WSAD and the mouse to move around!" << endl;
 
    // Flag to keep our main loop running
    bool running = true;
    CIniFile ini;
    ini.load("md_visualizer.ini");
    int nx = ini.getint("nx");
    int ny = ini.getint("ny");
    int nz = ini.getint("nz");
    bool preload = ini.getbool("preload");
    int current_timestep = 0;
    int max_timestep = ini.getint("max_timestep");
    string foldername_base = ini.getstring("foldername_base");
    
    Mts0_io *mts0_io = new Mts0_io(nx,ny,nz,max_timestep, foldername_base, preload);

    mdopengl.initialize(ini.getint("screen_width"),ini.getint("screen_height"), handle_keypress, handle_mouse_move);
    GLenum err = glewInit();

    sphere_shader.Initialize("sphere_shader");
    Timestep *current_timestep_object = mts0_io->get_next_timestep();

    while (running)
    {
        if(!stop_loading) current_timestep_object = mts0_io->get_next_timestep();

        // Calculate our camera movement
        mdopengl.cam->move();
 
        // Draw our scene
        drawScene(mts0_io,mdopengl,current_timestep_object);
 
        // exit if ESC was pressed or window was closed
        running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
 
        // Call our fpsManager to limit the FPS and get the frame duration to pass to the cam->move method
        double deltaTime = mdopengl.fps_manager.enforceFPS();
    }
 
    // Clean up GLFW and exit
    glfwTerminate();
 
    return 0;
}