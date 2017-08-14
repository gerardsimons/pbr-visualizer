/** \file trackball.h
 
An OpenGL trackball.
No worries... you do not need to understand or modify this file!

BTW all comments here are French. :)
*/
#ifndef TRAQUEBOULE
#define TRAQUEBOULE
#ifdef _WIN32
	#include <GL/glut.h>
#elif __APPLE__
    #include <GLUT/GLUT.h>
#elif __linux
	#include <GL/glut.h>
#endif
#include <cmath>
#include "Geometry/matrix.h"



static const float speedfact = 0.2;

/** votre fonction d'affichage */
void display();

/** Placement de la scene par rapport a la camera */
GLdouble tb_matrix[16] =   { 1,0,0,0,
                             0,1,0,0,
                             0,0,1,0,
                             0,0,0,1  };
GLdouble tb_inverse[16] =  { 1,0,0,0,
                             0,1,0,0,
                             0,0,1,0,
                             0,0,0,1  };

/** Gestion de la souris */
int tb_ancienX, tb_ancienY, tb_tournerXY=0, tb_translaterXY=0, tb_bougerZ=0;


/** Lit dans la matrice courante la position initiale du point de vue */
void tbInitTransform()
{
    glGetDoublev( GL_MODELVIEW_MATRIX, tb_matrix );
    inverse( tb_matrix, tb_inverse );
}

/** Applique la transformation de point de vue */
void tbVisuTransform()
{
    glMultMatrixd( tb_matrix );
};

/** Affiche l'aide */
void tbHelp()
{
    std::cout<<"Left button to turn,\n";
    std::cout<<"Right button to translate,\n";
    std::cout<<"Middle button to zoom.\n";
}
void tb_zoom(float zoom) {
    double dx,dy,nrm, tx,ty,tz;
    dx = 10;
    dy = 10;
//    if( horizontal > vertical )
//    { // rotation z
//        
//        tx = tb_matrix[12];
//        tb_matrix[12]=0;
//        ty = tb_matrix[13];
//        tb_matrix[13]=0;
//        tz = tb_matrix[14];
//        tb_matrix[14]=0;
//        
//        glLoadIdentity();
//        glRotatef( dx, 0,0,-1 );/*axe perpendiculaire a l'ecran*/
//        glMultMatrixd( tb_matrix );
//        glGetDoublev( GL_MODELVIEW_MATRIX, tb_matrix );
//        
//        tb_matrix[12] = tx;
//        tb_matrix[13] = ty;
//        tb_matrix[14] = tz;
//    }
//    else if( vertical > horizontal )
//    {
        tb_matrix[14] -= zoom*speedfact;
//    printf("tb_matrix:\n");
//    printMatrix(tb_matrix);
//    }
//    tb_ancienX = x;
//    tb_ancienY = y;
}
void setZoom(float zoom) {
    tb_matrix[14] = zoom;
}
void startZoom(int x, int y) {
    printf("Start tb zoom\n");
    tb_bougerZ = 1;
    tb_ancienX = x;
    tb_ancienY = y;
}

void stopZoom() {
    printf("Stop tb zoom");
    tb_bougerZ = 0;
}
/** Gere les boutons de la souris */
void tbMouseFunc( int button, int state, int x, int y )
{
    /* enfoncer gauche */
    if( button==GLUT_LEFT_BUTTON && state==GLUT_DOWN )
    {
        tb_tournerXY = 1;
        tb_ancienX = x;
        tb_ancienY = y;
    }
    /* relacher gauche */
    else if( button==GLUT_LEFT_BUTTON && state==GLUT_UP )
    {
        tb_tournerXY = 0;
    }
    /* enfoncer milieu */
    if( button==GLUT_MIDDLE_BUTTON && state==GLUT_DOWN )
    {
        startZoom(x, y);
    }
    /* relacher milieu */
    else if( button==GLUT_MIDDLE_BUTTON && state==GLUT_UP )
    {
        stopZoom();
    }
    /* enfoncer droit */
    else if( button==GLUT_RIGHT_BUTTON && state==GLUT_DOWN )
    {
        tb_translaterXY = 1;
        tb_ancienX = x;
        tb_ancienY = y;
    }
    /* relacher droit */
    else if( button==GLUT_RIGHT_BUTTON && state==GLUT_UP )
    {
        tb_translaterXY = 0;
    }
}

/** Traite le changement de position de la souris */
void tbMotionFunc( int x, int y )
{
    double dx,dy,nrm, tx,ty,tz;

//    printf("tb_matrix = \n");
//    for(int i = 0 ; i < 16 ; i+=4) {
//        printf("[ %.2f %.2f %.2f %.2f]\n",tb_matrix[i],tb_matrix[i+1],tb_matrix[i+2],tb_matrix[i+3]);
//    }
    
    if( tb_tournerXY || tb_translaterXY || tb_bougerZ )
    {
        /* deplacement */
        dx = x - tb_ancienX;
        dy = tb_ancienY - y; /* axe vertical dans l'autre sens */

        if( tb_tournerXY )
        {
            tx = tb_matrix[12];
            tb_matrix[12]=0;
            ty = tb_matrix[13];
            tb_matrix[13]=0;
            tz = tb_matrix[14];
            tb_matrix[14]=0;

			nrm = ::sqrt( dx*dx+dy*dy+dx*dx+dy*dy )*speedfact;
            glLoadIdentity();
            glRotatef( nrm, -dy, dx, 0 );/*axe perpendiculaire au deplacement*/
            glMultMatrixd( tb_matrix );
            glGetDoublev( GL_MODELVIEW_MATRIX, tb_matrix );

            tb_matrix[12] = tx;
            tb_matrix[13] = ty;
            tb_matrix[14] = tz;
        }
        else if( tb_translaterXY )
        {
            tb_matrix[12] += dx/100.0*speedfact;
            tb_matrix[13] += dy/100.0*speedfact;
        }
        else if( fabs(dx)>fabs(dy) )
        { // rotation z
            
            tx = tb_matrix[12];
            tb_matrix[12]=0;
            ty = tb_matrix[13];
            tb_matrix[13]=0;
            tz = tb_matrix[14];
            tb_matrix[14]=0;

            glLoadIdentity();
            glRotatef( dx, 0,0,-1 );/*axe perpendiculaire a l'ecran*/
            glMultMatrixd( tb_matrix );
            glGetDoublev( GL_MODELVIEW_MATRIX, tb_matrix );

            tb_matrix[12] = tx;
            tb_matrix[13] = ty;
            tb_matrix[14] = tz;
        }
        else if( fabs(dy)>fabs(dx) )
        {
            tb_matrix[14] -= dy/100.0*speedfact;
        }
        tb_ancienX = x;
        tb_ancienY = y;
        inverse( tb_matrix, tb_inverse );
//        glutPostRedisplay();
    }
}

/** Traite le changement de position de la souris */
void tbRotate( double angle, double x, double y, double z )
{
    double tx,ty,tz;

    tx = tb_matrix[12];
    tb_matrix[12]=0;
    ty = tb_matrix[13];
    tb_matrix[13]=0;
    tz = tb_matrix[14];
    tb_matrix[14]=0;

    glLoadIdentity();
    glRotatef( angle, x, y, z );
    glMultMatrixd( tb_matrix );
    glGetDoublev( GL_MODELVIEW_MATRIX, tb_matrix );

    tb_matrix[12] = tx;
    tb_matrix[13] = ty;
    tb_matrix[14] = tz;

    inverse( tb_matrix, tb_inverse );
//    glutPostRedisplay();
}

/// Projection dans le repere du monde
void tbProject( const GLdouble *m, const GLdouble* p, GLdouble* q )
{
    double pp[4];
    //cout<<"tb, matrix: "; printMatrix(tb_matrix); cout<<endl;
    //cout<<"tb, inverse: "; printMatrix(tb_inverse); cout<<endl;
    project( m, p, pp );
    //cout<<"proj: "<<pp[0]<<", "<<pp[1]<<", "<<pp[2]<<", "<<pp[3]<<endl;
    project( tb_inverse, pp, q );
    //cout<<"projRep: "<<q[0]<<", "<<q[1]<<", "<<q[2]<<", "<<q[3]<<endl;
}
#endif
	

