//
//  helper.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 16/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "helper.h"

#include <math.h>
#include <algorithm>

inline float* linearInterpolateColor(float ratio, const float colorOne[3],const float colorTwo[3]) {
    float color[3];
    printArray(colorOne, 3);
    printArray(colorTwo, 3);
    for(int i = 0 ; i < 3 ; i++) {
        color[i] = ratio * colorOne[i] + (1-ratio) * colorTwo[i];
    }
    printArray(color, 3);
    return color;
}
void println(const std::string& text) {
	std::cout << text << std::endl;
}
double diffclock( clock_t clock1, clock_t clock2 ) {
	
	double diffticks = clock1 - clock2;
	double diffms    = diffticks / ( CLOCKS_PER_SEC / 1000 );
	
	return diffms;
}
bool almost_equal(double x, double y, double epsilon)
{
    double diff = x - y;
    if (x != 0 && y != 0){
        diff = diff/y;
    }
	
    if (diff < epsilon && -1.0*diff < epsilon){
        return true;
    }
    return false;
}


// r,g,b values are from 0 to 1
// h = [0,360], s = [0,1], v = [0,1]
//		if s == 0, then h = -1 (undefined)
void RGBtoHSV( float r, float g, float b, float *h, float *s, float *v )
{
	float min_val, max_val, delta;
	min_val = std::min( r, std::min( g, b ));
	max_val = std::max( r, std::max( g, b ));
	*v = max_val;				// v
	delta = max_val - min_val;
	if( max_val != 0 )
		*s = delta / max_val;		// s
	else {
		// r = g = b = 0		// s = 0, v is undefined
		*s = 0;
		*h = -1;
		return;
	}
	if( r == max_val )
		*h = ( g - b ) / delta;		// between yellow & magenta
	else if( g == max_val )
		*h = 2 + ( b - r ) / delta;	// between cyan & yellow
	else
		*h = 4 + ( r - g ) / delta;	// between magenta & cyan
	*h *= 60;				// degrees
	if( *h < 0 )
		*h += 360;
}
void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v )
{
	int i;
	float f, p, q, t;
	if( s == 0 ) {
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}
	h /= 60;			// sector 0 to 5
	i = floor( h );
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );
	switch( i ) {
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;
		default:		// case 5:
			*r = v;
			*g = p;
			*b = q;
			break;
	}
}

unsigned long factorization(int N,unsigned long maxF) {
    if(N < 0) {
        throw std::string("Invalid value for N");
    }
    unsigned long f = 1;
    unsigned long lastF = 0; //To observe overflows
    for(int i = 1 ; i <= N ; ++i) {
        f *= i;
        if(f > maxF) {
            return maxF;
        }
        if(f < lastF) { //It wrapped around
            return std::numeric_limits<unsigned long>::max();
        }
        lastF = f;
    }
    return f;
}

unsigned long long
choose(unsigned long long n, unsigned long long k) {
    if (k > n) {
        return 0;
    }
    unsigned long long r = 1;
    for (unsigned long long d = 1; d <= k; ++d) {
        r *= n--;
        r /= d;
    }
    return r;
}

void printDelimiterLine(char c,int width) {
    int i = 0 ;
    while ( i < width) {
        std::cout << c;
        ++i;
    }
    std::cout << "\n";
}
std::vector<std::vector<size_t> > generateCombinations(unsigned long long n, unsigned long long r, size_t requested) {
    
    std::vector<bool> v(n);
    std::fill(v.begin() + n - r, v.end(), true);
    
    unsigned long long nrOfCombinations = choose(n,r);
    //    printf("Generating %llu combinations\n",nrOfCombinations);
    
    if(requested == -1 || requested > nrOfCombinations) {
        printf("Warning: More combinations requested than can be generated.");
        requested = nrOfCombinations;
    }
    
    //Sample 3 combinations
    NonReplacementSampler<unsigned long long> sampler = NonReplacementSampler<unsigned long long>(0,nrOfCombinations);
    std::vector<size_t> combinationsChosen;
    for(size_t i = 0 ; i < requested ; i++) {
        combinationsChosen.push_back(sampler.RequestSample());
    }
    
    std::vector<std::vector<size_t> > combinations;
    size_t combinationIndex = 0;
    
    do {
        std::vector<size_t> combination;
        for (size_t i = 0; i < n; ++i) {
            if (v[i]) {
//                std::cout << (i+1) << " ";
                combination.push_back(i);
            }
        }
        if (std::find(combinationsChosen.begin(), combinationsChosen.end(), combinationIndex) != combinationsChosen.end()) {
            combinations.push_back(combination);
        }
        combinationIndex++;
//        std::cout << "\n";
    } while (std::next_permutation(v.begin(), v.end()));
    
//    for(std::vector<size_t> combination : combinations) {
//        std:: cout << "[";
//        for(size_t i : combination) {
//            std::cout << i << " ";
//        }
//        std::cout << "]";
//    }
    
    return combinations;
}

std::vector<std::vector<size_t> > generateCombinations(std::vector<size_t> pool, size_t r, size_t requested) {
    
    size_t n = pool.size();
    
    std::vector<bool> v(n);
    std::fill(v.begin() + n - r, v.end(), true);
    
    unsigned long long nrOfCombinations = choose(n,r);
    //    printf("Generating %llu combinations\n",nrOfCombinations);
    
    if(requested > nrOfCombinations) {
        throw "More combinations requested than can be generated.";
    }
    
    //Sample 3 combinations
    NonReplacementSampler<size_t> sampler = NonReplacementSampler<size_t>(0,nrOfCombinations);
    std::vector<size_t> combinationsChosen;
    for(size_t i = 0 ; i < requested ; i++) {
        combinationsChosen.push_back(sampler.RequestSample());
    }
    
    std::vector<std::vector<size_t> > combinations;
    size_t combinationIndex = 0;
    
    do {
        std::vector<size_t> combination;
        for (size_t i = 0; i < n; ++i) {
            if (v[i]) {
//                std::cout << (i+1) << " ";
                combination.push_back(pool[i]);
            }
        }
        if (std::find(combinationsChosen.begin(), combinationsChosen.end(), combinationIndex) != combinationsChosen.end()) {
            combinations.push_back(combination);
        }
        combinationIndex++;
//        std::cout << "\n";
    } while (std::next_permutation(v.begin(), v.end()));
    
//    for(std::vector<size_t> combination : combinations) {
//        std:: cout << "[";
//        for(size_t i : combination) {
//            std::cout << i << " ";
//        }
//        std::cout << "]";
//    }
    
    return combinations;
}

void copy_buffer()
{
    static GLint viewport[4];
    static GLfloat raster_pos[4];
    
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    /* set source buffer */
    glReadBuffer(GL_FRONT); //red
    //    glDrawBuffer(GL_BACK); //green
    
    /* set projection matrix */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity() ;
    gluOrtho2D(0, viewport[2], 0, viewport[3]);
    
    /* set modelview matrix */
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    /* save old raster position */
    glGetFloatv(GL_CURRENT_RASTER_POSITION, raster_pos);
    
    /* set raster position */
    glRasterPos4f(0.0, 0.0, 0.0, 1.0);
    
    /* copy buffer */
    glCopyPixels(0, 0, viewport[2], viewport[3], GL_COLOR);
    
    /* restore old raster position */
    glRasterPos4fv(raster_pos);
    
    /* restore old matrices */
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    /* restore source buffer */
    //    glReadBuffer(GL_BACK);
    glReadBuffer(GL_FRONT); //red
}

void copy_buffer_back_to_front()
{
    static GLint viewport[4];
    static GLfloat raster_pos[4];
    
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    /* set source buffer */
    glReadBuffer(GL_BACK);
    
    /* set projection matrix */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity() ;
    gluOrtho2D(0, viewport[2], 0, viewport[3]);
    
    /* set modelview matrix */
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    /* save old raster position */
    glGetFloatv(GL_CURRENT_RASTER_POSITION, raster_pos);
    
    /* set raster position */
    glRasterPos4f(0.0, 0.0, 0.0, 1.0);
    
    /* copy buffer */
    glCopyPixels(0, 0, viewport[2], viewport[3], GL_COLOR);
    
    /* restore old raster position */
    glRasterPos4fv(raster_pos);
    
    /* restore old matrices */
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    /* restore source buffer */
    glReadBuffer(GL_FRONT);
}
