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
#define FILTERSIZE	5
#define LIGHTPOSX	50
#define LIGHTPOSY	50
#define LIGHTPOSZ	50
#define TREENUM		64

#define MAX(x,y)	((x<y)?y:x)

enum CubeType{
	air,
	soil,
	grass,
	stone,
	bole,
	leaf
};
bool canMoveIn(CubeType type)
{
	switch (type)
	{
	case air:
		return true;
	default:
		return false;
	}
}
bool canDig(CubeType type)
{
	switch (type)
	{
	case air:
		return false;
	default:
		return true;
	}
}
CubeType cubeAttribute[WORLDWIDTH][WORLDLENGTH][WORLDHEIGHT];
glm::vec3 lightPos = glm::vec3(LIGHTPOSX * CUBESIZE * 2, LIGHTPOSY * CUBESIZE * 2, LIGHTPOSZ * CUBESIZE * 2);
GLfloat cubeVertices[] = {
	// 面								法线					纹理
										// Back face
	-CUBESIZE, -CUBESIZE, -CUBESIZE,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, // Bottom-left
	CUBESIZE,  CUBESIZE, -CUBESIZE,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, // top-right
	CUBESIZE, -CUBESIZE, -CUBESIZE,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f, // bottom-right         
	CUBESIZE,  CUBESIZE, -CUBESIZE,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, // top-right
	-CUBESIZE, -CUBESIZE, -CUBESIZE,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, // bottom-left
	-CUBESIZE,  CUBESIZE, -CUBESIZE,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f, // top-left
									  // Front face
	-CUBESIZE, -CUBESIZE,  CUBESIZE,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f, // bottom-left
	CUBESIZE, -CUBESIZE,  CUBESIZE,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f, // bottom-right
	CUBESIZE,  CUBESIZE,  CUBESIZE,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f, // top-right
	CUBESIZE,  CUBESIZE,  CUBESIZE,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f, // top-right
	-CUBESIZE,  CUBESIZE,  CUBESIZE,  0.0f,  0.0f, 1.0f,  0.0f, 1.0f, // top-left
	-CUBESIZE, -CUBESIZE,  CUBESIZE,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f, // bottom-left
									  // Left face
	-CUBESIZE,  CUBESIZE,  CUBESIZE, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f, // top-right
	-CUBESIZE,  CUBESIZE, -CUBESIZE, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f, // top-left
	-CUBESIZE, -CUBESIZE, -CUBESIZE, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f, // bottom-left
	-CUBESIZE, -CUBESIZE, -CUBESIZE, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f, // bottom-left
	-CUBESIZE, -CUBESIZE,  CUBESIZE, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f, // bottom-right
	-CUBESIZE,  CUBESIZE,  CUBESIZE, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f, // top-right
									  // Right face
	CUBESIZE,  CUBESIZE,  CUBESIZE,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f, // top-left
	CUBESIZE, -CUBESIZE, -CUBESIZE,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, // botetom-right
	CUBESIZE,  CUBESIZE, -CUBESIZE,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, // top-right         
	CUBESIZE, -CUBESIZE, -CUBESIZE,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, // bottom-right
	CUBESIZE,  CUBESIZE,  CUBESIZE,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f, // top-left
	CUBESIZE, -CUBESIZE,  CUBESIZE,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, // bottom-left     
									 // Bottom face
	-CUBESIZE, -CUBESIZE, -CUBESIZE,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f, // top-right
	CUBESIZE, -CUBESIZE, -CUBESIZE,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f, // top-left
	CUBESIZE, -CUBESIZE,  CUBESIZE,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, // bottom-left
	CUBESIZE, -CUBESIZE,  CUBESIZE,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, // bottom-left
	-CUBESIZE, -CUBESIZE,  CUBESIZE,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f, // bottom-right
	-CUBESIZE, -CUBESIZE, -CUBESIZE,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f, // top-right
									  // Top face
	-CUBESIZE,  CUBESIZE, -CUBESIZE,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, // top-left
	CUBESIZE,  CUBESIZE,  CUBESIZE,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, // bottom-right
	CUBESIZE,  CUBESIZE, -CUBESIZE,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f, // top-right     
	CUBESIZE,  CUBESIZE,  CUBESIZE,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, // bottom-right
	-CUBESIZE,  CUBESIZE, -CUBESIZE,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, // top-left
	-CUBESIZE,  CUBESIZE,  CUBESIZE,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f  // bottom-left        
};
GLfloat skyboxVertices[] = {
	// Positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f,  1.0f
};

GLfloat soilCubeVertices[sizeof(cubeVertices) / sizeof(GLfloat)];
GLfloat stoneCubeVertices[sizeof(cubeVertices) / sizeof(GLfloat)];
GLfloat grassCubeVertices[sizeof(cubeVertices) / sizeof(GLfloat)];
GLfloat boleCubeVertices[TREENUM * 8 * 10];
GLfloat leafCubeVertices[TREENUM * 8 * 10 * 50];

//glm::vec3 cubePositions[WORLDWIDTH][WORLDLENGTH][WORLDHEIGHT];
//int verticeNumber = 0;
//GLfloat allCubeVertices[WORLDWIDTH * WORLDLENGTH * WORLDHEIGHT * 36 * 4];
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
void genLeaf(int i, int j, int attribute, int r, bool first) {
	if (r == 0) {
		return;
	}
	if (!first && !(cubeAttribute[i][j][attribute] == leaf || cubeAttribute[i][j][attribute] == air)) {
		return;
	}
	if (!first) {
		cubeAttribute[i][j][attribute] = leaf;
	}
	genLeaf(i + 1, j, attribute, r - 1, false);
	genLeaf(i - 1, j, attribute, r - 1, false);
	genLeaf(i, j + 1, attribute, r - 1, false);
	genLeaf(i, j - 1, attribute, r - 1, false);
	return;
}
int genBole(int i, int j, int attribute, float probability, int treeHeight) {
	if (rand() % 100 < probability * 100 && probability > 0.03) {
		cubeAttribute[i][j][attribute] = bole;
	}
	else {
		return treeHeight;
	}
	return genBole(i, j, attribute + 1, probability * 0.8, treeHeight + 1);
}

void initCubeAttribute(void) {
	int altitudeRandom[WORLDWIDTH + FILTERSIZE - 1][WORLDLENGTH + FILTERSIZE - 1];
	int altitudeAverage[WORLDWIDTH][WORLDLENGTH];
	int treePostion[TREENUM];
	memset(cubeAttribute, air, sizeof(cubeAttribute));
	srand(time(NULL));

	// 生成位置
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
	for (int i = 0; i < TREENUM; i++) {// 树的位置，用一个int表示
		treePostion[i] = (rand() % WORLDWIDTH) * WORLDLENGTH + rand() % WORLDLENGTH;
		for (int j = 0; j < i; j++) {
			if (treePostion[i] == treePostion[j] + 1 - WORLDLENGTH || treePostion[i] == treePostion[j] - WORLDLENGTH || treePostion[i] == treePostion[j] - 1 - WORLDLENGTH
				|| treePostion[i] == treePostion[j] + 1 || treePostion[i] == treePostion[j] || treePostion[i] == treePostion[j] - 1
				|| treePostion[i] == treePostion[j] + 1 + WORLDLENGTH || treePostion[i] == treePostion[j] + WORLDLENGTH || treePostion[i] == treePostion[j] - 1 + WORLDLENGTH) {
				i--;
				break;
			}
		}
	}
	averageFilter(altitudeRandom, altitudeAverage);
	for (int i = 0; i < WORLDWIDTH; i++) {
		for (int j = 0; j < WORLDLENGTH; j++) {
			//cubePositions[cubeNumber++] = glm::vec3((float)i*CUBESIZE * 2, (float)altitudeAverage[i][j] *CUBESIZE * 2, (float)j*CUBESIZE * 2);
			for (int k = 0; k < MAX(altitudeAverage[i][j] - 3, 0); k++){
				cubeAttribute[i][j][k] = stone;
			}
			for (int k = MAX(altitudeAverage[i][j] - 3, 0); k < MAX(altitudeAverage[i][j] - 1, 0); k++) {
				cubeAttribute[i][j][k] = soil;
			}
			cubeAttribute[i][j][altitudeAverage[i][j] - 1] = grass;
		}
	}
	for (int i = 0; i < TREENUM; i++) {
		int x, y;
		x = treePostion[i] / WORLDLENGTH;
		y = treePostion[i] % WORLDLENGTH;
		if (x + 3 >= WORLDWIDTH || y + 3 >= WORLDLENGTH || x - 3 < 0 || y - 3 < 0){
			continue;
		}
		int attribute = altitudeAverage[x][y];
		cubeAttribute[x][y][attribute] = bole;
		cubeAttribute[x][y][attribute + 1] = bole;
		cubeAttribute[x][y][attribute + 2] = bole;
		cubeAttribute[x][y][attribute + 3] = bole;
		int treeHeight = genBole(x, y, attribute + 4, 1 - 0.0001, 3);
		for (int j = 2; j <= treeHeight; j++) {
			if (treeHeight - 2 < 4) {
				genLeaf(x, y, attribute + j, treeHeight - j + 1, true);
			}
			else {
				genLeaf(x, y, attribute + j, ((treeHeight - j) * 2 + 0.5) / (treeHeight - 2) + 2, true);
			}
		}
		cubeAttribute[x][y][attribute + treeHeight] = leaf;
	}

	// 生成顶点
	//soil
	memcpy(soilCubeVertices, cubeVertices, sizeof(cubeVertices));
	for (int l = 0; l < 36; l++) {
		soilCubeVertices[l * 8 + 6] *= 1.0 / 16 / 16 * 12;
		soilCubeVertices[l * 8 + 6] += (6 * 16 + 3) * 1.0 / 256;
		soilCubeVertices[l * 8 + 7] *= 1.0 / 16 / 16 * 12;
		soilCubeVertices[l * 8 + 7] += (15 * 16 + 3) * 1.0 / 256;
	}
	//stone
	memcpy(stoneCubeVertices, cubeVertices, sizeof(cubeVertices));
	for (int l = 0; l < 36; l++) {
		stoneCubeVertices[l * 8 + 6] *= 1.0 / 16 / 16 * 12;
		stoneCubeVertices[l * 8 + 6] += (5 * 16 + 3) * 1.0 / 256;
		stoneCubeVertices[l * 8 + 7] *= 1.0 / 16 / 16 * 12;
		stoneCubeVertices[l * 8 + 7] += (15 * 16 + 3) * 1.0 / 256;
	}
	//grass
	memcpy(grassCubeVertices, cubeVertices, sizeof(cubeVertices));
	for (int l = 0; l < 24; l++) {
		grassCubeVertices[l * 8 + 6] *= 1.0 / 16 / 16 * 12;
		grassCubeVertices[l * 8 + 6] += 0;//(1 * 16 + 3) * 1.0 / 256;
		grassCubeVertices[l * 8 + 7] *= 1.0 / 16 / 16 * 12;
		grassCubeVertices[l * 8 + 7] = 1.0 / 16 / 16 * 12 - grassCubeVertices[l * 8 + 7];
		grassCubeVertices[l * 8 + 7] += (14 * 16 + 3) * 1.0 / 256;
	}
	for (int l = 24; l < 30; l++) {
		grassCubeVertices[l * 8 + 6] *= 1.0 / 16 / 16 * 12;
		grassCubeVertices[l * 8 + 6] += 0;//(5 * 16 + 3) * 1.0 / 256;
		grassCubeVertices[l * 8 + 7] *= 1.0 / 16 / 16 * 12;
		grassCubeVertices[l * 8 + 7] += (14 * 16 + 3) * 1.0 / 256;
	}
	for (int l = 30; l < 36; l++) {
		grassCubeVertices[l * 8 + 6] *= 1.0 / 16 / 16 * 12;
		grassCubeVertices[l * 8 + 6] += 0;//(5 * 16 + 3) * 1.0 / 256;
		grassCubeVertices[l * 8 + 7] *= 1.0 / 16 / 16 * 12;
		grassCubeVertices[l * 8 + 7] += (13 * 16 + 3) * 1.0 / 256;
	}
	//tree leave
	memcpy(leafCubeVertices, cubeVertices, sizeof(cubeVertices));
	for (int l = 0; l < 36; l++) {
		leafCubeVertices[l * 8 + 6] *= 1.0 / 16 / 16 * 12;
		leafCubeVertices[l * 8 + 6] += (14 * 16 + 3) * 1.0 / 256;
		leafCubeVertices[l * 8 + 7] *= 1.0 / 16 / 16 * 12;
		leafCubeVertices[l * 8 + 7] += (15 * 16 + 3) * 1.0 / 256;
	}
	//tree bole
	memcpy(boleCubeVertices, cubeVertices, sizeof(cubeVertices));
	for (int l = 0; l < 36; l++) {
		boleCubeVertices[l * 8 + 6] *= 1.0 / 16 / 16 * 12;
		boleCubeVertices[l * 8 + 6] += (4 * 16 + 3) * 1.0 / 256;
		boleCubeVertices[l * 8 + 7] *= 1.0 / 16 / 16 * 12;
		boleCubeVertices[l * 8 + 7] += (14 * 16 + 3) * 1.0 / 256;
	}
}
