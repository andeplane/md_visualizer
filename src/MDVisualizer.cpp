#include <MDVisualizer.h>
#include <vector>
#include <GLUT/glut.h>

Variables MDVisualizer::var;

void MDVisualizer::Update(void) {
    if(!Initialized) Initialize_();
    glutPostRedisplay();
    InternalUpdate();
    Events();
}

void MDVisualizer::Display (void) {
    if (!Initialized)
    return;

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); 

    ogl.clip_near = 0.01;
    ogl.clip_far = 1000;
    ogl.setperspective(60);

    ogl.camera = CVector(0,0,1);
    
    ogl.target = CVector(0,0,0);
    ogl.setup_camera();

    glBegin(GL_POINTS);

    glVertex3f(0.0,0.0,0.0);

    glEnd();

    glutSwapBuffers(); 
    Events();
}

void MDVisualizer::Initialize_() {
    Initialized = true;
}  

void MDVisualizer::Events ()  {
    if (key==27)
        exit(1);
}
