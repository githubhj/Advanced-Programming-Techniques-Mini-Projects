/*
 * calsses.h
 *
 *  Created on: Nov 6, 2014
 *      Author: Harshit
 */

#ifndef CALSSES_H_
#define CALSSES_H_
#include<iostream>
#include<GL/glut.h>
#include <GL/gl.h>

typedef struct vertex{
	GLfloat x;
	GLfloat y;
	GLfloat z;
}vertex_t;

typedef struct face{
	int a,b,c;
	int normalIdxA, normalIdxB, normalIdxC;
	int texA, texB,	texC;
	int material;
	bool istexture;
}face_t;

typedef struct material{
	//Shiness Value:
	GLfloat ns;

	//Transparency Value:
	GLfloat Tr;

	//Ambient Light Values:
	GLfloat ar, ag, ab;

	//Diffusive Light values:
	GLfloat dr, dg, db;

	//Emissive Light Values:
	GLfloat er, eg, eb;

	//Specular Light Values:
	GLfloat sr,sg,sb;

}material_t;

typedef struct texture{
public:
	GLfloat u,v;
}texture_t;




#endif /* CALSSES_H_ */
