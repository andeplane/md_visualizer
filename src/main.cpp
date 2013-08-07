#include <MDOpenGL.h>
#include <iostream>
#include <string>
#include <GLUT/glut.h>
#include <Camera.h>       // Include our Camera header so we can work with Camera objects
#include <FpsManager.hpp> // Include our FpsManager class
#include <Vec3.hpp>       // Include our Vec3 class
#include <mts0_io.h>
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

vector<float> system_size;
bool paused = true;
bool draw_water = true;
double dr2_max = 3500;
double water_dr2_max = 3500;
double color_cutoff = 2000;
MDTexture texture;
MDOpenGL mdopengl;
int render_mode = 1;
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
    glDrawBuffer(GL_BACK_RIGHT);
    glLoadIdentity();
 
    // Move the camera to our location in space
    glRotatef(mdopengl.camera->get_rot_x(), 1.0f, 0.0f, 0.0f); // Rotate our camera on the x-axis (looking up and down)
    glRotatef(mdopengl.camera->get_rot_y(), 0.0f, 1.0f, 0.0f); // Rotate our camera on the  y-axis (looking left and right)

    // Translate the ModelView matrix to the position of our camera - everything should now be drawn relative
    // to this position!
    glTranslatef( -mdopengl.camera->position.x, -mdopengl.camera->position.y, -mdopengl.camera->position.z );

    vector<vector<float> > &positions = timestep->positions;
    vector<int> &atom_types = timestep->atom_types;
    CVector up_on_screen = mdopengl.coord_to_ray(0,mdopengl.window_height/2.0);
    if(render_mode == 1) texture.render_billboards(mdopengl, atom_types, positions, draw_water, color_cutoff, dr2_max, system_size, periodic_boundary_conditions, water_dr2_max);
    if(render_mode == 2) texture.render_billboards2(mdopengl, atom_types, positions, draw_water, color_cutoff, dr2_max, system_size, periodic_boundary_conditions);
    if(render_mode == 3) texture.render_billboards3(mdopengl, atom_types, positions, draw_water, color_cutoff, dr2_max, system_size, periodic_boundary_conditions);

    // ----- Stop Drawing Stuff! ------ 
    glfwSwapBuffers(); // Swap the buffers to display the scene (so we don't have to watch it being drawn!)
    return;

    // Reset the matrix
    glMatrixMode(GL_MODELVIEW);
    glDrawBuffer(GL_BACK_LEFT);
    glLoadIdentity();
 
    // Move the camera to our location in space
    glRotatef(mdopengl.camera->get_rot_x(), 1.0f, 0.0f, 0.0f); // Rotate our camera on the x-axis (looking up and down)
    glRotatef(mdopengl.camera->get_rot_y(), 0.0f, 1.0f, 0.0f); // Rotate our camera on the  y-axis (looking left and right)

    // Translate the ModelView matrix to the position of our camera - everything should now be drawn relative
    // to this position!
    glTranslatef( -mdopengl.camera->position.x+100, -mdopengl.camera->position.y +20, -mdopengl.camera->position.z +20);

    if(render_mode == 1) texture.render_billboards(mdopengl, atom_types, positions, draw_water, color_cutoff, dr2_max, system_size, periodic_boundary_conditions, water_dr2_max);
    if(render_mode == 2) texture.render_billboards2(mdopengl, atom_types, positions, draw_water, color_cutoff, dr2_max, system_size, periodic_boundary_conditions);
    if(render_mode == 3) texture.render_billboards3(mdopengl, atom_types, positions, draw_water, color_cutoff, dr2_max, system_size, periodic_boundary_conditions);
}

// Callback function to handle mouse movements
void handle_mouse_move(int mouse_x, int mouse_y) {
    mdopengl.camera->handle_mouse_move(mouse_x, mouse_y);
}

// Callback function to handle keypresses
void handle_keypress(int theKey, int theAction) {
    // If a key is pressed, toggle the relevant key-press flag
    if (theAction == GLFW_PRESS)
    {
        switch (theKey)
        {
        case 'W':
            mdopengl.camera->holding_forward = true;
            break;
        case 'S':
            mdopengl.camera->holding_backward = true;
            break;
        case 'A':
            mdopengl.camera->holding_left_strafe = true;
            break;
        case 'D':
            mdopengl.camera->holding_right_strafe = true;
            break;
        case '[':
            mdopengl.fps_manager.set_target_fps(mdopengl.fps_manager.get_target_fps() - 10);
            break;
        case ']':
            mdopengl.fps_manager.set_target_fps(mdopengl.fps_manager.get_target_fps() + 10);
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
            paused = !paused;
            break;
        case 'R':
            draw_water = !draw_water;
            break;
        case '1':
            render_mode = 1;
            break;
        case '2':
            render_mode = 2;
            break;
        case '3':
            render_mode = 3;
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
            mdopengl.camera->holding_forward = false;
            break;
        case 'S':
            mdopengl.camera->holding_backward = false;
            break;
        case 'A':
            mdopengl.camera->holding_left_strafe = false;
            break;
        case 'D':
            mdopengl.camera->holding_right_strafe = false;
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
    char *window_title = new char[1000];
    CIniFile ini;
    Timestep *current_timestep_object;

    ini.load("md_visualizer.ini");
    double camera_speed = ini.getdouble("speed");
    int nx = ini.getint("nx");
    int ny = ini.getint("ny");
    int nz = ini.getint("nz");
    bool preload = ini.getbool("preload");
    int max_timestep = ini.getint("max_timestep");
    string foldername_base = ini.getstring("foldername_base");
    dr2_max = ini.getdouble("dr2_max");
    water_dr2_max = ini.getdouble("water_dr2_max");
    color_cutoff = ini.getdouble("color_cutoff");
    double dt = ini.getdouble("dt");
    periodic_boundary_conditions = ini.getbool("periodic_boundary_conditions");
    step = ini.getint("step");
    bool full_screen = ini.getbool("full_screen");

    mts0_io = new Mts0_io(nx,ny,nz,max_timestep, foldername_base, preload, step);
    sprintf(window_title, "Molecular Dynamics Visualizer (MDV) - [%.2f fps] - t = %.2f ps (timestep %d - step: %d)",60.0, 0.0, mts0_io->current_timestep, mts0_io->step);

    mdopengl.initialize(ini.getint("screen_width"),ini.getint("screen_height"), string(window_title), handle_keypress, handle_mouse_move, full_screen, camera_speed);
    GLenum error = glewInit();

    current_timestep_object = mts0_io->get_next_timestep(time_direction);
    system_size = current_timestep_object->get_lx_ly_lz();

    texture.create_sphere1("sphere1", 1000);
    texture.create_sphere2("sphere2", 1000);
    texture.prepare_billboards3();
    
    bool running = true;
    while (running)
    {
        if(!paused) current_timestep_object = mts0_io->get_next_timestep(time_direction);

        // Calculate our camera movement
        mdopengl.camera->move(system_size, periodic_boundary_conditions);
 
        // Draw our scene
        drawScene(mts0_io,mdopengl,current_timestep_object);
 
        // exit if ESC was pressed or window was closed
        running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
 
        // Call our fps manager to limit the FPS
        mdopengl.fps_manager.enforce_fps();
        double fps = mdopengl.fps_manager.average_fps;

        // Calculate the current time in pico seconds to show in the title bar
        double t_in_ps = mts0_io->current_timestep*dt/1000;
        sprintf(window_title, "Molecular Dynamics Visualizer (MDV) - [%.2f fps] - t = %.2f ps (timestep %d - step: %d)",fps, t_in_ps, mts0_io->current_timestep, mts0_io->step);
        mdopengl.set_window_title(string(window_title));
    }
 
    // Clean up GLFW and exit
    glfwTerminate();
 
    return 0;
}