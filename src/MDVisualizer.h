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
    int frame;
    int max_frame;
    bool state_loaded;
    bool draw_mode;
    Mts0_io *mts0_io;
    vector<double> system_center;
    vector<double> system_size;
};

class MDVisualizer : public CApplication {
public:
    static Variables var;

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
