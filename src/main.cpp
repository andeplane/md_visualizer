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
                    // Not Av   Si                      Si-O       H            O      Na                   Cl
double colors[7][3] = {{1,1,1},{230.0/255,230.0/255,0},{0,0,1},{1.0,1.0,1.0},{1,0,0},{9.0/255,92.0/255,0},{95.0/255,216.0/255,250.0/255}};
double visual_atom_radii[7] = {0, 1.11, 0.66, 0.35, 0.66, 1.86, 1.02};
double hit_atom_radii[7] = {0, 1.11, 0.66, 0.35, 0.66, 2*1.86, 2*1.02};

bool stop_loading = true;
int draw_mode = 3;
bool draw_water = true;

MDSphereShader sphere_shader;
// Data
GLuint *spheres_vertices_gpu = 0;
GLuint *indices_gpu = 0;
GLuint all_spheres_vertices_gpu = 0;
GLuint all_indices_gpu = 0;
int points_1 = 10;
int points_2 = 10;
int num_indices = 2*points_1*points_2;

bool game_mode = false;
vector<int> atom_ids_taken;
int number_of_na;
int number_of_cl;
int caught_na_ions = 0;
int caught_cl_ions = 0;
double t0 = time(NULL);
double dr2_max = 3500;
double color_cutoff = 2000;

void restart() {
    atom_ids_taken.clear();
    caught_na_ions = 0;
    caught_cl_ions = 0;
    t0 = time(NULL);
}

void prepare_spheres_for_atom_types() {
    double size_factor = 0.5;
    GLfloat *vertices = new GLfloat[6*num_indices];
    GLuint *indices = new GLuint[num_indices];
    spheres_vertices_gpu = new GLuint[7];
    indices_gpu = new GLuint[7];

    for(int atom_type = 1; atom_type < 7; atom_type++) {
        double radius = visual_atom_radii[atom_type];
        int count = 0;
        int index = 0;

        for(int i = 0; i < points_1; i++)
        {
            double theta = M_PI * (-0.5 + (double) (i - 1) / (points_1-1));
            double z0  = size_factor*radius*sin(theta);
            double zr0 =  size_factor*radius*cos(theta);

            double theta_2 = M_PI * (-0.5 + (double) i / (points_1-1));
            double z1 = size_factor*radius*sin(theta_2);
            double zr1 = size_factor*radius*cos(theta_2);

            for(int j = 0; j < points_2; j++)
            {
                double phi = 2 * M_PI * (double) (j - 1) / (points_2-1);
                double x = cos(phi);
                double y = sin(phi);

                vertices[count++] = (x * zr0); //X
                vertices[count++] = (y * zr0); //Y
                vertices[count++] = (z0);      //Z
                
                CVector normal((x * zr0), (y * zr0), (z0));
                normal = normal.Normalize();

                vertices[count++] = normal.x;
                vertices[count++] = normal.y;
                vertices[count++] = normal.z;

                indices[index] = index++;
                vertices[count++] = (x * zr1); //X
                vertices[count++] = (y * zr1); //Y
                vertices[count++] = (z1);      //Z

                normal = CVector((x * zr1), (y * zr1), (z1));
                normal = normal.Normalize();

                vertices[count++] = normal.x;
                vertices[count++] = normal.y;
                vertices[count++] = normal.z;

                indices[index] = index++;
            }
        }

        glGenBuffers(1, &spheres_vertices_gpu[atom_type]);
        glBindBuffer(GL_ARRAY_BUFFER, spheres_vertices_gpu[atom_type]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*6*num_indices, NULL, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*6*num_indices, vertices);

        glGenBuffers(1, &indices_gpu[atom_type]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_gpu[atom_type]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*num_indices, NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLuint)*num_indices, indices);
    }
}

void draw_spheres_3(MDOpenGL &mdopengl, Timestep *timestep) {
    sphere_shader.Start();

    vector<vector<float> > &positions = timestep->positions;
    vector<int> &atom_types = timestep->atom_types;

    double cam_x = mdopengl.cam->getXPos();
    double cam_y = mdopengl.cam->getYPos();
    double cam_z = mdopengl.cam->getZPos();

    for(int n=0;n<positions.size(); n++) {
        int atom_type = atom_types[n];
        if( (atom_type == H_TYPE || atom_type == O_TYPE) && !draw_water) continue;
        double dx = positions[n][0]-cam_x;
        double dy = positions[n][1]-cam_y;
        double dz = positions[n][2]-cam_z;
        double cam_target_times_dr = dx*mdopengl.cam->target[0] + dy*mdopengl.cam->target[1] + dz*mdopengl.cam->target[2];
        
        if(cam_target_times_dr < 0) continue;

        double dr2 = dx*dx + dy*dy + dz*dz;
        if(dr2>dr2_max) continue;

        double color_factor = max( (1-dr2/color_cutoff),0.2);
        
        // Step 1
        glBindBuffer(GL_ARRAY_BUFFER, spheres_vertices_gpu[atom_type]);
        // Step 2
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);

        // Step 3
        glVertexPointer(3, GL_FLOAT, 6*sizeof(GLfloat), NULL);
        glNormalPointer(GL_FLOAT, 6*sizeof(GLfloat), (float*)(3*sizeof(GLfloat)));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_gpu[1]);
        

        glPushMatrix();
        glTranslatef(positions[n][0],positions[n][1],positions[n][2]);
        sphere_shader.set_light_pos(-dx,-dy,-dz);
        sphere_shader.set_color(color_factor*colors[atom_type][0],color_factor*colors[atom_type][1],color_factor*colors[atom_type][2]);
        
        // glDrawElements(GL_POINTS, num_indices, GL_UNSIGNED_INT, NULL);
        glDrawElements(GL_TRIANGLE_STRIP, num_indices, GL_UNSIGNED_INT, NULL);
        
        glPopMatrix();
        // Step 5
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
    }
    
    sphere_shader.End();
}

void draw_spheres_2(Mts0_io *mts0_io, MDOpenGL &mdopengl, Timestep *timestep) {
    vector<vector<float> > &positions = timestep->positions;
    vector<int> &atom_types = timestep->atom_types;
    vector<int> &atom_ids = timestep->atom_ids;
    double cam_x = mdopengl.cam->getXPos();
    double cam_y = mdopengl.cam->getYPos();
    double cam_z = mdopengl.cam->getZPos();

    sphere_shader.Start();
    double one_over_dr2_max = 1.0/dr2_max;
    
    for(int n=0;n<positions.size(); n++) {
        int atom_type = atom_types[n];

        if( (atom_type == H_TYPE || atom_type == O_TYPE) && !draw_water) continue;
        
        int atom_id = atom_ids[n];
        double dx = positions[n][0]-cam_x;
        double dy = positions[n][1]-cam_y;
        double dz = positions[n][2]-cam_z;

        double dr2 = dx*dx + dy*dy + dz*dz;
        double dr2_times_one_over_dr2_max = dr2*one_over_dr2_max;
        double color_factor = max( (1-dr2/color_cutoff),0.2);
        
        if(dr2<dr2_max) {
            // double size_factor = 1.0 - dr2_times_one_over_dr2_max;
            double size_factor = 0.5;
            bool did_take_before = false;
            if(game_mode) {
                for(int m=0;m<atom_ids_taken.size(); m++) {
                    if(atom_ids_taken[m] == atom_id) did_take_before = true;
                }

                if(!did_take_before && dr2 < size_factor*hit_atom_radii[atom_type]) {
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
            if(dr2 < 50) continue;
            glPushMatrix();
            glTranslatef(positions[n][0],positions[n][1],positions[n][2]);
            sphere_shader.set_light_pos(-dx,-dy,-dz);
            sphere_shader.set_color(color_factor*colors[atom_type][0],color_factor*colors[atom_type][1],color_factor*colors[atom_type][2]);
            int quality = max( (10 - 10*dr2_times_one_over_dr2_max),1.0);
            glutSolidSphere(size_factor*visual_atom_radii[atom_type],quality,quality);
            glPopMatrix();
        }
    }
    sphere_shader.End();
}

void draw_points_1(Mts0_io *mts0_io, Timestep *timestep) {
    vector<vector<float> > &positions = timestep->positions;
    vector<int> &atom_types = timestep->atom_types;
    
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for(int n=0;n<positions.size(); n++) {
        int atom_type = atom_types[n];
        if( (atom_type == H_TYPE || atom_type == O_TYPE) && !draw_water) continue;
        
        glColor4f(colors[atom_type][0],colors[atom_type][1],colors[atom_type][2],1.0);
        glVertex3f(positions[n][0],positions[n][1],positions[n][2]);
    }
    glEnd();
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

    if(draw_mode==1) draw_points_1(mts0_io,timestep);
    else if(draw_mode==2) draw_spheres_2(mts0_io,mdopengl,timestep);
    else if(draw_mode==3) draw_spheres_3(mdopengl, timestep);
    
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
        case 'R':
            draw_water = !draw_water;
            break;
        case '1':
            draw_mode = 1;
            break;
        case '2':
            draw_mode = 2;
            break;
        case '3':
            draw_mode = 3;
            break;
        case '4':
            draw_mode = 4;
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
    color_cutoff = ini.getdouble("color_cutoff");
    double dt = ini.getdouble("dt");
    
    bool remove_water = ini.getbool("remove_water");

    Mts0_io *mts0_io = new Mts0_io(nx,ny,nz,max_timestep, foldername_base, preload, remove_water);
    
    mdopengl.initialize(ini.getint("screen_width"),ini.getint("screen_height"), handle_keypress, handle_mouse_move);
    GLenum err = glewInit();

    sphere_shader.Initialize("sphere_shader");
    Timestep *current_timestep_object = mts0_io->get_next_timestep();
    vector<int> number_of_atoms_of_each_type = current_timestep_object->get_number_of_atoms_of_each_type();
    
    number_of_na = number_of_atoms_of_each_type[NA_TYPE];
    number_of_cl = number_of_atoms_of_each_type[CL_TYPE];
    cout << "There are " << number_of_na << " na ions and " << number_of_cl << " cl ions. Gotta catch'em all!" << endl;

    prepare_spheres_for_atom_types();
    char *title = new char[1000];
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
        double fps = mdopengl.fps_manager.average_fps;
        // cout << "FPS: " << fps << endl;
        double t_in_ps = mts0_io->current_timestep*dt/1000;

        sprintf(title, "Molecular Dynamics Visualizer (MDV) - [%.2f fps] - t = %.2f ps (timestep %d)",fps, t_in_ps, mts0_io->current_timestep);
        mdopengl.set_window_title(string(title));
    }
 
    // Clean up GLFW and exit
    glfwTerminate();
 
    return 0;
}