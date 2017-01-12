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

#include "Camera.h"


#include "Interaction.h"
#include "Quad.h"

#include <iostream>
#include <math.h>

#ifndef  M_PI
#define M_PI (acos(-1.0))
#endif

const GLint screenWidth = 800, screenHeight = 600;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void Do_Movement();
void Fetch_Around();
void Init();
GLuint loadSkyCubeTexture();
GLint touch_cube_edge(GLint *x, GLint *y, GLint *z);
void place_cube(GLint x, GLint y, GLint z);


// Camera
Camera camera(glm::vec3(20.0f, 8.0f, 20.0f));
bool keys[1024];
bool mouse[8];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLboolean bloom = true;


GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
/*GLint soilIndex;
glm::mat4 soilMat[10000];
glm::vec3 stoneOffset[10000];
glm::vec3 grassOffset[10000];*/
GLfloat diffuse = 0.4f;
GLfloat spec = 0.6f;
CubeType currentCubeOnHand = soil;
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
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, screenWidth, screenHeight);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_MULTISAMPLE);
	//glEnable(GL_FRAMEBUFFER_SRGB);

	// Shader 
	Shader cubeShader("shader/cube.vs", "shader/cube.frag");
	Shader lightShader("shader/cube.vs", "shader/lightbox.frag");
	Shader modelShader("shader/model.vs", "shader/model.frag");
	//Shader hdrShader("shader/hdr.vs", "shader/hdr.frag");
	Shader bloomShader("shader/bloom-hdr.vs", "shader/bloom-hdr.frag");
	Shader blurShader("shader/blur.vs", "shader/blur.frag");
	Shader skyboxShader("shader/sky.vs", "shader/sky.frag");
	//Shader shader("shadow_mapping.vs", "shadow_mapping.frag");
	Shader simpleDepthShader("shader/shadow-mapping-depth.vs", "shader/shadow-mapping-depth.frag");
	//Shader debugDepthQuad("debug_quad.vs", "debug_quad_depth.frag");

	Model steveModel("model/steve.obj");

	bloomShader.Use();
	glUniform1i(glGetUniformLocation(bloomShader.Program, "scene"), 0);
	glUniform1i(glGetUniformLocation(bloomShader.Program, "bloomBlur"), 1);

	// Configure depth map FBO
	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// - Create depth texture
	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



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

	GLuint boleVBO, boleVAO;
	glGenVertexArrays(1, &boleVAO);
	glGenBuffers(1, &boleVBO);
	glBindVertexArray(boleVAO);
	glBindBuffer(GL_ARRAY_BUFFER, boleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boleCubeVertices), boleCubeVertices, GL_STATIC_DRAW);
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

	GLuint leafVBO, leafVAO;
	glGenVertexArrays(1, &leafVAO);
	glGenBuffers(1, &leafVBO);
	glBindVertexArray(leafVAO);
	glBindBuffer(GL_ARRAY_BUFFER, leafVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(leafCubeVertices), leafCubeVertices, GL_STATIC_DRAW);
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

	// 天空盒 VAO
	GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	// 加载纹理
	GLuint cubeTexture;
	glGenTextures(1, &cubeTexture);
	glBindTexture(GL_TEXTURE_2D, cubeTexture); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	GLint imageWidth, imageHeight;
	unsigned char* image = SOIL_load_image("texture/texture.png", &imageWidth, &imageHeight, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
	//天空盒纹理
	GLuint skycubeTexture = loadSkyCubeTexture();

	// hdr & bloom
	// Set up floating point framebuffer to render scene to
	GLuint hdrFBO;
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	// - Create 2 floating point color buffers (1 for normal rendering, other for brightness treshold values)
	GLuint colorBuffers[2];
	glGenTextures(2, colorBuffers);
	for (GLuint i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // We clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}
	// - Create and attach depth buffer (renderbuffer)
	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	// - Finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Ping pong framebuffer for blurring
	GLuint pingpongFBO[2];
	GLuint pingpongColorbuffers[2];
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorbuffers);
	for (GLuint i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // We clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
		// Also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}

	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	while (!glfwWindowShouldClose(window)){
		glm::mat4 view;
		glm::mat4 projection;
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//fps
		// cout << 1 / deltaTime << endl;
		// Check and call events
		glfwPollEvents();
		Do_Movement();
		Fetch_Around();
		LBA_handle();


		glm::vec3 lightPos = glm::vec3(20.0f, 25.0f, 10.0f);
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		GLfloat near_plane = 0.1f, far_plane = 37.5f;
		lightProjection = glm::ortho(-30.0f, 30.0f, -30.0f, 0.0f, near_plane, far_plane);
		//lightProjection = glm::perspective(45.0f, (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // Note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene.
		lightView = glm::lookAt(lightPos, glm::vec3(20.0f,5.0f,25.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// - now render scene from light's point of view
		simpleDepthShader.Use();
		glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

		GLint modelLoc = glGetUniformLocation(simpleDepthShader.Program, "model");
		//GLint modelAdjustLoc = glGetUniformLocation(cubeShader.Program, "modelAdjust");
		GLint viewLoc = glGetUniformLocation(simpleDepthShader.Program, "view");
		GLint projLoc = glGetUniformLocation(simpleDepthShader.Program, "projection");

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
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
						//soilMat[soilIndex++] = model;
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
					else if (cubeAttribute[i][j][k] == bole) {
						glBindVertexArray(boleVAO);
						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
						glDrawArrays(GL_TRIANGLES, 0, 36);
						glBindVertexArray(0);
					}
					else if (cubeAttribute[i][j][k] == leaf) {
						glBindVertexArray(leafVAO);
						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
						glDrawArrays(GL_TRIANGLES, 0, 36);
						glBindVertexArray(0);
					}
				}
			}
		}
		glm::mat4 stevePosModels;
		GLint steveY;
		for (GLint i = 0; i < WORLDHEIGHT; i++)
		{
			if (cubeAttribute[50][50][i] == air)
			{
				steveY = i;
				break;
			}
		}
		stevePosModels = glm::translate(stevePosModels, glm::vec3(20.0f, steveY*CUBESIZE*2.0f - 0.2f, 20.0f));
		//stevePosModel = glm::translate(stevePosModel, glm::vec3(-1.0f, -CUBESIZE, -1.0f));
		stevePosModels = glm::scale(stevePosModels, glm::vec3(0.245f, 0.245f, 0.245f));
		stevePosModels = glm::translate(stevePosModels, glm::vec3(0.0f, -0.032011f, 0.0f));
		stevePosModels = glm::rotate(stevePosModels, glm::radians(-camera.Yaw + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(stevePosModels));
		glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		steveModel.Draw(simpleDepthShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2. Render scene as normal 
		glViewport(0, 0, screenWidth, screenHeight);



		// 绑定 HDR 帧缓冲
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		// Clear the colorbuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// draw
		cubeShader.Use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		glUniform1i(glGetUniformLocation(cubeShader.Program, "material.diffuse"), 0);
		//临时采用相同的光照贴图
		glUniform1i(glGetUniformLocation(cubeShader.Program, "material.specular"), 0);

		glUniform3fv(glGetUniformLocation(cubeShader.Program, "lightPos"), 1, &lightPos[0]);
		//glUniform3fv(glGetUniformLocation(cubeShader.Program, "viewPos"), 1, &camera.Position[0]);
		glUniformMatrix4fv(glGetUniformLocation(cubeShader.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
		// Enable/Disable shadows by pressing 'SPACE'
		//glUniform1i(glGetUniformLocation(cubeShader.Program, "shadows"), shadows);
		glUniform1i(glGetUniformLocation(cubeShader.Program, "shadowMap"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)screenWidth/(float)screenHeight, 0.1f, 1000.0f);

        // Get the uniform locations
        modelLoc = glGetUniformLocation(cubeShader.Program, "model");
		//GLint modelAdjustLoc = glGetUniformLocation(cubeShader.Program, "modelAdjust");
		viewLoc = glGetUniformLocation(cubeShader.Program, "view");
        projLoc = glGetUniformLocation(cubeShader.Program, "projection"); 

        // Pass the matrices to the shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		
		glUniform3f(glGetUniformLocation(cubeShader.Program, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		glUniform1f(glGetUniformLocation(cubeShader.Program, "material.shininess"), 32.0f);

		glUniform3f(glGetUniformLocation(cubeShader.Program, "dirLight.direction"), 0.0f, -20.0f, -15.0f);
		glUniform3f(glGetUniformLocation(cubeShader.Program, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(cubeShader.Program, "dirLight.diffuse"), diffuse, diffuse, diffuse);
		glUniform3f(glGetUniformLocation(cubeShader.Program, "dirLight.specular"), spec, spec, spec);

		glUniform3f(glGetUniformLocation(cubeShader.Program, "pointLights[0].position"), 25.0f, 8.0f, 20.0f);
		glUniform3f(glGetUniformLocation(cubeShader.Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(cubeShader.Program, "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(cubeShader.Program, "pointLights[0].specular"), 5.0f, 5.0f, 5.0f);
		glUniform1f(glGetUniformLocation(cubeShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(cubeShader.Program, "pointLights[0].linear"), 0.09);
		glUniform1f(glGetUniformLocation(cubeShader.Program, "pointLights[0].quadratic"), 0.032);
		glUniform1i(glGetUniformLocation(cubeShader.Program, "point_lights"), 1);
		//soilIndex = 0;
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
						//soilMat[soilIndex++] = model;
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
					else if (cubeAttribute[i][j][k] == bole) {
						glBindVertexArray(boleVAO);
						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
						glDrawArrays(GL_TRIANGLES, 0, 36);
						glBindVertexArray(0);
					}
					else if (cubeAttribute[i][j][k] == leaf) {
						glBindVertexArray(leafVAO);
						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
						glDrawArrays(GL_TRIANGLES, 0, 36);
						glBindVertexArray(0);
					}
				}
			}
		}

		//modelShader.Use();
		
		//glUniform3f(glGetUniformLocation(modelShader.Program, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		//stevePosModel = glm::translate(stevePosModel, glm::vec3(0.0f, -CUBESIZE - 0.032011f*0.225f, 0.0f));
		
		/*
		cout << camera.Position.x <<" ";
		cout << camera.Position.y << " ";
		cout << camera.Position.z << "\n";
		*/
		/*
		stevePosModel = glm::translate(stevePosModel, glm::vec3(camera.Position.x, camera.Position.y - cameraHeight, camera.Position.z) +
			normalize(glm::vec3(camera.Front.x, 0, camera.Front.z)));
		stevePosModel = glm::rotate(stevePosModel, glm::radians(-camera.Yaw + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		stevePosModel = glm::scale(stevePosModel, glm::vec3(0.225f, 0.225f, 0.225f));
		stevePosModel = glm::translate(stevePosModel, glm::vec3(camera.Position.x+3.06801f, camera.Position.y- 0.645863f, camera.Position.z-0.03818f));
		stevePosModel = glm::translate(stevePosModel, glm::vec3(0.0f, -2 * CUBESIZE, 0.0f));
		stevePosModel = glm::translate(stevePosModel, glm::vec3(-1.0f, -CUBESIZE, -1.0f));
		stevePosModel = glm::translate(stevePosModel, glm::vec3(0.0f,- 0.032011f, 0.0f));
		*/
		glm::mat4 stevePosModel;
		for (GLint i=0; i < WORLDHEIGHT; i++)
		{
			if (cubeAttribute[50][50][i] == air)
			{
				steveY = i;
				break;
			}
		}
		stevePosModel = glm::translate(stevePosModel, glm::vec3(20.0f, steveY*CUBESIZE*2.0f-0.2f, 20.0f));
		//stevePosModel = glm::translate(stevePosModel, glm::vec3(-1.0f, -CUBESIZE, -1.0f));
		stevePosModel = glm::scale(stevePosModel, glm::vec3(0.245f, 0.245f, 0.245f));
		stevePosModel = glm::translate(stevePosModel, glm::vec3(0.0f, -0.032011f, 0.0f));
		stevePosModel = glm::rotate(stevePosModel, glm::radians(-camera.Yaw + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(stevePosModel));
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		steveModel.Draw(modelShader);
		cubeShader.Use();
		glm::mat4 tempModel;
		tempModel = glm::translate(tempModel, glm::vec3(25.0f, 8.0f, 20.0f));
		GLfloat test=1.0f;
		lightShader.Use();
		tempModel = glm::scale(tempModel, glm::vec3(0.5f, 0.5f, 0.5f));
		//tempModel = glm::translate(tempModel, glm::vec3(0.0f, CUBESIZE/2, 0.0f));
		glUniform3f(glGetUniformLocation(lightShader.Program, "lightColor"), 5.0f,5.0f,5.0f);
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(tempModel));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		RenderCube();
		//glBindVertexArray(grassVAO);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		//glBindVertexArray(0);

		// 天空盒
		glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(skyboxShader.Program, "skybox"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skycubeTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // Set depth function back to default

		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// 2. Blur bright fragments w/ two-pass Gaussian Blur 
		GLboolean horizontal = true, first_iteration = true;
		GLuint amount = 10;
		blurShader.Use();
		for (GLuint i = 0; i < amount; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
			glUniform1i(glGetUniformLocation(blurShader.Program, "horizontal"), horizontal);
			glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
			RenderQuad();
			horizontal = !horizontal;
			if (first_iteration)
				first_iteration = false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2. Now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		bloomShader.Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
		//glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
		
		GLfloat exposure = 1.0f;
		glUniform1i(glGetUniformLocation(bloomShader.Program, "bloom"), bloom);
		glUniform1f(glGetUniformLocation(bloomShader.Program, "exposure"), exposure);
		RenderQuad();


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

void Fetch_Around()
{
	return;
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
	//if (keys[GLFW_KEY_SPACE])
		//camera.ProcessKeyboard(JUMP, deltaTime);
	camera.ProcessFloated(deltaTime);
}

GLfloat offsetC = 1.0f;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
	{
		if (camera.Mode == NORMAL_MODE)
		{
			camera.Mode = GOD_MODE;
		}
		else
		{
			camera.Mode = NORMAL_MODE;
		}
	}
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		spec *= 1.5f;
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		spec *= 0.5f;
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		diffuse *= 1.5f;
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		diffuse *= 0.5f;
	}
	if (key == GLFW_KEY_SPACE&&action == GLFW_PRESS)
	{
		JUMPING = true;
	}
	if (key == GLFW_KEY_SPACE&&action == GLFW_RELEASE)
	{
		JUMPING = false;
	}
	if (key == GLFW_KEY_F2&&action == GLFW_PRESS)
	{
		bloom = !bloom;
	}
	if (key == GLFW_KEY_ENTER&&action == GLFW_PRESS)
	{
		currentCubeOnHand = CubeType((currentCubeOnHand + 1) % 6);
		if (currentCubeOnHand == air)
		{
			currentCubeOnHand = soil;
		}
	}
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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	GLint x, y, z;
	if (action == GLFW_PRESS)
	{
		mouse[button] = true;
	}
	else
	{
		mouse[button] = false;
	}
	if (button == GLFW_MOUSE_BUTTON_2&&action == GLFW_PRESS)
	{
		if (touch_cube_edge(&x, &y, &z))
		{
			place_cube(x, y, z);
		}
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void Init() {
	initCubeAttribute();
}


void UpdateDynamicItem(CubeType item)
{
	return;
}

GLint touch_cube(GLint *x, GLint *y, GLint *z)
{
	glm::vec3 temp = camera.Position;
	temp += camera.Right*0.09375f;
	cout << "offset" << offsetC << " ";
	GLint i;
	for (i = 1; i <= TOUCH_MAX; i++)
	{
		temp += camera.Front*TOUCH_STEP;
		*x = temp.x / CUBESIZE / 2.0f;
		*y = temp.y / CUBESIZE / 2.0f;
		*z = temp.z / CUBESIZE / 2.0f;
		if (!canMoveIn(cubeAttribute[*x][*z][*y]))
		{
			break;
		}
	}
	if (i == TOUCH_MAX + 1)
	{
		return 0;
	}
	return i - 1;
}

GLint touch_cube_edge(GLint *x, GLint *y, GLint *z)
{
	glm::vec3 temp = camera.Position;
	temp += camera.Right*0.09375f;
	cout << "offset" << offsetC << " ";
	GLint i;
	for (i = 1; i <= TOUCH_MAX; i++)
	{
		temp += camera.Front*TOUCH_STEP;
		*x = temp.x / CUBESIZE / 2.0f;
		*y = temp.y / CUBESIZE / 2.0f;
		*z = temp.z / CUBESIZE / 2.0f;
		if (!canMoveIn(cubeAttribute[*x][*z][*y]))
		{
			break;
		}
	}
	if (i == TOUCH_MAX + 1)
	{
		return 0;
	}
	temp -= camera.Front*TOUCH_STEP;
	*x = temp.x / CUBESIZE / 2.0f;
	*y = temp.y / CUBESIZE / 2.0f;
	*z = temp.z / CUBESIZE / 2.0f;
	return i - 1;
}

void digged_cube(GLint x, GLint y, GLint z)
{
	UpdateDynamicItem(cubeAttribute[x][z][y]);
	cubeAttribute[x][z][y] = air;
	return;
}


void place_cube(GLint x, GLint y, GLint z)
{
	cubeAttribute[x][z][y] = currentCubeOnHand;
	cout << currentCubeOnHand<<"placed!";
	//cubeAttribute[x][y][z] = stone;
}

void LBA_handle()
{
	GLint x, y, z;
	GLint times;
	glm::vec3 temp = camera.Position;
	
	cout << left_button_state << " ";
	cout << LBA_hold_time << " ";
	switch (left_button_state)
	{
	case LBA_FREE:
		if (mouse[0])
		{
			if (touch_cube(&x, &y, &z))
			{
				left_button_state = LBA_DIGGING;
				last_x = x;
				last_y = y;
				last_z = z;
			}
		}
		LBA_hold_time = 0.0f;
		break;
	case LBA_DIGGING:
		if (!mouse[0])
		{
			left_button_state = LBA_FREE;
			LBA_hold_time = 0.0f;
		}
		else if (touch_cube(&x, &y, &z))
		{
			if (last_x == x&&last_y == y&&last_z == z)
			{
				LBA_hold_time += deltaTime;
				if (LBA_hold_time >= 1.0f)
				{
					digged_cube(x, y, z);
					left_button_state = LBA_FREE;
					LBA_hold_time = 0.0f;
				}
			}
		}
		break;
	case LBA_PLACE:
		times = touch_cube(&x, &y, &z);
		if (times)
		{
			temp += times*TOUCH_STEP;
			x = temp.x / CUBESIZE / 2.0f;
			y = temp.y / CUBESIZE / 2.0f;
			z = temp.z / CUBESIZE / 2.0f;
			cubeAttribute[x][z][y] = active_item;
		}
		break;
	default:
		left_button_state = LBA_FREE;
		LBA_hold_time = 0.0f;
		break;
	}
}

GLuint loadSkyCubeTexture()
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	//glActiveTexture(GL_TEXTURE0);

	int width, height;
	unsigned char* image;

	vector<const GLchar*> faces;
	faces.push_back("texture/skybox/right.png");
	faces.push_back("texture/skybox/left.png");
	faces.push_back("texture/skybox/top.png");
	faces.push_back("texture/skybox/bottom.png");
	faces.push_back("texture/skybox/back.png");
	faces.push_back("texture/skybox/front.png");


	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}
