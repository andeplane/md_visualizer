#pragma once
#include <GL/glew.h>
#include <GL/glfw.h>      // Include OpenGL Framework library
#include <string>
#include <FpsManager.hpp> // Include our FpsManager class

using std::string;

class FpsManager;
class Camera;
class CVector;


class MDOpenGL {
public:   
    static const int MIPMAP = 0;
    static const int NOMIPMAP = 1;
    static const int MIPMAPALPHA = 2;

    GLint window_width, window_height; 
    GLint mid_window_x, mid_window_y;
    double aspect_ratio;
    bool full_screen;
    
    Camera *camera;
    string window_title;

    // Create a FPS manager that locks to 60fps and updates the window title with stats every 3 seconds
    FpsManager fps_manager;
    GLfloat perspective;
    GLfloat field_of_view;
    GLfloat near;
    GLfloat far;

    void initialize(int w, int h, string window_title_, GLFWkeyfun cbfun, GLFWmouseposfun, bool full_screen, double camera_speed);
    void pop();
    void push();
    void init_GL();
    void set_window_title(string title);
    CVector coord_to_ray(double px, double py);
    MDOpenGL() { }
}; 
