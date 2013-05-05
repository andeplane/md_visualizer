#pragma once

#include <CApplication.h>
#include <COpenGL.h>

using namespace std;

class Variables  {
public:
    
};

class MDVisualizer : public CApplication {
public:
    static Variables var;

    static void Initialize_();

    // Rasterizing
    static void Display(void);

    // Update physics / OpenGL
    static void Update(void);

    // Handle events (keys, mouse)
    static void Events();

    // internal stuff
private:    
    
};
