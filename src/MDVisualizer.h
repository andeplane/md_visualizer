#pragma once

#include <CApplication.h>
#include <MDShaders.h>
class Mts0_io;

using namespace std;

enum draw_modes {
    draw_mode_points = 0,
    draw_mode_spheres = 1
};

class Variables  {
public:
    MDSphereShader sphere_shader;
    char *filename;
    char *window_title;
    int frame;
    int max_frame;
    bool state_loaded;
    bool draw_mode;
    Mts0_io *mts0_io;
    vector<double> system_center;
    vector<double> system_size;
    vector<int> window_size;
    vector<double> camera_target;
    double theta, phi;
    long current_mouse_x, current_mouse_y;
    long last_mouse_x, last_mouse_y;
};

class MDVisualizer : public CApplication {
public:
    static Variables var;

    static void passive_motion_callback(int x, int y);
    static void motion_callback(int x, int y);
    static void Initialize_();
    static void load_next_frame();
    static void draw_points();
    static void draw_spheres();

    // Rasterizing
    static void Display(void);

    // Update physics / OpenGL
    static void Update(void);

    // Handle events (keys, mouse)
    static void Events();

    // internal stuff
private:    

};
