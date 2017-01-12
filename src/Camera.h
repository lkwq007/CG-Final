#pragma once

// Std. Includes
#include <vector>

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define CAMERABODYDISTANCE 0.5f

//定义几种相机操作
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	JUMP
};

//相机相关的常数值
const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 1.0f;
const GLfloat SENSITIVTY = 0.25f;
const GLfloat ZOOM = 45.0f;
const GLfloat GRAVITY = CUBESIZE*6.4f;

//相机盒半高
GLfloat cameraHWidth = CUBESIZE*0.4;
GLfloat cameraHTick = CUBESIZE*0.2;
GLfloat cameraHeight = 1.65*CUBESIZE*2.0;
GLfloat playerHeight = 1.8*CUBESIZE*2.0;

//相机模式
enum Camera_Mode
{
	NORMAL_MODE,//普通模式
	GOD_MODE	//上帝模式
};

bool onGround = false;	//在地上与否的标志位
bool JUMPING = false;	//跳起与否的标志位
Camera_Movement jumpDir = FORWARD;
Camera_Movement lastDir = FORWARD;
GLfloat dropSpeed = 0.0f;

//冲突检测
bool collisionDetector(glm::vec3 position)
{
	int x, y, z;
	position = position / CUBESIZE / 2.0f;
	x = position.x;
	y = position.y;
	z = position.z;
	return !(canMoveIn(cubeAttribute[x][z][y]) && canMoveIn(cubeAttribute[x][z][y - 1]));
}

bool sglCollDetr(glm::vec3 position)
{
	int x, y, z;
	position = position / CUBESIZE / 2.0f;
	x = position.x;
	y = position.y;
	z = position.z;
	return !canMoveIn(cubeAttribute[x][z][y]);
}

//判断是否能进入
bool canDoMove(glm::vec3 nextPos, glm::vec3 frontV, glm::vec3 rightV)
{
	nextPos.y -= cameraHeight;
	if (sglCollDetr(nextPos + rightV + frontV) || sglCollDetr(nextPos - rightV + frontV) || sglCollDetr(nextPos + rightV - frontV) ||
		sglCollDetr(nextPos - rightV - frontV))
		return false;
	nextPos.y += playerHeight*0.5f;
	if (sglCollDetr(nextPos + rightV + frontV) || sglCollDetr(nextPos - rightV + frontV) || sglCollDetr(nextPos + rightV - frontV) ||
		sglCollDetr(nextPos - rightV - frontV))
		return false;
	nextPos.y += playerHeight*0.5f;
	if (sglCollDetr(nextPos + rightV + frontV) || sglCollDetr(nextPos - rightV + frontV) || sglCollDetr(nextPos + rightV - frontV) ||
		sglCollDetr(nextPos - rightV - frontV))
		return false;
	return true;
}

//冲突修正
glm::vec3 collisionCorrector(glm::vec3 curPos, glm::vec3 nextPos, glm::vec3 frontV, glm::vec3 rightV)
{
	frontV = cameraHTick*frontV;
	rightV = cameraHWidth*rightV;
	if (JUMPING)
	{
		nextPos.y += CUBESIZE*2.1f;
	}
	for (GLint i = 0; i <= 5; i++)
	{
		if (canDoMove(nextPos, frontV, rightV))
		{
			return nextPos;
		}
		nextPos = curPos + (nextPos - curPos)*0.5f;
	}
	return curPos;
}
GLint minY;
//定义相机类
class Camera
{
public:
	// Camera 属性
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// 欧拉角
	GLfloat Yaw;
	GLfloat Pitch;
	// Camera 选项
	GLfloat MovementSpeed;
	GLfloat MouseSensitivity;
	GLfloat Zoom;
	Camera_Mode Mode;

	// 无参构造函数
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
	{
		this->Position = position;
		this->WorldUp = up;
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->Mode = NORMAL_MODE;
		this->updateCameraVectors();
	}
	// 带参构造函数
	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
	{
		this->Position = glm::vec3(posX, posY, posZ);
		this->WorldUp = glm::vec3(upX, upY, upZ);
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->Mode = NORMAL_MODE;
		this->updateCameraVectors();
	}

	//返回view矩阵
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
	}
	//每过一段时间执行一次
	GLint floatedTime;
	void ProcessFloated(GLfloat deltaTime)
	{
		if (this->Mode == GOD_MODE)//上帝模式
		{
			return;
		}
		//得到方向向量
		glm::vec3 frontXZ = this->Front;
		frontXZ.y = 0;
		frontXZ = glm::normalize(frontXZ);
		glm::vec3 rightXZ = this->Right;
		rightXZ.y = 0;
		rightXZ = glm::normalize(rightXZ);
		int x, y, z, yUpdated, i;
		glm::vec3 tempPos = this->Position;
		x = this->Position.x / CUBESIZE / 2.0f;
		y = this->Position.y / CUBESIZE / 2.0f;
		z = this->Position.z / CUBESIZE / 2.0f;
		
		//判断脚下是否是土地
		if (canMoveIn(cubeAttribute[x][z][y - 2]))
		{
			onGround = false;
		}
		else
		{
			onGround = true;
			this->Position.y = y*CUBESIZE*2.0f + cameraHeight;
			dropSpeed = 0.0f;
		}
		tempPos = this->Position;
		if (onGround == false)
		{
			//找出脚下最近的土地
			if (minY < 0)
			{
				while (canMoveIn(cubeAttribute[x][z][y - 2]) && canMoveIn(cubeAttribute[x][z][y - 1]) && canMoveIn(cubeAttribute[x][z][y]))
				{
					y--;
				}
				if (JUMPING)
				{
					minY = y + 1;
				}
				else
				{
					minY = y;
				}
			}
			//重力下落方程
			tempPos.y = -dropSpeed*deltaTime - deltaTime*deltaTime*GRAVITY / 2.0f;
			dropSpeed += deltaTime*GRAVITY;
			if (tempPos.y < minY*CUBESIZE*2.0f + cameraHeight)
			{
				tempPos.y = minY*CUBESIZE*2.0f + cameraHeight;
				onGround = true;
				dropSpeed = 0.0f;
				minY = -1;
			}
			this->Position = tempPos;
		}
	}

	//键盘函数
	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
	{
		GLfloat velocity = this->MovementSpeed * deltaTime;
		//对不同的操作做出不同反应
		if (this->Mode == GOD_MODE)
		{
			if (direction == FORWARD)
				this->Position += this->Front * velocity;
			if (direction == BACKWARD)
				this->Position -= this->Front * velocity;
			if (direction == LEFT)
				this->Position -= this->Right * velocity;
			if (direction == RIGHT)
				this->Position += this->Right * velocity;
			return;
		}
		//计算下一位置
		glm::vec3 frontXZ = this->Front;
		frontXZ.y = 0;
		frontXZ = glm::normalize(frontXZ);
		glm::vec3 rightXZ = this->Right;
		rightXZ.y = 0;
		rightXZ = glm::normalize(rightXZ);
		glm::vec3 nextPosition;
		
		if (onGround)//地面移动函数
		{
			if (direction == FORWARD)
			{
				nextPosition = this->Position + frontXZ*velocity;
			}
			if (direction == BACKWARD)
			{
				nextPosition = this->Position - frontXZ*velocity;
			}
			if (direction == LEFT)
			{
				nextPosition = this->Position - rightXZ*velocity;
			}
			if (direction == RIGHT)
			{
				nextPosition = this->Position + rightXZ*velocity;
			}
			if (direction == JUMP)
			{
				cout << "jump";
				//JUMPING = true;
				//onGround = false;
				//dropSpeed = -CUBESIZE*5.0f;
				//nextPosition.y += CUBESIZE*3.0f;

			}
			this->Position = collisionCorrector(this->Position, nextPosition, frontXZ, rightXZ);
		}
		lastDir = direction;
		return;
	}

	//鼠标移动函数
	void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= this->MouseSensitivity;
		yoffset *= this->MouseSensitivity;

		this->Yaw += xoffset;
		this->Pitch += yoffset;

		//保证数据范围
		if (constrainPitch)
		{
			if (this->Pitch > 89.0f)
				this->Pitch = 89.0f;
			if (this->Pitch < -89.0f)
				this->Pitch = -89.0f;
		}

		// 更新相机
		this->updateCameraVectors();
	}

	//滚轮函数
	void ProcessMouseScroll(GLfloat yoffset)
	{
		if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
			this->Zoom -= yoffset*0.1f;
		if (this->Zoom <= 1.0f)
			this->Zoom = 1.0f;
		if (this->Zoom >= 45.0f)
			this->Zoom = 45.0f;
	}

private:
	// 计算向量和欧拉角
	void updateCameraVectors()
	{
		//计算front向量
		glm::vec3 front;
		front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		front.y = sin(glm::radians(this->Pitch));
		front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		this->Front = glm::normalize(front);
		//计算right和up向量
		this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  //归一
		this->Up = glm::normalize(glm::cross(this->Right, this->Front));
	}
};