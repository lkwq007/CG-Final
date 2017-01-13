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
// 自定义
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

// 函数定义
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




// 全局变量
GLboolean bloom = true;
GLfloat exposure = 1.0f;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat diffuse = 0.4f;
GLfloat spec = 0.6f;
CubeType currentCubeOnHand = soil;

int main(){
	// 初始化方块数据
	Init();

	// 初始化GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "CG_Final", nullptr, nullptr); // Windowed
	glfwMakeContextCurrent(window);

	// 设定回调函数
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// 隐藏鼠标
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// 初始化GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	//初始化视口
	glViewport(0, 0, screenWidth, screenHeight);
	glEnable(GL_DEPTH_TEST);		//启用深度测试
	glEnable(GL_CULL_FACE);			//启用面剔除
	glCullFace(GL_BACK);
	glEnable(GL_MULTISAMPLE);

	// 加载各类Shader 
	Shader cubeShader("shader/cube.vs", "shader/cube.frag");
	Shader lightShader("shader/cube.vs", "shader/lightbox.frag");
	Shader modelShader("shader/model.vs", "shader/model.frag");
	Shader bloomShader("shader/bloom-hdr.vs", "shader/bloom-hdr.frag");
	Shader blurShader("shader/blur.vs", "shader/blur.frag");
	Shader skyboxShader("shader/sky.vs", "shader/sky.frag");
	Shader simpleDepthShader("shader/shadow-mapping-depth.vs", "shader/shadow-mapping-depth.frag");

	//加载人物模块
	Model steveModel("model/steve.obj");
	//高光着色器
	bloomShader.Use();
	glUniform1i(glGetUniformLocation(bloomShader.Program, "scene"), 0);
	glUniform1i(glGetUniformLocation(bloomShader.Program, "bloomBlur"), 1);

	// 深度映射
	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// 创建深度贴图
	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	// 初始化深度贴图
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	//绑定帧缓冲进行设置
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//加载方块VAO
	//土方块
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
	
	//石方块
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

	// 草方块
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

	//树干方块
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

	//叶属性 
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

	// HDR和高光
	// 设置HDR帧缓冲
	GLuint hdrFBO;
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	// 创建2个颜色缓冲，一个普通一个用于高亮度
	GLuint colorBuffers[2];
	glGenTextures(2, colorBuffers);
	for (GLuint i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// 绑定纹理到帧缓冲
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}
	// 创建并绑定深度缓冲
	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	// 检查帧缓冲是否完整
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Ping pong framebuffer
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
		// 检查帧缓冲是否完整
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "Framebuffer not complete!" << std::endl;
	}

	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	while (!glfwWindowShouldClose(window)){
		glm::mat4 view;
		glm::mat4 projection;
		//设置帧时钟
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//事件检测
		glfwPollEvents();
		Do_Movement();
		Fetch_Around();
		LBA_handle();

		// 初始化阴影光源的属性
		glm::vec3 lightPos = glm::vec3(20.0f, 25.0f, 10.0f);
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		GLfloat near_plane = 0.1f, far_plane = 47.5f;
		lightProjection = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(20.0f,5.0f,25.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// 从光的视角渲染物体（为了渲染阴影）
		simpleDepthShader.Use();
		glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
		//获得着色器Uniform变量的位置
		GLint modelLoc = glGetUniformLocation(simpleDepthShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(simpleDepthShader.Program, "view");
		GLint projLoc = glGetUniformLocation(simpleDepthShader.Program, "projection");
		//切换视口
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		//绑定深度帧缓冲
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		for (int i = 0; i < WORLDWIDTH; i++) {
			for (int j = 0; j < WORLDLENGTH; j++) {
				for (int k = 0; k < WORLDHEIGHT; k++) {
					if (cubeAttribute[i][j][k] == air) {
						continue;
					}
					//model矩阵
					glm::mat4 model;
					model = glm::translate(model, glm::vec3((float)i*CUBESIZE * 2, (float)k * CUBESIZE * 2, (float)j*CUBESIZE * 2));
					//判断方块类型，并绘制相应方块
					//土方块
					if (cubeAttribute[i][j][k] == soil) {
						glBindVertexArray(soilVAO);
						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
						glDrawArrays(GL_TRIANGLES, 0, 36);
						glBindVertexArray(0);

					}
					//石方块
					else if (cubeAttribute[i][j][k] == stone) {
						glBindVertexArray(stoneVAO);
						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
						glDrawArrays(GL_TRIANGLES, 0, 36);
						glBindVertexArray(0);
					}
					//草方块
					else if (cubeAttribute[i][j][k] == grass) {
						glBindVertexArray(grassVAO);
						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
						glDrawArrays(GL_TRIANGLES, 0, 36);
						glBindVertexArray(0);
					}
					//树干方块
					else if (cubeAttribute[i][j][k] == bole) {
						glBindVertexArray(boleVAO);
						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
						glDrawArrays(GL_TRIANGLES, 0, 36);
						glBindVertexArray(0);
					}
					//叶方块
					else if (cubeAttribute[i][j][k] == leaf) {
						glBindVertexArray(leafVAO);
						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
						glDrawArrays(GL_TRIANGLES, 0, 36);
						glBindVertexArray(0);
					}
				}
			}
		}
		//绘制史蒂夫，导入外部OBJ
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
		//利用平移、缩放、旋转矩阵对模型进行变化
		stevePosModels = glm::translate(stevePosModels, glm::vec3(20.0f, steveY*CUBESIZE*2.0f - 0.2f, 20.0f));
		stevePosModels = glm::scale(stevePosModels, glm::vec3(0.245f, 0.245f, 0.245f));
		stevePosModels = glm::translate(stevePosModels, glm::vec3(0.0f, -0.032011f, 0.0f));
		stevePosModels = glm::rotate(stevePosModels, glm::radians(-camera.Yaw + 270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(stevePosModels));
		glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		steveModel.Draw(simpleDepthShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//正常渲染物体 
		glViewport(0, 0, screenWidth, screenHeight);

		//绑定帧缓冲
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		//清颜色缓冲
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//绘制方块
		cubeShader.Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		glUniform1i(glGetUniformLocation(cubeShader.Program, "material.diffuse"), 0);
		//临时采用相同的光照贴图
		glUniform1i(glGetUniformLocation(cubeShader.Program, "material.specular"), 0);
		//渲染阴影
		glUniform3fv(glGetUniformLocation(cubeShader.Program, "lightPos"), 1, &lightPos[0]);
		glUniformMatrix4fv(glGetUniformLocation(cubeShader.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
		glUniform1i(glGetUniformLocation(cubeShader.Program, "shadowMap"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)screenWidth/(float)screenHeight, 0.1f, 1000.0f);

        // 获得uniform变量位置
        modelLoc = glGetUniformLocation(cubeShader.Program, "model");
		viewLoc = glGetUniformLocation(cubeShader.Program, "view");
        projLoc = glGetUniformLocation(cubeShader.Program, "projection"); 

        //向着色器传输uniform
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		
		glUniform3f(glGetUniformLocation(cubeShader.Program, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		glUniform1f(glGetUniformLocation(cubeShader.Program, "material.shininess"), 32.0f);

		glUniform3f(glGetUniformLocation(cubeShader.Program, "dirLight.direction"), 0.2f, -1.0f, 0.3f);
		glUniform3f(glGetUniformLocation(cubeShader.Program, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(cubeShader.Program, "dirLight.diffuse"), diffuse, diffuse, diffuse);
		glUniform3f(glGetUniformLocation(cubeShader.Program, "dirLight.specular"), spec, spec, spec);

		glUniform3f(glGetUniformLocation(cubeShader.Program, "pointLights[0].position"), 25.0f, 8.0f, 20.0f);
		glUniform3f(glGetUniformLocation(cubeShader.Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(cubeShader.Program, "pointLights[0].diffuse"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(cubeShader.Program, "pointLights[0].specular"), 0.4f, 0.4f, 0.4f);
		glUniform1f(glGetUniformLocation(cubeShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(cubeShader.Program, "pointLights[0].linear"), 0.7);
		glUniform1f(glGetUniformLocation(cubeShader.Program, "pointLights[0].quadratic"), 1.8);
		glUniform1i(glGetUniformLocation(cubeShader.Program, "point_lights"), 1);
		
		for (int i = 0; i < WORLDWIDTH; i++) {
			for (int j = 0; j < WORLDLENGTH; j++) {
				for (int k = 0; k < WORLDHEIGHT; k++) {
					if (cubeAttribute[i][j][k] == air) {
						continue;
					}
					//model矩阵
					glm::mat4 model;
					model = glm::translate(model, glm::vec3((float)i*CUBESIZE * 2, (float)k * CUBESIZE * 2, (float)j*CUBESIZE * 2));
					//土方块
					if (cubeAttribute[i][j][k] == soil) {
						glBindVertexArray(soilVAO);
						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
						glDrawArrays(GL_TRIANGLES, 0, 36);
						glBindVertexArray(0);
						
					}
					//石方块
					else if (cubeAttribute[i][j][k] == stone) {
						glBindVertexArray(stoneVAO);
						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
						glDrawArrays(GL_TRIANGLES, 0, 36);
						glBindVertexArray(0);
					}
					//草方块
					else if (cubeAttribute[i][j][k] == grass) {
						glBindVertexArray(grassVAO);
						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
						glDrawArrays(GL_TRIANGLES, 0, 36);
						glBindVertexArray(0);
					}
					//树干方块
					else if (cubeAttribute[i][j][k] == bole) {
						glBindVertexArray(boleVAO);
						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
						glDrawArrays(GL_TRIANGLES, 0, 36);
						glBindVertexArray(0);
					}
					//叶方块
					else if (cubeAttribute[i][j][k] == leaf) {
						glBindVertexArray(leafVAO);
						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
						glDrawArrays(GL_TRIANGLES, 0, 36);
						glBindVertexArray(0);
					}
				}
			}
		}

		//steve模型
		glm::mat4 stevePosModel;
		for (GLint i=0; i < WORLDHEIGHT; i++)
		{
			if (cubeAttribute[50][50][i] == air)
			{
				steveY = i;
				break;
			}
		}
		//利用平移、缩放、旋转矩阵对模型进行变化
		stevePosModel = glm::translate(stevePosModel, glm::vec3(20.0f, steveY*CUBESIZE*2.0f-0.2f, 20.0f));
		stevePosModel = glm::scale(stevePosModel, glm::vec3(0.245f, 0.245f, 0.245f));
		stevePosModel = glm::translate(stevePosModel, glm::vec3(0.0f, -0.032011f, 0.0f));
		stevePosModel = glm::rotate(stevePosModel, glm::radians(-camera.Yaw + 270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
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
		glUniform3f(glGetUniformLocation(lightShader.Program, "lightColor"), 5.0f,5.0f,5.0f);
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(tempModel));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		RenderCube();

		// 天空盒
		glDepthFunc(GL_LEQUAL);  // 改变深度测试函数
		skyboxShader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// 去除平移
		projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(skyboxShader.Program, "skybox"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skycubeTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); //把深度测试函数改回默认值

		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// 高光帧、高斯模糊
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

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		bloomShader.Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
		
		//GLfloat exposure = 1.0f;
		glUniform1i(glGetUniformLocation(bloomShader.Program, "bloom"), bloom);
		glUniform1f(glGetUniformLocation(bloomShader.Program, "exposure"), exposure);
		RenderQuad();


        //交换缓冲
        glfwSwapBuffers(window);
    }
    //释放使用空间
	glDeleteVertexArrays(1, &soilVAO);
	glDeleteBuffers(1, &soilVBO);
	glDeleteVertexArrays(1, &stoneVAO);
	glDeleteBuffers(1, &stoneVBO);
	glDeleteVertexArrays(1, &grassVAO);
	glDeleteBuffers(1, &grassVBO);
	glDeleteVertexArrays(1, &boleVAO);
	glDeleteBuffers(1, &boleVBO);
	glDeleteVertexArrays(1, &leafVAO);
	glDeleteBuffers(1, &leafVBO);
	glfwTerminate();
	return 0;
}

void Fetch_Around()
{
	return;
}

void Do_Movement(){
	// Camera控制
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (keys[GLFW_KEY_Q])
		exposure -= 0.2 * deltaTime;
	else if (keys[GLFW_KEY_E])
		exposure += 0.2 * deltaTime;
	camera.ProcessFloated(deltaTime);
}

GLfloat offsetC = 1.0f;//交互偏移测试值
//按键回调函数
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)//esc退出
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_F1 && action == GLFW_PRESS)//F1上帝模式
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
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)//上下控制反射光
	{
		spec *= 1.5f;
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		spec *= 0.5f;
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)//左右控制散射光
	{
		diffuse *= 1.5f;
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		diffuse *= 0.5f;
	}
	if (key == GLFW_KEY_SPACE&&action == GLFW_PRESS)//空格跳跃
	{
		JUMPING = true;
	}
	if (key == GLFW_KEY_SPACE&&action == GLFW_RELEASE)
	{
		JUMPING = false;
	}
	if (key == GLFW_KEY_F2&&action == GLFW_PRESS)//F2控制高光开关
	{
		bloom = !bloom;
	}
	if (key == GLFW_KEY_ENTER&&action == GLFW_PRESS)//回车换手中方块
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

//鼠标移动回调函数
void mouse_callback(GLFWwindow* window, double xpos, double ypos){
	if (firstMouse){
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);//调用camera的函数进行处理
}

//鼠标按键回调函数
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	GLint x, y, z;
	if (action == GLFW_PRESS)//左键挖方块，右键放方块
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

//滚轮回调函数
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

//初始化方块属性
void Init() {
	initCubeAttribute();
}

//动态更新
void UpdateDynamicItem(CubeType item)
{
	return;
}

//第一个触碰的方块
GLint touch_cube(GLint *x, GLint *y, GLint *z)
{
	glm::vec3 temp = camera.Position;
	temp += camera.Right*0.09375f;
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

//第一个触碰的方块边界
GLint touch_cube_edge(GLint *x, GLint *y, GLint *z)
{
	glm::vec3 temp = camera.Position;
	temp += camera.Right*0.09375f;
	//cout << "offset" << offsetC << " ";
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

//挖方块函数
void digged_cube(GLint x, GLint y, GLint z)
{
	UpdateDynamicItem(cubeAttribute[x][z][y]);
	cubeAttribute[x][z][y] = air;
	return;
}

//放置方块函数
void place_cube(GLint x, GLint y, GLint z)
{
	cubeAttribute[x][z][y] = currentCubeOnHand;
}

//鼠标左键控制函数
void LBA_handle()
{
	GLint x, y, z;
	GLint times;
	glm::vec3 temp = camera.Position;
	
	switch (left_button_state)
	{
	case LBA_FREE://松开
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
	case LBA_DIGGING://挖方块
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
	case LBA_PLACE://放方块
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

//加载天空盒纹理
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
	for (GLuint i = 0; i < faces.size(); i++)//调用6次加载6个面
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
