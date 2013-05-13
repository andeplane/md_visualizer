#include <math.h>
#include <CVector.h>
#include <MDOpenGL.h>
#include <MDTexture.h>
#include <Camera.h>

#define SI_TYPE 1
#define A_TYPE 2
#define H_TYPE 3
#define O_TYPE 4
#define NA_TYPE 5
#define CL_TYPE 6
#define X_TYPE 7
                    // Not Av   Si                      Si-O       H            O      Na                   Cl
double colors[7][3] = {{1,1,1},{230.0/255,230.0/255,0},{0,0,1},{1.0,1.0,1.0},{1,0,0},{9.0/255,92.0/255,0},{95.0/255,216.0/255,250.0/255}};
double visual_atom_radii[7] = {0, 1.11, 0.66, 0.35, 0.66, 1.86, 1.02};

void MDTexture::create_sphere1(string name, int w) {
    CBitMap bmp;
    MDOpenGLTexture texture;
    double val;

    bmp.width = w;
    bmp.height= w;
    bmp.data = new unsigned char[4*w*w];
    double max_val = 0;
    double sqrt2 = sqrt(2);
    for (int i=0;i<w; i++) {
        for (int j=0;j<w; j++) {
            double x = (i-w/2)/(double)w;
            double y = (j-w/2)/(double)w;
            double dr = sqrt(x*x + y*y)*2;
            dr = min(dr,1.0);
            
            if(dr>=0.99) {
                bmp.data[4*i + 4*w*j  +0] = 0;
                bmp.data[4*i + 4*w*j  +1] = 0;
                bmp.data[4*i + 4*w*j  +2] = 0;
                bmp.data[4*i + 4*w*j  +3] = 0;
            } else {
                bmp.data[4*i + 4*w*j  +0] = (unsigned char)(255.0*(1 - 0.7*dr));
                bmp.data[4*i + 4*w*j  +1] = (unsigned char)(255.0*(1 - 0.7*dr));
                bmp.data[4*i + 4*w*j  +2] = (unsigned char)(255.0*(1 - 0.7*dr));
                bmp.data[4*i + 4*w*j  +3] = 255;
            }
        }
    }
    
    load_texture(&bmp, &texture, true);
    textures.push_back(texture);
    names.push_back(name);
}

void MDTexture::create_sphere2(string name, int w) {
    CBitMap bmp;
    MDOpenGLTexture texture;
    double val;

    bmp.width = w;
    bmp.height= w;
    bmp.data = new unsigned char[4*w*w];
    double max_val = 0;
    double sqrt2 = sqrt(2);
    for (int i=0;i<w; i++) {
        for (int j=0;j<w; j++) {
            double x = (i-w/2)/(double)w;
            double y = (j-w/2)/(double)w;
            double dr = sqrt(x*x + y*y)*2;
            dr = min(dr,1.0);
            
            bmp.data[4*i + 4*w*j  +0] = (unsigned char)(255.0*(1 - dr));
            bmp.data[4*i + 4*w*j  +1] = (unsigned char)(255.0*(1 - dr));
            bmp.data[4*i + 4*w*j  +2] = (unsigned char)(255.0*(1 - dr));
            bmp.data[4*i + 4*w*j  +3] = 255.0;
        }
    }
    
    load_texture(&bmp, &texture,true);
    textures.push_back(texture);
    names.push_back(name);
}

void MDTexture::load_texture(CBitMap* bmp, MDOpenGLTexture* texture, bool has_alpha) {
    glGenTextures(1, &texture->id); 

    texture->has_alpha = true;
    glBindTexture(GL_TEXTURE_2D, texture->id);

    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    if(has_alpha) gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA, bmp->width, bmp->height, GL_RGBA, GL_UNSIGNED_BYTE, bmp->data  );
    else gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGB, bmp->width, bmp->height, GL_RGB, GL_UNSIGNED_BYTE, bmp->data  );
    
}



void MDTexture::render_billboards(MDOpenGL &opengl, vector<int> &atom_types, vector<vector<float> > &positions, bool draw_water, double color_cutoff, double dr2_max, vector<float> system_size, bool periodic_boundary_conditions) {
    Camera *camera = opengl.camera;
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA_SATURATE);
    MDOpenGLTexture texture = textures[0];
    GLuint texture_id = texture.id;

    glEnable( GL_TEXTURE_2D );
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glDepthMask(GL_TRUE);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER,0.9);

    glBegin(GL_QUADS);


    CVector left, up, right, direction, v0, v1, v2, v3;
    int S = 1.0;
    double cam_x = camera->position.x; double cam_y = camera->position.y; double cam_z = camera->position.z;
    double one_over_color_cutoff = 1.0/color_cutoff;

    CVector up_on_screen = opengl.coord_to_ray(0,opengl.window_height/2.0);
    double dx = camera->target.x - cam_x;
    double dy = camera->target.y - cam_y;
    double dz = camera->target.z - cam_z;
    direction = camera->target;

    left = direction.Cross(up_on_screen);
    up = (direction.Cross(left)).Normalize();
    right = (direction.Cross(up)).Normalize();

    v0 = (right + up);
    v1 = (right*-1 + up);
    v2 = (right*-1 + up*-1);
    v3 = (right + up*-1);
    
    glNormal3f(direction.x, direction.y, direction.z);
    for(int n=0; n<positions.size(); n++) {
        int atom_type = atom_types[n];
        if( (atom_type == H_TYPE || atom_type == O_TYPE) && !draw_water) continue;

        double scale = visual_atom_radii[atom_type];

        double real_x = positions[n][0];
        double real_y = positions[n][1];
        double real_z = positions[n][2];

        for(int dx = -1; dx <= 1; dx++) {
            for(int dy = -1; dy <= 1; dy++) {
                for(int dz = -1; dz <= 1; dz++) {
                    if(!periodic_boundary_conditions && (dx != 0 || dy != 0 || dz != 0)) continue;

                    double x = real_x + system_size[0]*dx;
                    double y = real_y + system_size[1]*dy;
                    double z = real_z + system_size[2]*dz;

                    double delta_x = x - cam_x;
                    double delta_y = y - cam_y;
                    double delta_z = z - cam_z;

                    double dr2 = delta_x*delta_x + delta_y*delta_y + delta_z*delta_z;
                    if(dr2 < 50) continue;
                    if(dr2 > dr2_max) continue;

                    double cam_target_times_dr = delta_x*direction.x + delta_y*direction.y + delta_z*direction.z;
                    if(cam_target_times_dr < 0) continue;

                    double color_factor = max( (1-dr2*one_over_color_cutoff),0.1);
                    glColor4f(color_factor*colors[atom_type][0],color_factor*colors[atom_type][1],color_factor*colors[atom_type][2], 1.0);
                    
                    glTexCoord2f(0,0);
                    glVertex3f(v0.x*scale + x,v0.y*scale + y,v0.z*scale + z);
                    glTexCoord2f(1,0);
                    glVertex3f(v1.x*scale + x,v1.y*scale + y,v1.z*scale + z);
                    glTexCoord2f(1,1);
                    glVertex3f(v2.x*scale + x,v2.y*scale + y,v2.z*scale + z);
                    glTexCoord2f(0,1);
                    glVertex3f(v3.x*scale + x,v3.y*scale + y,v3.z*scale + z);
                }
            }
        }
    }
    
    glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glDisable(GL_ALPHA_TEST);

    glDepthMask(GL_TRUE);
    glDisable( GL_TEXTURE_2D );
    glColor4f(1.0,1.0,1.0,1.0);
}

void MDTexture::render_billboards2(MDOpenGL &opengl, vector<int> &atom_types, vector<vector<float> > &positions, bool draw_water, double color_cutoff, double dr2_max, vector<float> system_size, bool periodic_boundary_conditions) {
    Camera *camera = opengl.camera;
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    MDOpenGLTexture texture = textures[1];
    GLuint texture_id = texture.id;

    glEnable( GL_TEXTURE_2D );
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glDepthMask(GL_FALSE);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER,0.05);

    glBegin(GL_QUADS);

    CVector left, up, right, direction, v0, v1, v2, v3;
    int S = 1.0;
    double cam_x = camera->position.x; double cam_y = camera->position.y; double cam_z = camera->position.z;
    double one_over_color_cutoff = 1.0/color_cutoff;

    CVector up_on_screen = opengl.coord_to_ray(0,opengl.window_height/2.0);
    double dx = camera->target.x - cam_x;
    double dy = camera->target.y - cam_y;
    double dz = camera->target.z - cam_z;
    direction = camera->target;

    left = direction.Cross(up_on_screen);
    up = (direction.Cross(left)).Normalize();
    right = (direction.Cross(up)).Normalize();

    v0 = (right + up);
    v1 = (right*-1 + up);
    v2 = (right*-1 + up*-1);
    v3 = (right + up*-1);
    
    glNormal3f(direction.x, direction.y, direction.z);

    for(int n=0; n<positions.size(); n++) {
        int atom_type = atom_types[n];
        if( (atom_type == H_TYPE || atom_type == O_TYPE) && !draw_water) continue;
        double scale = visual_atom_radii[atom_type];

        double real_x = positions[n][0];
        double real_y = positions[n][1];
        double real_z = positions[n][2];

        for(int dx = -1; dx <= 1; dx++) {
            for(int dy = -1; dy <= 1; dy++) {
                for(int dz = -1; dz <= 1; dz++) {
                    if(!periodic_boundary_conditions && (dx != 0 || dy != 0 || dz != 0)) continue;

                    double x = real_x + system_size[0]*dx;
                    double y = real_y + system_size[1]*dy;
                    double z = real_z + system_size[2]*dz;

                    double delta_x = x - cam_x;
                    double delta_y = y - cam_y;
                    double delta_z = z - cam_z;

                    double dr2 = delta_x*delta_x + delta_y*delta_y + delta_z*delta_z;
                    if(dr2 < 50) continue;
                    if(dr2 > dr2_max) continue;
                    
                    double cam_target_times_dr = delta_x*direction.x + delta_y*direction.y + delta_z*direction.z;
                    if(cam_target_times_dr < 0) continue;

                    glColor4f(colors[atom_type][0],colors[atom_type][1],colors[atom_type][2], 0.3);
                    
                    glTexCoord2f(0,0);
                    glVertex3f(v0.x*scale + x,v0.y*scale + y,v0.z*scale + z);
                    glTexCoord2f(1,0);
                    glVertex3f(v1.x*scale + x,v1.y*scale + y,v1.z*scale + z);
                    glTexCoord2f(1,1);
                    glVertex3f(v2.x*scale + x,v2.y*scale + y,v2.z*scale + z);
                    glTexCoord2f(0,1);
                    glVertex3f(v3.x*scale + x,v3.y*scale + y,v3.z*scale + z);
                }
            }
        }
    }
    
    glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glDisable(GL_ALPHA_TEST);

    glDepthMask(GL_TRUE);
    glDisable( GL_TEXTURE_2D );
    glColor4f(1.0,1.0,1.0,1.0);
}