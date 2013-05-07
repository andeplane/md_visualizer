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
 
double colors[7][3] = {{1,1,1},{0,0,1},{230.0/255,230.0/255,0},{1,1,1},{1,0,0},{9.0/255,92.0/255,0},{12.0/255,240.0/255}};
// double atom_radii[7] = {0, 1.11, 0.66, 0.35, 0.66, 1.86, 1.02};
double atom_radii[7] = {0, 1.11, 0.66, 0.35, 0.66, 2*1.86, 2*1.02};

bool stop_loading = true;
int draw_mode = 2;

MDSphereShader sphere_shader;

// Data
// GLuint geometry_array = 0;
// GLuint indice_array = 0;

// int num_geometry = 25000000;
// int num_indices = 25000000;

// GLfloat *geometry = new GLfloat[8*num_geometry];
// GLuint *indices = new GLuint[num_indices];

void prepare() {
    // for(int i=0;i<num_geometry;i++) {
    //     geometry[8*i+0] = 100*(double)rand()/RAND_MAX;
    //     geometry[8*i+1] = 100*(double)rand()/RAND_MAX;
    //     geometry[8*i+2] = 100*(double)rand()/RAND_MAX;
    //     geometry[8*i+3] = (double)rand()/RAND_MAX;
    //     geometry[8*i+4] = (double)rand()/RAND_MAX;
    //     geometry[8*i+5] = (double)rand()/RAND_MAX;
    //     geometry[8*i+6] = 0;
    //     geometry[8*i+7] = 0;
        
    //     indices[i] = i;
    // }

    // glGenBuffers(1, &geometry_array);
    // glBindBuffer(GL_ARRAY_BUFFER, geometry_array);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*8*num_geometry, NULL, GL_DYNAMIC_DRAW);
    // glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*8*num_geometry, geometry);

    // glGenBuffers(1, &indice_array);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indice_array);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*num_indices, NULL, GL_STATIC_DRAW);
    // glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLuint)*num_indices, indices);
}

void draw_points2(Mts0_io *mts0_io, Timestep *timestep) {
    //Render
    // Step 1
    // glBindBuffer(GL_ARRAY_BUFFER, geometry_array);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indice_array);

    // // Step 2
    // glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    // glEnableClientState(GL_NORMAL_ARRAY);
    // glEnableClientState(GL_VERTEX_ARRAY);

    // // Step 3
    // glTexCoordPointer(3, GL_FLOAT, sizeof(GLfloat)*8, (float*)(sizeof(GLfloat)*5));
    // glNormalPointer(GL_FLOAT, sizeof(GLfloat)*8, (float*)(sizeof(GLfloat)*3));
    // glVertexPointer(3, GL_FLOAT, sizeof(GLfloat)*8, NULL);

    // // Step 4
    // glDrawElements(GL_POINTS, num_indices, GL_UNSIGNED_INT, NULL);

    // // Step 5
    // glDisableClientState(GL_VERTEX_ARRAY);
    // glDisableClientState(GL_NORMAL_ARRAY);
    // glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    
}

void draw_points(Mts0_io *mts0_io, Timestep *timestep) {
    vector<vector<float> > &positions = timestep->positions;
    vector<int> &atom_types = timestep->atom_types;
    
    glBegin(GL_POINTS);
    for(int n=0;n<positions.size(); n++) {
        int atom_type = atom_types[n];
        if(atom_type == H_TYPE || atom_type == O_TYPE) continue;
        glColor4f(colors[atom_type][0],colors[atom_type][1],colors[atom_type][2],1.0);
        glVertex3f(positions[n][0],positions[n][1],positions[n][2]);
    }
    glEnd();
}

bool game_mode = false;
vector<int> atom_ids_taken;
int number_of_na;
int number_of_cl;
int caught_na_ions = 0;
int caught_cl_ions = 0;
double t0 = time(NULL);
double dr2_max = 3500;

void restart() {
    atom_ids_taken.clear();
    caught_na_ions = 0;
    caught_cl_ions = 0;
    t0 = time(NULL);
}

void draw_spheres(Mts0_io *mts0_io, MDOpenGL &mdopengl, Timestep *timestep) {
    vector<vector<float> > &positions = timestep->positions;
    vector<int> &atom_types = timestep->atom_types;
    vector<int> &atom_ids = timestep->atom_ids;
    double cam_x = mdopengl.cam->getXPos();
    double cam_y = mdopengl.cam->getYPos();
    double cam_z = mdopengl.cam->getZPos();

    sphere_shader.Start();
    double one_over_dr2_max = 1.0/dr2_max;
    double color_min = 100000;
    for(int n=0;n<positions.size(); n++) {
        int atom_type = atom_types[n];
        if(atom_type == H_TYPE || atom_type == O_TYPE) continue;

        int atom_id = atom_ids[n];
        double dx = positions[n][0]-cam_x;
        double dy = positions[n][1]-cam_y;
        double dz = positions[n][2]-cam_z;
        double dr2 = dx*dx + dy*dy + dz*dz;
        double dr2_times_one_over_dr2_max = dr2*one_over_dr2_max;
        double color_factor = max( (1-dr2/2000),0.2);
        if(color_min>color_factor) color_min = color_factor;

        if(dr2<dr2_max) {
            // double size_factor = 1.0 - dr2_times_one_over_dr2_max;
            double size_factor = 0.5;
            bool did_take_before = false;
            if(game_mode) {
                for(int m=0;m<atom_ids_taken.size(); m++) {
                    if(atom_ids_taken[m] == atom_id) did_take_before = true;
                }

                if(!did_take_before && dr2 < size_factor*atom_radii[atom_type]) {
                    if(atom_type == NA_TYPE) {
                        caught_na_ions++;
                        atom_ids_taken.push_back(atom_id);
                        cout << "You collected a sodium ion. There are " << (number_of_na - caught_na_ions) << " left. ";
                        std::cout << "\007" << endl;
                    } else if(atom_type == CL_TYPE) {
                        caught_cl_ions++;
                        atom_ids_taken.push_back(atom_id);
                        cout << "You collected a chlorine ion. There are " << (number_of_cl - caught_cl_ions) << " left. ";
                        std::cout << "\007" << endl;
                    } else if(atom_type == H_TYPE || atom_type == O_TYPE) {
                        cout << "\nYou drowned on a water molecule ..." << endl;
                        cout << "You only collected " << 100.0*(caught_cl_ions+caught_na_ions)/(number_of_cl+number_of_na) << "% of the ions. Now you'll never be the man your mother is." << endl;
                        std::cout << "\007" << endl;
                        restart();
                    } else {
                        cout << "\nYou broke a glass ..." << endl;
                        cout << "You only collected " << 100.0*(caught_cl_ions+caught_na_ions)/(number_of_cl+number_of_na) << "% of the ions. Ordinarily people live and learn. You just live." << endl;
                        std::cout << "\007" << endl;
                        restart();
                    }

                    if(number_of_na == caught_na_ions && number_of_cl == caught_cl_ions) {
                        double total_time = time(NULL) - t0;
                        double na_mass = NA_MASS*1.660538921e-21;
                        double cl_mass = CL_MASS*1.660538921e-21;
                        cout << "\nYou collected " << caught_na_ions*na_mass + caught_cl_ions*cl_mass << " mg of table salt in " << total_time << " seconds. I don't know what your problem is, but I'll bet it's hard to pronounce." << endl;
                        restart();
                    }
                }
            }
            
            if(game_mode && did_take_before) continue;

            glPushMatrix();
            glTranslatef(positions[n][0],positions[n][1],positions[n][2]);
            sphere_shader.set_light_pos(-dx,-dy,-dz);
            sphere_shader.set_color(color_factor*colors[atom_type][0],color_factor*colors[atom_type][1],color_factor*colors[atom_type][2]);
            int quality = max( (10 - 10*dr2_times_one_over_dr2_max),1.0);
            glutSolidSphere(size_factor*atom_radii[atom_type],quality,quality);
            glPopMatrix();
        }
    }
    sphere_shader.End();

    // for(int n=0;n<positions.size(); n++) {
    //     int atom_type = atom_types[n];
    //     if(atom_type == H_TYPE || atom_type == O_TYPE) continue;
    //     double dx = positions[n][0]-cam_x;
    //     double dy = positions[n][1]-cam_y;
    //     double dz = positions[n][2]-cam_z;
    //     double dr2 = dx*dx + dy*dy + dz*dz;
    //     if(dr2>=dr2_max) {
    //         glBegin(GL_POINTS);
    //         glColor4f(colors[atom_type][0],colors[atom_type][1],colors[atom_type][2],1.0);
    //         glVertex3f(positions[n][0],positions[n][1],positions[n][2]);
    //         glEnd();
    //     }
    // }
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

    if(draw_mode==1) draw_points(mts0_io,timestep);
    else if(draw_mode==2) draw_spheres(mts0_io,mdopengl,timestep);
 
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
        case 'G':
            game_mode = !game_mode;
            cout << "Cheating on me already? Game mode is " << ((game_mode) ? "on." : "off.") << endl;
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
    game_mode = ini.getbool("game_mode");
    string foldername_base = ini.getstring("foldername_base");
    dr2_max = ini.getdouble("dr2_max");
    
    Mts0_io *mts0_io = new Mts0_io(nx,ny,nz,max_timestep, foldername_base, preload);

    mdopengl.initialize(ini.getint("screen_width"),ini.getint("screen_height"), handle_keypress, handle_mouse_move);
    GLenum err = glewInit();

    sphere_shader.Initialize("sphere_shader");
    Timestep *current_timestep_object = mts0_io->get_next_timestep();
    vector<int> number_of_atoms_of_each_type = current_timestep_object->get_number_of_atoms_of_each_type();
    
    number_of_na = number_of_atoms_of_each_type[NA_TYPE];
    number_of_cl = number_of_atoms_of_each_type[CL_TYPE];
    cout << "There are " << number_of_na << " na ions and " << number_of_cl << " cl ions. Gotta catch'em all!" << endl;

    prepare();

    while (running)
    {
        if(!stop_loading) current_timestep_object = mts0_io->get_next_timestep();

        // Calculate our camera movement
        mdopengl.cam->move();
 
        mdopengl.set_window_title(string("Molecular Dynamics Visualizer (MDV) - timestep 0"));
        // Draw our scene
        drawScene(mts0_io,mdopengl,current_timestep_object);
 
        // exit if ESC was pressed or window was closed
        running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
 
        // Call our fpsManager to limit the FPS and get the frame duration to pass to the cam->move method
        double deltaTime = mdopengl.fps_manager.enforceFPS();
        // cout << mdopengl.fps_manager.average_fps << endl;
    }
 
    // Clean up GLFW and exit
    glfwTerminate();
 
    return 0;
}