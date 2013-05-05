#define ARMA_NO_DEBUG
#include <MDVisualizer.h>

int main(int a, char** b) {
    MDVisualizer p;

    // Wrapper: loads ini file and initialized all
    try {
        p.Initialize("md_visualizer.ini");
        p.Loop(p.Display, p.Update, std::string("Molecular Dynamics"), a, b);
    }
    catch (string l) {
        cout << endl <<l << endl;
        return 1;
    }
    catch (...) {
        cout << "Unknown, fucked-up error.. should not happen!" << endl;
        return 1;
    }
    return 0;
}
