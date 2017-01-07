// custom DEBUG flag
#define CAMERAFREE
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// SOIL
#include <SOIL.h>
// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// custom
#include "CubeData.h"
#include "Shader.h"
#include "Model.h"
#ifdef CAMERAFREE
#include "CameraFree.h"
#else
#include "Camera.h"
#endif // CAMERAFREE

#include <iostream>

int screenWidth = 800, screenHeight = 600;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();
void Init();

// Camera
Camera camera(glm::vec3(-1.0f, 0.0f, -1.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// The MAIN function, from here we start our application and run our Game loop
int main(){
	// Init some data
	Init();

	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "CG_Final", nullptr, nullptr); // Windowed
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	//glfwSetScrollCallback(window, scroll_callback);
	
	// Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, screenWidth, screenHeight);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


	// Shader
	Shader cubeShader("cube.vs", "cube.frag");
	Shader modelShader("model.vs", "model.frag");
	Model steveModel("model/steve.obj");
	GLuint soilVBO, soilVAO;
	glGenVertexArrays(1, &soilVAO);
	glGenBuffers(1, &soilVBO);
	glBindVertexArray(soilVAO);
	glBindBuffer(GL_ARRAY_BUFFER, soilVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(soilCubeVertices), soilCubeVertices, GL_STATIC_DRAW);
	// 位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// 法线
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// 纹理
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	GLuint stoneVBO, stoneVAO;
	glGenVertexArrays(1, &stoneVAO);
	glGenBuffers(1, &stoneVBO);
	glBindVertexArray(stoneVAO);
	glBindBuffer(GL_ARRAY_BUFFER, stoneVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(stoneCubeVertices), stoneCubeVertices, GL_STATIC_DRAW);
	// 位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// 法线
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// 纹理
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	GLuint grassVBO, grassVAO;
	glGenVertexArrays(1, &grassVAO);
	glGenBuffers(1, &grassVBO);
	glBindVertexArray(grassVAO);
	glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grassCubeVertices), grassCubeVertices, GL_STATIC_DRAW);
	// 位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// 法线
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// 纹理
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	// 加载纹理
	GLuint cubeTexture;
	glGenTextures(1, &cubeTexture);
	glBindTexture(GL_TEXTURE_2D, cubeTexture); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	unsigned char* image = SOIL_load_image("texture.png", &screenWidth, &screenHeight, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	while (!glfwWindowShouldClose(window)){
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//fps
		// cout << 1 / deltaTime << endl;
		// Check and call events
		glfwPollEvents();
		Do_Movement();

		// Clear the colorbuffer
		glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// draw
		cubeShader.Use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		glUniform1i(glGetUniformLocation(cubeShader.Program, "material.diffuse"), 0);
		//临时采用相同的光照贴图
		glUniform1i(glGetUniformLocation(cubeShader.Program, "material.specular"), 0);


		glm::mat4 view;
        view = camera.GetViewMatrix();
        glm::mat4 projection;	
        projection = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight, 0.1f, 1000.0f);

        // Get the uniform locations
        GLint modelLoc = glGetUniformLocation(cubeShader.Program, "model");
		//GLint modelAdjustLoc = glGetUniformLocation(cubeShader.Program, "modelAdjust");
		GLint viewLoc = glGetUniformLocation(cubeShader.Program, "view");
        GLint projLoc = glGetUniformLocation(cubeShader.Program, "projection"); 

        // Pass the matrices to the shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glUniform1f(glGetUniformLocation(cubeShader.Program, "material.shininess"), 32.0f);

		glUniform3f(glGetUniformLocation(cubeShader.Program, "dirLight.direction"), 0.2f, -1.0f, 0.3f);
		glUniform3f(glGetUniformLocation(cubeShader.Program, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(cubeShader.Program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
		glUniform3f(glGetUniformLocation(cubeShader.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);

		for (int i = 0; i < WORLDWIDTH; i++) {
			for (int j = 0; j < WORLDLENGTH; j++) {
				for (int k = 0; k < WORLDHEIGHT; k++) {
					if (cubeAttribute[i][j][k] == air) {
						continue;
					}
					glm::mat4 model;
					model = glm::translate(model, glm::vec3((float)i*CUBESIZE * 2, (float)k * CUBESIZE * 2, (float)j*CUBESIZE * 2));
					//glm::mat3 modelAdjust = glm::mat3(transpose(inverse(model)));
					//glUniformMatrix4fv(modelAdjustLoc, 1, GL_FALSE, glm::value_ptr(modelAdjust));
					if (cubeAttribute[i][j][k] == soil) {
						glBindVertexArray(soilVAO);
						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
						glDrawArrays(GL_TRIANGLES, 0, 36);
						glBindVertexArray(0);
					}
					else if (cubeAttribute[i][j][k] == stone) {
						glBindVertexArray(stoneVAO);
						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
						glDrawArrays(GL_TRIANGLES, 0, 36);
						glBindVertexArray(0);
					}
					else if (cubeAttribute[i][j][k] == grass) {
						glBindVertexArray(grassVAO);
						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
						glDrawArrays(GL_TRIANGLES, 0, 36);
						glBindVertexArray(0);
					}
				}
			}
		}
		modelShader.Use();
		glm::mat4 stevePosModel;
		stevePosModel = glm::translate(stevePosModel, glm::vec3(-2.0f, -CUBESIZE - 0.032011f*0.225f, -1.0f));
		stevePosModel = glm::scale(stevePosModel, glm::vec3(0.225f, 0.225f, 0.225f));
		stevePosModel = glm::translate(stevePosModel, glm::vec3(0.12f, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(stevePosModel));
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		steveModel.Draw(modelShader);
		cubeShader.Use();
		glm::mat4 tempModel;
		tempModel = glm::translate(tempModel, glm::vec3(-2.0f, 0.0f, -1.0f));
		GLfloat test=1.0f;

		tempModel = glm::scale(tempModel, glm::vec3(1.0f, test, 1.0f));
		//tempModel = glm::translate(tempModel, glm::vec3(0.0f, CUBESIZE/2, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(cubeShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(tempModel));
		glUniformMatrix4fv(glGetUniformLocation(cubeShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(cubeShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glBindVertexArray(grassVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

        // Swap the buffers
        glfwSwapBuffers(window);
    }
    // Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &soilVAO);
	glDeleteBuffers(1, &soilVBO);
	glDeleteVertexArrays(1, &stoneVAO);
	glDeleteBuffers(1, &stoneVBO);
	glfwTerminate();
	return 0;
}

void Do_Movement(){
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
#ifndef CAMERAFREE
	if (keys[GLFW_KEY_SPACE])
		camera.ProcessKeyboard(JUMP, deltaTime);
	camera.ProcessFloated(deltaTime);
#endif // CAMERAFREE
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024){
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
	if (firstMouse){
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void Init() {
	initCubeAttribute();
}