#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <iterator>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <fstream> 
#include <sstream>

void loadModel(std::string);

std::vector<glm::vec3> verticies;
std::vector<glm::uvec3> faces;

glm::vec3 camPosition(0.0f, 0.0f, 4.0f);
glm::vec3 camForward(0.0f, 0.0f, -1.0f);
glm::vec3 camUp(0.0f, 1.0f, 0.0f);

float red = 1, green = 1, blue = 1;

float color[3] = { 0.5, 0.5, 0.5 };

float fov = 45.0;
float nearClip = 0.1;
float farClip = 10.0;
float aspectWidth = 1.0;
float aspectHeight = 1.0;

bool clockwise = false;
int window1, window2;

// for GLSL
GLuint vsID, fsID, pID, VBO, VAO, EBO;

// for uniform input
GLfloat projectionMat[16], modelViewMat[16];


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

void readShaderFile(const GLchar* shaderPath, std::string& shaderCode)
{

	std::ifstream shaderFile;

	// ensures ifstream objects can throw exceptions:
	shaderFile.exceptions(std::ifstream::badbit);
	try
	{
		// Open files
		shaderFile.open(shaderPath);
		std::stringstream shaderStream;

		// Read file's buffer contents into streams
		shaderStream << shaderFile.rdbuf();

		// close file handlers
		shaderFile.close();

		// Convert stream into GLchar array
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

}

void setShaders()
{
	char *vs = NULL, *fs = NULL;

	std::string vertShaderString; //empty string
	std::string fragShaderString; //empty string

	vsID = glCreateShader(GL_VERTEX_SHADER);
	fsID = glCreateShader(GL_FRAGMENT_SHADER);

	readShaderFile("vertexshader.txt", vertShaderString);
	readShaderFile("fragshader.txt", fragShaderString);

	const GLchar * pVertShaderSource = vertShaderString.c_str();
	const GLchar * pFragShaderSource = fragShaderString.c_str();

	std::cout << vertShaderString.c_str() << std::endl << std::endl;

	std::cout << fragShaderString.c_str() << std::endl << std::endl;


	glShaderSource(vsID, 1, &pVertShaderSource, NULL);
	glShaderSource(fsID, 1, &pFragShaderSource, NULL);

	glCompileShader(vsID);
	glCompileShader(fsID);
	int success = 99;

	glGetShaderiv(vsID, GL_COMPILE_STATUS, &success);

	if (success == GL_TRUE) {
		printf("Vertex compilation worked\n");
	}
	else {
		printf("Vertex compilation did not work\n");
	}

	glGetShaderiv(fsID, GL_COMPILE_STATUS, &success);
	if (success == GL_TRUE) {
		printf("Fragment compilation worked\n");
	}
	else {
		printf("Fragment compilation did not work\n");
	}


	pID = glCreateProgram();
	glAttachShader(pID, vsID);
	glAttachShader(pID, fsID);

	glLinkProgram(pID);
	glUseProgram(pID);

	// get uniform input 
	GLint modeViewMatLocation = glGetUniformLocation(pID, "modelViewMatrix");
	GLint projectionMatLocation = glGetUniformLocation(pID, "projectionMatrix");
	GLint colorMat = glGetUniformLocation(pID, "colorVec");

	glUniformMatrix4fv(modeViewMatLocation, 1, GL_FALSE, modelViewMat);
	glUniformMatrix4fv(projectionMatLocation, 1, GL_FALSE, projectionMat);
	//float colorArray[3] = { red, green, blue };
	glUniform1fv(colorMat, 3, &color[0]);

	//glDeleteShader(vsID);
	//glDeleteShader(fsID);
}

void initBufferObject(void)
{
	glutSetWindow(window2);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// setup VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(glm::vec3), &verticies[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(glm::uvec3), &faces[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	// Use depth buffering for hidden surface elimination
	//glEnable(GL_DEPTH_TEST);

	// Setup the view of the cube
	//glMatrixMode(GL_PROJECTION);
	//gluPerspective(40.0, 1.0, 1.0, 10.0);

	//glGetFloatv(GL_PROJECTION_MATRIX, projectionMat);


	//glMatrixMode(GL_MODELVIEW);
	//gluLookAt(0.0, 0.0, 5.0,  // eye is at (0,0,5) 
	//	0.0, 0.0, 0.0,        // center is at (0,0,0) 
	//	0.0, 1.0, 0.);        // up is in positive Y direction 

							  // Adjust cube position
	//glTranslatef(0.0, 0.0, -1.0);
	//glRotatef(60, 1.0, 0.0, 0.0);
	//glRotatef(-20, 0.0, 0.0, 1.0);

	//glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMat);
}
/*
void initBufferObject(void)
{
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// setup VAO
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), &verticies[0], GL_STATIC_DRAW);

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	//glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faces), &faces[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	
	// Use depth buffering for hidden surface elimination
	glEnable(GL_DEPTH_TEST);

	// Setup the view of the cube
	glMatrixMode(GL_PROJECTION);
	gluPerspective(40.0, 1.0, 1.0, 10.0);

	glGetFloatv(GL_PROJECTION_MATRIX, projectionMat);


	glMatrixMode(GL_MODELVIEW);
	gluLookAt(0.0, 0.0, 5.0,  // eye is at (0,0,5) 
		0.0, 0.0, 0.0,        // center is at (0,0,0) 
		0.0, 1.0, 0.);        // up is in positive Y direction 

							  // Adjust cube position
	glTranslatef(0.0, 0.0, -1.0);
	glRotatef(60, 1.0, 0.0, 0.0);
	glRotatef(-20, 0.0, 0.0, 1.0);

	glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMat);
	
}
*/

void resetCamera() {
	camPosition = glm::vec3(0, 0, 4);
	camForward = glm::vec3(0, 0, -1);
	camUp = glm::vec3(0, 1, 0);
	fov = 60.0;
	nearClip = 0.1;
	farClip = 10.0;
}

void changeClockwise() {
	if (clockwise) {
		glutSetWindow(window1);
		glFrontFace(GL_CCW);
		glutSetWindow(window2);
		glFrontFace(GL_CCW);
	}
	else {
		glutSetWindow(window1);
		glFrontFace(GL_CW);
		glutSetWindow(window2);
		glFrontFace(GL_CW);
	}
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
	
	else if (key == 'r') color[0] = changeColor(color[0], -.05);
	else if (key == 'R') color[0] = changeColor(color[0], .05);

	else if (key == 'g') color[1] = changeColor(color[1], -.05);
	else if (key == 'G') color[1] = changeColor(color[1], .05);

	else if (key == 'b') color[2] = changeColor(color[2], -.05);
	else if (key == 'B') color[2] = changeColor(color[2], .05);

	else if (key == 'p') {
		glutSetWindow(window1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		glutSetWindow(window2);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
	else if (key == 'l') {
		glutSetWindow(window1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glutSetWindow(window2);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else if (key == 'f') {
		glutSetWindow(window1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glutSetWindow(window2);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

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
	std::cout << "Initializing buffer objects" << std::endl;
	initBufferObject();
	std::cout << "Finished initializing" << std::endl;
	return;
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glShadeModel(GL_SMOOTH);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, aspectWidth / aspectHeight, nearClip, farClip);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camPosition.x, camPosition.y, camPosition.z,
		camPosition.x + camForward.x, camPosition.y + camForward.y, camPosition.z + camForward.z,
		camUp.x, camUp.y, camUp.z);
	
	glColor3f(color[0], color[1], color[2]);
	
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

void setProjectionMatrix() {
	float top = nearClip * tan(fov * (M_PI / 360));
	float bottom = -top;
	float right = top * (aspectWidth / aspectHeight);
	float left = -right;
	projectionMat[0] = (2 * nearClip) / (right - left);
	projectionMat[4] = 0;
	projectionMat[8] = (right + left) / (right - left);
	projectionMat[12] = 0;
	projectionMat[1] = 0;
	projectionMat[5] = (2 * nearClip) / (top - bottom);
	projectionMat[9] = (top + bottom) / (top - bottom);
	projectionMat[13] = 0;
	projectionMat[2] = 0;
	projectionMat[6] = 0;
	projectionMat[10] = -(farClip + nearClip) / (farClip - nearClip);
	projectionMat[14] = -(2 * farClip * nearClip) / (farClip - nearClip);
	projectionMat[3] = 0;
	projectionMat[7] = 0;
	projectionMat[11] = -1;
	projectionMat[15] = 0;
}

void setModelViewMatrix() {
	glm::vec3 camSide = cross(camForward, camUp);
	modelViewMat[0] = camSide.x; 
	modelViewMat[4] = camSide.y;
	modelViewMat[8] = camSide.z;
	modelViewMat[12] = -dot(camPosition, camSide);

	modelViewMat[1] = camUp.x;
	modelViewMat[5] = camUp.y;
	modelViewMat[9] = camUp.z;
	modelViewMat[13] = -dot(camPosition, camUp);
	
	modelViewMat[2] = -camForward.x;
	modelViewMat[6] = -camForward.y;
	modelViewMat[10] = -camForward.z;
	modelViewMat[14] = -dot(camPosition, -camForward);
	
	modelViewMat[3] = 0;
	modelViewMat[7] = 0;
	modelViewMat[11] = 0;
	modelViewMat[15] = 1;
}

void printMatricies() {
	std::cout << std::endl << "Model View Matrix" << std::endl;
	for (int i = 0; i < 16; i++) {
		if (i % 4 == 0) std::cout << std::endl;
		std::cout << modelViewMat[i] << " ";
	}
	std::cout << std::endl << "PerspectiveProjection? Matrix" << std::endl;
	for (int i = 0; i < 16; i++) {
		if (i % 4 == 0) std::cout << std::endl;
		std::cout << projectionMat[i] << " ";
	}

}

void newDisplay() {
	glUseProgram(pID);
	//glUseProgram(0);

	glValidateProgram(pID);
	GLint validate = 0;
	glGetProgramiv(pID, GL_VALIDATE_STATUS, &validate);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glShadeModel(GL_SMOOTH);
		
	setModelViewMatrix();
	setProjectionMatrix();

	GLint modeViewMatLocation = glGetUniformLocation(pID, "modelViewMatrix");
	GLint projectionMatLocation = glGetUniformLocation(pID, "projectionMatrix");
	GLint colorVecLocation = glGetUniformLocation(pID, "colorVec");

	glUniformMatrix4fv(modeViewMatLocation, 1, GL_FALSE, modelViewMat);
	glUniformMatrix4fv(projectionMatLocation, 1, GL_FALSE, projectionMat);
	//float colorArray[3] = { red, green, blue };
	glUniform1fv(colorVecLocation, 3, &color[0]);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, faces.size() * 3 , GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glutSwapBuffers();

}

void idle() {
	glutSetWindow(window1);
	glutPostRedisplay();
	glutSetWindow(window2);
	glutPostRedisplay();
}

void newIdle() {
	glutSetWindow(window2);
	glutPostRedisplay();
}

void myReshape(int w, int h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, 1.0f, nearClip, farClip);
	glMatrixMode(GL_MODELVIEW);
}


int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(10, 10);
	glutInitContextFlags(GLUT_COMPATIBILITY_PROFILE);
	window1 = glutCreateWindow("Model Viewer (old)");
	glewInit();
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glutInitWindowPosition(510, 10);
	window2 = glutCreateWindow("Model Viewer (new)");
	setShaders();
	glutReshapeFunc(myReshape);
	glutDisplayFunc(newDisplay);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	loadModel("models/bunny.obj");
	glutMainLoop();
}

