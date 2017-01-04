#pragma once
// GLEW
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// SOIL
#include <SOIL.h>
// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <time.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

#define WORLDWIDTH	100
#define WORLDLENGTH	100
#define WORLDHEIGHT	20
#define CUBESIZE	0.2f
#define FILTERSIZE	7

enum CubeType{
	air,
	soil,
	grass,
	stone
};

CubeType cubeAttribute[WORLDWIDTH][WORLDLENGTH][WORLDHEIGHT];

GLfloat cubeVertices[] = {
	// Back face
	-CUBESIZE, -CUBESIZE, -CUBESIZE,  0.0f, 0.0f, // Bottom-left
	CUBESIZE,  CUBESIZE, -CUBESIZE,  1.0f, 1.0f, // top-right
	CUBESIZE, -CUBESIZE, -CUBESIZE,  1.0f, 0.0f, // bottom-right         
	CUBESIZE,  CUBESIZE, -CUBESIZE,  1.0f, 1.0f, // top-right
	-CUBESIZE, -CUBESIZE, -CUBESIZE,  0.0f, 0.0f, // bottom-left
	-CUBESIZE,  CUBESIZE, -CUBESIZE,  0.0f, 1.0f, // top-left
									  // Front face
	-CUBESIZE, -CUBESIZE,  CUBESIZE,  0.0f, 0.0f, // bottom-left
	CUBESIZE, -CUBESIZE,  CUBESIZE,  1.0f, 0.0f, // bottom-right
	CUBESIZE,  CUBESIZE,  CUBESIZE,  1.0f, 1.0f, // top-right
	CUBESIZE,  CUBESIZE,  CUBESIZE,  1.0f, 1.0f, // top-right
	-CUBESIZE,  CUBESIZE,  CUBESIZE,  0.0f, 1.0f, // top-left
	-CUBESIZE, -CUBESIZE,  CUBESIZE,  0.0f, 0.0f, // bottom-left
									  // Left face
	-CUBESIZE,  CUBESIZE,  CUBESIZE,  1.0f, 0.0f, // top-right
	-CUBESIZE,  CUBESIZE, -CUBESIZE,  1.0f, 1.0f, // top-left
	-CUBESIZE, -CUBESIZE, -CUBESIZE,  0.0f, 1.0f, // bottom-left
	-CUBESIZE, -CUBESIZE, -CUBESIZE,  0.0f, 1.0f, // bottom-left
	-CUBESIZE, -CUBESIZE,  CUBESIZE,  0.0f, 0.0f, // bottom-right
	-CUBESIZE,  CUBESIZE,  CUBESIZE,  1.0f, 0.0f, // top-right
									  // Right face
	CUBESIZE,  CUBESIZE,  CUBESIZE,  1.0f, 0.0f, // top-left
	CUBESIZE, -CUBESIZE, -CUBESIZE,  0.0f, 1.0f, // bottom-right
	CUBESIZE,  CUBESIZE, -CUBESIZE,  1.0f, 1.0f, // top-right         
	CUBESIZE, -CUBESIZE, -CUBESIZE,  0.0f, 1.0f, // bottom-right
	CUBESIZE,  CUBESIZE,  CUBESIZE,  1.0f, 0.0f, // top-left
	CUBESIZE, -CUBESIZE,  CUBESIZE,  0.0f, 0.0f, // bottom-left     
									 // Bottom face
	-CUBESIZE, -CUBESIZE, -CUBESIZE,  0.0f, 1.0f, // top-right
	CUBESIZE, -CUBESIZE, -CUBESIZE,  1.0f, 1.0f, // top-left
	CUBESIZE, -CUBESIZE,  CUBESIZE,  1.0f, 0.0f, // bottom-left
	CUBESIZE, -CUBESIZE,  CUBESIZE,  1.0f, 0.0f, // bottom-left
	-CUBESIZE, -CUBESIZE,  CUBESIZE,  0.0f, 0.0f, // bottom-right
	-CUBESIZE, -CUBESIZE, -CUBESIZE,  0.0f, 1.0f, // top-right
									  // Top face
	-CUBESIZE,  CUBESIZE, -CUBESIZE,  0.0f, 1.0f, // top-left
	CUBESIZE,  CUBESIZE,  CUBESIZE,  1.0f, 0.0f, // bottom-right
	CUBESIZE,  CUBESIZE, -CUBESIZE,  1.0f, 1.0f, // top-right     
	CUBESIZE,  CUBESIZE,  CUBESIZE,  1.0f, 0.0f, // bottom-right
	-CUBESIZE,  CUBESIZE, -CUBESIZE,  0.0f, 1.0f, // top-left
	-CUBESIZE,  CUBESIZE,  CUBESIZE,  0.0f, 0.0f  // bottom-left        
};

//glm::vec3 cubePositions[WORLDWIDTH][WORLDLENGTH][WORLDHEIGHT];
int verticeNumber = 0;
GLfloat allCubeVertices[WORLDWIDTH * WORLDLENGTH * WORLDHEIGHT * 36 * 4];
void averageFilter(int altitudeRandom[WORLDWIDTH + FILTERSIZE - 1][WORLDLENGTH + FILTERSIZE - 1], int altitudeAverage[WORLDWIDTH][WORLDLENGTH]) {
	for (int i = 0; i < WORLDWIDTH; i++) {
		for (int j = 0; j < WORLDLENGTH; j++) {
			int altitudeSum = 0;
			for (int k = 0; k < FILTERSIZE; k++) {
				for (int l = 0; l < FILTERSIZE; l++) {
					altitudeSum += altitudeRandom[i + k][j + l];
				}
			}
			altitudeAverage[i][j] = altitudeSum / (FILTERSIZE*FILTERSIZE);
		}
	}
	return;
}

void initCubeAttribute(void) {
	int altitudeRandom[WORLDWIDTH + FILTERSIZE - 1][WORLDLENGTH + FILTERSIZE - 1];
	int altitudeAverage[WORLDWIDTH][WORLDLENGTH];
	memset(cubeAttribute, air, sizeof(cubeAttribute));
	srand(time(NULL));
	for (int i = 0; i < WORLDWIDTH + FILTERSIZE - 1; i++) {
		for (int j = 0; j < WORLDLENGTH + FILTERSIZE - 1; j++) {
			if (i % ((FILTERSIZE - 1) / 2) && j % ((FILTERSIZE - 1) / 2)) {
				altitudeRandom[i][j] = rand() % WORLDHEIGHT;
			}
			else {
				altitudeRandom[i][j] = WORLDHEIGHT / 2;
			}
		}
	}
	averageFilter(altitudeRandom, altitudeAverage);
	for (int i = 0; i < WORLDWIDTH; i++) {
		for (int j = 0; j < WORLDLENGTH; j++) {
			//cubePositions[cubeNumber++] = glm::vec3((float)i*CUBESIZE * 2, (float)altitudeAverage[i][j] *CUBESIZE * 2, (float)j*CUBESIZE * 2);
			for (int k = 0; k < altitudeAverage[i][j]; k++){
				cubeAttribute[i][j][k] = soil;
			}
		}
	}
}