#include <MDOpenGL.h>
#include <CMath.h>
#include <CVector.h>
#include <Camera.h>       // Include our Camera header so we can work with Camera objects

// Frame counter and window settings variables
int redBits    = 8, greenBits = 8,    blueBits    = 8;
int alphaBits  = 8, depthBits = 24,   stencilBits = 0;
int stereoBits = 2;

void MDOpenGL::initialize(int w, int h, string window_title_, GLFWkeyfun handle_keypress, GLFWmouseposfun handle_mouse_move, bool full_screen, double camera_speed) {
    window_title = window_title_;

    window_width = w;
    window_height = h;
    mid_window_x = window_width/2;
    mid_window_y = window_height/2;
    aspect_ratio = (GLfloat)window_width/GLfloat(window_height);

    field_of_view = 60.0f;            // Define our field of view (i.e. how quickly foreshortening occurs)
    near        = 2.0f;             // The near (Z Axis) point of our viewing frustum (default 2.0f)
    far         = 1500.0f;          // The far  (Z Axis) point of our viewing frustum (default 1500.0f)

    // Initialise GLFW
    if (!glfwInit() )
    {
        std::cout << "Failed to initialise GLFW!" << endl;
        glfwTerminate();
        exit(1);
    }

    // glfwOpenWindowHint( GLFW_STEREO, GL_TRUE );

    // Create a window
    // GLFW_FULLSCREEN / GLFW_WINDOW
    if(full_screen) {
        GLFWvidmode desktop;
        glfwGetDesktopMode( &desktop );
        window_width = desktop.Width;
        window_height = desktop.Height;
        redBits = desktop.RedBits;
        greenBits = desktop.GreenBits;
        blueBits = desktop.BlueBits;

        mid_window_x = window_width/2;
        mid_window_y = window_height/2;
        aspect_ratio = (GLfloat)window_width/GLfloat(window_height);

        if( !glfwOpenWindow(window_width, window_height, redBits, greenBits, blueBits, alphaBits, depthBits, stencilBits, GLFW_FULLSCREEN))
        {
            std::cout << "Failed to open window!" << std::endl;
            glfwTerminate();
            exit(1);
        }

        glfwSetWindowPos(0,0);
    } else {
        if( !glfwOpenWindow(window_width, window_height, redBits, greenBits, blueBits, alphaBits, depthBits, stencilBits, GLFW_WINDOW))
        {
            std::cout << "Failed to open window!" << std::endl;
            glfwTerminate();
            exit(1);
        }
        glfwSetWindowSize(w,h);
        glfwSetWindowPos(0,0);
    }

    

    // Call our initGL function to set up our OpenGL options
    init_GL();

    // Instantiate our pointer to a Camera object providing it the size of the window
    camera = new Camera(window_width, window_height, camera_speed);

    // Set the mouse cursor to the centre of our window
    glfwSetMousePos(mid_window_x, mid_window_y);

    // // Specify the function which should execute when a key is pressed or released
    glfwSetKeyCallback(handle_keypress);

    // // Specify the function which should execute when the mouse is moved
    glfwSetMousePosCallback(handle_mouse_move);
}

void MDOpenGL::set_window_title(string window_title_) {
  window_title = window_title_;
  glfwSetWindowTitle(window_title.c_str());
  glfwPollEvents();
}

void MDOpenGL::init_GL() {
    // ----- GLFW Settings -----
    
    glfwDisable(GLFW_MOUSE_CURSOR); // Hide the mouse cursor
 
    glfwSwapInterval(0);            // Disable vsync
 
    // ----- Window and Projection Settings -----
 
    // Set the window title
    glfwSetWindowTitle(window_title.c_str());
 
    // Setup our viewport to be the entire size of the window
    glViewport(0, 0, (GLsizei)window_width, (GLsizei)window_height);
 
    // Change to the projection matrix, reset the matrix and set up our projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
 
    // The following code is a fancy bit of math that is eqivilant to calling:
    // gluPerspective(fieldOfView / 2.0f, width / height, near, far);
    // We do it this way simply to avoid requiring glu.h
    // GLfloat aspectRatio = (window_width > window_height)? float(window_width)/float(window_height) : float(window_height)/float(window_width);
    GLfloat fH = tan( float(field_of_view / 360.0f * 3.14159f) ) * near;
    GLfloat fW = fH * aspect_ratio;
    glFrustum(-fW, fW, -fH, fH, near, far);
 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
 
    // ----- OpenGL settings -----
 
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);              // Set out clear colour to black, full alpha
    glEnable(GL_DEPTH_TEST);                           // Enable the depth buffer
    glClearDepth(1.0f);                                // Clear the entire depth of the depth buffer
    glDepthFunc(GL_LEQUAL);                            // Set our depth function to overwrite if new value less than or equal to current value
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Ask for nicest perspective correction
    glLineWidth(2.0f);                                 // Set a 'chunky' line width
    glShadeModel(GL_SMOOTH);
}

void MDOpenGL::push() {
  glPushMatrix();
}

void MDOpenGL::pop() {
  glPopMatrix();
}

CVector MDOpenGL::coord_to_ray(double px, double py) {
   double P = field_of_view / 360.0 * 2*3.14159; // convert to radians
   float pm[16];     // to get viewport matrix
   CVector res, dir;
   
   double x = px + window_width/2;
   double y = py + window_height/2;
   // modifiers 
   double mmx = tan(P*0.5)*(1.0-(x*2)/(double)window_width)* (window_width/(double)window_height);
   double mmy = tan(P*0.5)*-(1.0-(y*2)/(double)window_height);
   
   glGetFloatv(GL_MODELVIEW_MATRIX, pm);
   // find position in viewspace
   dir = CVector(mmx,mmy,1);
   res = (dir.glMatMul(pm)).Normalize();
   
   return res;       
}