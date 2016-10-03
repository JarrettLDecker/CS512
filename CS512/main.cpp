#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <iterator>
#include <math.h>
#include <stdio.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

void loadModel(std::string);

std::vector<glm::vec3> verticies;
std::vector<glm::vec3> faces;

glm::vec3 camPosition(0.0f, 0.0f, 4.0f);
glm::vec3 camForward(0.0f, 0.0f, -1.0f);
glm::vec3 camUp(0.0f, 1.0f, 0.0f);

float red = 1, green = 1, blue = 1;

float fov = 45.0;
float nearClip = 0.1;
float farClip = 10.0;

bool clockwise = false;

void translateCamera(glm::vec3 vec, float scale) {
	camPosition = camPosition + (vec * scale);
	std::cout << "Moving camera by [" << vec.x << "," << vec.y << "," << vec.z << "]" << std::endl;
}

glm::vec3 rotateVec3(glm::vec3 vec, glm::vec3 axis, float angle) {
	return ((cos(angle) * vec) + (sin(angle) * (glm::cross(axis, vec))) + ((1 - cos(angle)) * (glm::dot(axis, vec) * axis)));
}

void rotateCamera(glm::vec3 axis, float angle) {
	camForward = glm::normalize(rotateVec3(camForward, axis, angle));
	camUp = glm::normalize(rotateVec3(camUp, axis, angle));
	std::cout << "Rotated camera " << "camForward = " << camForward.x << "x " << camForward.y << "y " << camForward.z << "z camUp = "
		 << camUp.x << "x " << camUp.y << "y " << camUp.z << "z" << std::endl;
}

float changeColor(float color, float value) {
	float ret = color + value;
	if (ret < 0) ret = 0;
	if (ret > 1) ret = 1;
	return ret;
}

void resetCamera() {
	camPosition = glm::vec3(0, 0, 4);
	camForward = glm::vec3(0, 0, -1);
	camUp = glm::vec3(0, 1, 0);
	fov = 60.0;
	nearClip = 0.1;
	farClip = 10.0;
}

void changeClockwise() {
	if (clockwise) glFrontFace(GL_CCW);
	else glFrontFace(GL_CW);
	clockwise = !clockwise;
}

void changeNear(float value) {
	nearClip += value;
	std::cout << "near changed by " << value << " nearClip = " << nearClip << std::endl;
}

void changeFar(float value) {
	farClip += value;
	std::cout << "far changed by " << value << " farClip = " << farClip << std::endl;
}

void changeFov(float value) {
	fov += value;
	std::cout << "fov changed by " << value << " fov = " << fov << std::endl;
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == 'q' || key == 'Q') exit(0);
	else if (key == 's') translateCamera(-camForward, 0.1);
	else if (key == 'w') translateCamera(camForward, 0.1);
	else if (key == 'd') translateCamera(glm::cross(camForward, camUp), 0.1);
	else if (key == 'a') translateCamera(glm::cross(camUp, camForward), 0.1);
	else if (key == 'z') translateCamera(camUp, 0.1);
	else if (key == 'x') translateCamera(-camUp, 0.1);

	else if (key == 'W') rotateCamera(glm::cross(camForward, camUp), 0.1);
	else if (key == 'S') rotateCamera(glm::cross(camUp, camForward), 0.1);
	else if (key == 'A') rotateCamera(camUp, 0.1);
	else if (key == 'D') rotateCamera(-camUp, 0.1);
	else if (key == 'Z') rotateCamera(camForward, 0.1);
	else if (key == 'X') rotateCamera(-camForward, 0.1);

	else if (key == '1') loadModel("models/bunny.obj");
	else if (key == '2') loadModel("models/cactus.obj");
	
	else if (key == 'r') red = changeColor(red, -.05);
	else if (key == 'R') red = changeColor(red, .05);

	else if (key == 'g') green = changeColor(green, -.05);
	else if (key == 'G') green = changeColor(green, .05);

	else if (key == 'b') blue = changeColor(blue, -.05);
	else if (key == 'B') blue = changeColor(blue, .05);

	else if (key == 'p') glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	else if (key == 'l') glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else if (key == 'f') glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	else if (key == 'e') resetCamera();

	else if (key == 'c') changeClockwise();

	//near
	else if (key == '-') changeNear(0.1);
	else if (key == '_') changeNear(-0.1);
	//far
	else if (key == '=') changeFar(0.1);
	else if (key == '+') changeFar(-0.1);
	//fov
	else if (key == '[') changeFov(1);
	else if (key == ']') changeFov(-1);
}

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

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glShadeModel(GL_SMOOTH);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, 1.0f, nearClip, farClip);
	gluLookAt(camPosition.x, camPosition.y, camPosition.z,
		camPosition.x + camForward.x, camPosition.y + camForward.y, camPosition.z + camForward.z,
		camUp.x, camUp.y, camUp.z);
	glColor3f(red, green, blue);

	glBegin(GL_TRIANGLES);
	for (int i = 0; i < faces.size(); i++) {
		glVertex3f(verticies[faces[i].x].x, verticies[faces[i].x].y, verticies[faces[i].x].z);
		glVertex3f(verticies[faces[i].y].x, verticies[faces[i].y].y, verticies[faces[i].y].z);
		glVertex3f(verticies[faces[i].z].x, verticies[faces[i].z].y, verticies[faces[i].z].z);
	}
	glEnd();

	glFlush();
	glutSwapBuffers();
}

void spinCube() {
	glutPostRedisplay();
}

void myReshape(int w, int h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, 1.0f, nearClip, farClip);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv) {
	loadModel("models/bunny.obj");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitContextFlags(GLUT_COMPATIBILITY_PROFILE);
	glutCreateWindow("Model Viewer");
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutIdleFunc(spinCube);
	glutKeyboardFunc(keyboard);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glutMainLoop();
}
