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
#include <time.h>
#include <CBitMap.h>
#include <MDTexture.h>

#define SI_TYPE 1
#define A_TYPE 2
#define H_TYPE 3
#define O_TYPE 4
#define NA_TYPE 5
#define CL_TYPE 6
#define X_TYPE 7

#define NA_MASS 22.989769
#define CL_MASS 35.453

// Specify default namespace for commonly used elements
using std::string;
using std::cout;
using std::endl;
                    // Not Av   Si                      Si-O       H            O      Na                   Cl
double colors[7][3] = {{1,1,1},{230.0/255,230.0/255,0},{0,0,1},{1.0,1.0,1.0},{1,0,0},{9.0/255,92.0/255,0},{95.0/255,216.0/255,250.0/255}};
double visual_atom_radii[7] = {0, 1.11, 0.66, 0.35, 0.66, 1.86, 1.02};
double hit_atom_radii[7] = {0, 1.11, 0.66, 0.35, 0.66, 2*1.86, 2*1.02};

vector<float> system_size;
bool stop_loading = true;
bool draw_water = true;
double dr2_max = 3500;
double color_cutoff = 2000;
MDTexture texture;
MDOpenGL mdopengl;
bool periodic_boundary_conditions = false;
int step = 1;
int time_direction = 1; //-1 to run time backwards
Mts0_io *mts0_io;

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

    vector<vector<float> > &positions = timestep->positions;
    vector<int> &atom_types = timestep->atom_types;
    texture.render_billboards(mdopengl, atom_types, positions, draw_water, color_cutoff, dr2_max, system_size, periodic_boundary_conditions);

    // ----- Stop Drawing Stuff! ------ 
    glfwSwapBuffers(); // Swap the buffers to display the scene (so we don't have to watch it being drawn!)
}

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
        case 'T':
            time_direction *= -1;
            break;
        case 'P':
            mts0_io->step++;
            break;
        case 'M':
            mts0_io->step--;
            break;
        case 'B':
            periodic_boundary_conditions = !periodic_boundary_conditions;
            break;
        case ' ':
            stop_loading = !stop_loading;
            break;
        case 'R':
            draw_water = !draw_water;
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
    dr2_max = ini.getdouble("dr2_max");
    color_cutoff = ini.getdouble("color_cutoff");
    double dt = ini.getdouble("dt");
    periodic_boundary_conditions = ini.getbool("periodic_boundary_conditions");
    step = ini.getint("step");
    bool remove_water = ini.getbool("remove_water");

    mts0_io = new Mts0_io(nx,ny,nz,max_timestep, foldername_base, preload, remove_water, step);
    
    mdopengl.initialize(ini.getint("screen_width"),ini.getint("screen_height"), handle_keypress, handle_mouse_move);
    GLenum err = glewInit();

    Timestep *current_timestep_object = mts0_io->get_next_timestep(time_direction);
    system_size = current_timestep_object->get_lx_ly_lz();

    vector<int> number_of_atoms_of_each_type = current_timestep_object->get_number_of_atoms_of_each_type();
    char *title = new char[1000];
    texture.create_sphere("sphere", 1000);
    
    while (running)
    {
        if(!stop_loading) current_timestep_object = mts0_io->get_next_timestep(time_direction);

        // Calculate our camera movement
        mdopengl.cam->move(system_size, periodic_boundary_conditions);
 
        // Draw our scene
        drawScene(mts0_io,mdopengl,current_timestep_object);
 
        // exit if ESC was pressed or window was closed
        running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
 
        // Call our fpsManager to limit the FPS and get the frame duration to pass to the cam->move method
        double deltaTime = mdopengl.fps_manager.enforceFPS();
        double fps = mdopengl.fps_manager.average_fps;
        // cout << "FPS: " << fps << endl;
        double t_in_ps = mts0_io->current_timestep*dt/1000;

        sprintf(title, "Molecular Dynamics Visualizer (MDV) - [%.2f fps] - t = %.2f ps (timestep %d - step: %d)",fps, t_in_ps, mts0_io->current_timestep, mts0_io->step);
        mdopengl.set_window_title(string(title));
    }
 
    // Clean up GLFW and exit
    glfwTerminate();
 
    return 0;
}