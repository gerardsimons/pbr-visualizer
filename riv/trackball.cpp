//
//  trackball.cpp
//  embree
//
//  Created by Gerard Simons on 27/11/14.
//
//

#include "trackball.h"

using std::cout;

/// Matrice identite
const GLdouble identity[] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};


/// Affichage
void printMatrix( const GLdouble* m )
{
	for( int i=0; i<16; ++i ) cout<<m[i]<<" ";
}


/// Projection q=Mp
void project( const GLdouble* m, const GLdouble* p, GLdouble* q )
{
	for( int i=0; i<3; ++i ){
		q[i] = 0;
		for( int j=0; j<4; ++j )
			q[i] += m[i+4*j]*p[j];
	}
	q[3] = p[3];
}


/// Inversion
void inverse( const double *m, double *p )
{
	{   // transposition de la rotation
		for(int i=0; i<3; ++i )
			for(int j=0; j<3; ++j )
				p[i+4*j] = m[j+4*i];
	}
	
	{   // calcul de la translation
		double q[3]={0,0,0};
		for(int i=0; i<3; ++i ){
			for(int j=0; j<3; ++j )
				q[i] -= p[i+4*j]*m[12+j];
		}
		p[12]=q[0]; p[13]=q[1]; p[14]=q[2];
	}
	
	// derniere ligne
	p[3] = p[7] = p[11] = 0; p[15] = 1;
}


/** Lit dans la matrice courante la position initiale du point de vue */
void Trackball::tbInitTransform()
{
	glGetDoublev( GL_MODELVIEW_MATRIX, tb_matrix );
	inverse( tb_matrix, tb_inverse );
}

/** Applique la transformation de point de vue */
void Trackball::tbVisuTransform()
{
	glMultMatrixd( tb_matrix );
};

/** Affiche l'aide */
void Trackball::tbHelp()
{
	std::cout<<"Left button to turn,\n";
	std::cout<<"Right button to translate,\n";
	std::cout<<"Middle button to zoom.\n";
}

/** Gere les boutons de la souris */
void Trackball::tbMouseFunc( int button, int state, int x, int y )
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
		tb_bougerZ = 1;
		tb_ancienX = x;
		tb_ancienY = y;
	}
	/* relacher milieu */
	else if( button==GLUT_MIDDLE_BUTTON && state==GLUT_UP )
	{
		tb_bougerZ = 0;
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
void Trackball::tbMotionFunc( int x, int y )
{
	double dx,dy,nrm, tx,ty,tz;
	
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
void Trackball::tbRotate( double angle, double x, double y, double z )
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
void Trackball::tbProject( const GLdouble *m, const GLdouble* p, GLdouble* q )
{
	double pp[4];
	//cout<<"tb, matrix: "; printMatrix(tb_matrix); cout<<endl;
	//cout<<"tb, inverse: "; printMatrix(tb_inverse); cout<<endl;
	project( m, p, pp );
	//cout<<"proj: "<<pp[0]<<", "<<pp[1]<<", "<<pp[2]<<", "<<pp[3]<<endl;
	project( tb_inverse, pp, q );
	//cout<<"projRep: "<<q[0]<<", "<<q[1]<<", "<<q[2]<<", "<<q[3]<<endl;
}
