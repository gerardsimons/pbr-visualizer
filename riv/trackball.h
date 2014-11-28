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
#include <math.h>
#include <stdio.h>
#include <iostream>

static const float speedfact = 0.2;

class Trackball {
public:
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
	void tbInitTransform();

	/** Applique la transformation de point de vue */
	void tbVisuTransform();
	/** Affiche l'aide */
	void tbHelp();

	/** Gere les boutons de la souris */
	void tbMouseFunc( int button, int state, int x, int y );
	/** Traite le changement de position de la souris */
	void tbMotionFunc( int x, int y );

	/** Traite le changement de position de la souris */
	void tbRotate( double angle, double x, double y, double z );
	/// Projection dans le repere du monde
	void tbProject( const GLdouble *m, const GLdouble* p, GLdouble* q );
};
#endif
	

