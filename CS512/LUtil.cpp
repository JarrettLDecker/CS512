/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 001
#include "stdafx.h"
#include "LUtil.h"
#include <iostream>
#include <fstream>
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <iterator>

std::vector<glm::vec3> verticies;
std::vector<glm::vec3> faces;

bool initGL()
{
	std::cout << "initGL()" << std::endl;
    //Initialize Projection Matrix
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    //Initialize Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    //Initialize clear color
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    //Check for error
    GLenum error = glGetError();
    if( error != GL_NO_ERROR )
    {
        printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
        return false;
    }

    return true;
}
	
	/* 
	Take in a pathname and replace verticies and faces
	with data from model file.
	*/

void loadModel(std::string name) {

	verticies.clear();
	faces.clear();

	char character[2];
	FILE * pFile;
	verticies.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	pFile = fopen(name.c_str(), "r");
	std::cout << "Opened file" << std::endl;
	glm::vec3 v(0.0f, 0.0f, 0.0f);
	while (fscanf(pFile, "%s %f %f %f", &character[0], &v.x, &v.y, &v.z)) {
		if (feof(pFile) || (character[0] != 'v' && character[0] != 'f')) break;
		if (character[0] == 'v') verticies.push_back(v);
		else faces.push_back(v);
	}
	
	fclose(pFile);
	std::cout << "Closed file" << std::endl;
	return;
}

void update() {

}

void render()
{
    //Clear color buffer
    glClear( GL_COLOR_BUFFER_BIT );

    //Render quad
//    glBegin( GL_QUADS );
//        glVertex2f( -0.5f, -0.5f );
//        glVertex2f(  0.5f, -0.5f );
//        glVertex2f(  0.5f,  0.5f );
//        glVertex2f( -0.5f,  0.5f );
//    glEnd();

	for (int i = 0; i < faces.size(); i++) {
		glBegin(GL_TRIANGLES);
			glVertex3f(verticies[faces[i].x].x, verticies[faces[i].x].y, verticies[faces[i].x].z);
			glVertex3f(verticies[faces[i].y].x, verticies[faces[i].y].y, verticies[faces[i].y].z);
			glVertex3f(verticies[faces[i].z].x, verticies[faces[i].z].y, verticies[faces[i].z].z);
		glEnd();
	}

    //Update screen
    glutSwapBuffers();
}
