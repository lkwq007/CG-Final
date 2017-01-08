#pragma once

// Std. Includes
#include <vector>

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	JUMP
};

// Default camera values
const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 1.0f;
const GLfloat SENSITIVTY = 0.25f;
const GLfloat ZOOM = 45.0f;
const GLfloat GRAVITY = CUBESIZE*6.4f;

// camera box half size
GLfloat cameraHWidth = CUBESIZE*0.4;
GLfloat cameraHTick = CUBESIZE*0.2;
GLfloat cameraHeight = 1.65*CUBESIZE*2.0;
GLfloat playerHeight = 1.8*CUBESIZE*2.0;

// camera mode
enum Camera_Mode
{
	NORMAL_MODE,
	GOD_MODE
};

bool onGround = false;
bool JUMPING = false;
Camera_Movement jumpDir=FORWARD;
Camera_Movement lastDir=FORWARD;
GLfloat dropSpeed = 0.0f;

bool collisionDetector(glm::vec3 position)
{
	int x, y, z;
	position = position / CUBESIZE/ 2.0f;
	x = position.x;
	y = position.y;
	z = position.z;
	return !(canMoveIn(cubeAttribute[x][z][y])&&canMoveIn(cubeAttribute[x][z][y - 1]));
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

bool canDoMove(glm::vec3 nextPos, glm::vec3 frontV, glm::vec3 rightV)
{
	nextPos.y -= cameraHeight;
	if (sglCollDetr(nextPos + rightV+frontV) || sglCollDetr(nextPos - rightV + frontV) || sglCollDetr(nextPos + rightV - frontV) ||
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

glm::vec3 collisionCorrector(glm::vec3 curPos, glm::vec3 nextPos, glm::vec3 frontV,glm::vec3 rightV)
{
	frontV = cameraHTick*frontV;
	rightV = cameraHWidth*rightV;
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
// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// Eular Angles
	GLfloat Yaw;
	GLfloat Pitch;
	// Camera options
	GLfloat MovementSpeed;
	GLfloat MouseSensitivity;
	GLfloat Zoom;
	Camera_Mode Mode;

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
	{
		this->Position = position;
		this->WorldUp = up;
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->Mode = NORMAL_MODE;
		this->updateCameraVectors();
	}
	// Constructor with scalar values
	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
	{
		this->Position = glm::vec3(posX, posY, posZ);
		this->WorldUp = glm::vec3(upX, upY, upZ);
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->Mode = NORMAL_MODE;
		this->updateCameraVectors();
	}

	// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
	}
	GLint floatedTime;
	void ProcessFloated(GLfloat deltaTime)
	{
		if (this->Mode == GOD_MODE)
		{
			return;
		}
		glm::vec3 frontXZ = this->Front;
		frontXZ.y = 0;
		frontXZ = glm::normalize(frontXZ);
		glm::vec3 rightXZ = this->Right;
		rightXZ.y = 0;
		rightXZ = glm::normalize(rightXZ);
		int x, y, z, yUpdated,i;
		glm::vec3 tempPos=this->Position;
		x = this->Position.x / CUBESIZE / 2.0f;
		y = this->Position.y / CUBESIZE / 2.0f;
		z = this->Position.z / CUBESIZE / 2.0f;
		/*
		if (y < 2)
		{
			onGround = true;
			this->Position.y = cameraHeight;
			return;
		}
		*/
		//cout << cubeAttribute[x][z][y-2]<< cubeAttribute[x][z][y-1]<< cubeAttribute[x][z][y]<<" ";
		if (canMoveIn(cubeAttribute[x][z][y-2]))
		{
			onGround = false;
		}
		else
		{
			onGround = true;
			this->Position.y = y*CUBESIZE*2.0f+cameraHeight;
			dropSpeed = 0.0f;
		}
		tempPos = this->Position;
		if (onGround == false)
		{
			//this->Position.x = x*CUBESIZE*2.0f + CUBESIZE;
			//this->Position.z = z*CUBESIZE*2.0f + CUBESIZE;
			if (minY < 0)
			{
				while (canMoveIn(cubeAttribute[x][z][y - 2])&& canMoveIn(cubeAttribute[x][z][y - 1])&& canMoveIn(cubeAttribute[x][z][y]))
				{
					y--;
				}
				minY = y ;
			}
			tempPos.y=-dropSpeed*deltaTime- deltaTime*deltaTime*GRAVITY/2.0f;
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

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
	{
		GLfloat velocity = this->MovementSpeed * deltaTime;
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
		glm::vec3 frontXZ = this->Front;
		frontXZ.y = 0;
		frontXZ = glm::normalize(frontXZ);
		glm::vec3 rightXZ = this->Right;
		rightXZ.y = 0;
		rightXZ = glm::normalize(rightXZ);
		glm::vec3 nextPosition;
		/*
		if (JUMPING&&direction!=JUMP)
		{
			jumpDir = direction;
			JUMPING = false;
		}
		*/
		if (onGround)//认为地面阻力无限大，不考虑动量
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
				JUMPING = true;
				//onGround = false;
				dropSpeed = -CUBESIZE*5.0f;
				nextPosition.y += CUBESIZE*3.0f;
				
			}
			
			this->Position=collisionCorrector(this->Position, nextPosition, frontXZ, rightXZ);
		}
		lastDir = direction;
		return;
		/*
		if (onGround)
		{
			glm::vec3 prevPositon = this->Position;
			if (direction == FORWARD)
				this->Position += this->Front * velocity;
			if (direction == BACKWARD)
				this->Position -= this->Front * velocity;
			if (direction == LEFT)
				this->Position -= this->Right * velocity;
			if (direction == RIGHT)
				this->Position += this->Right * velocity;
			this->Position.y = prevPositon.y;
			if (direction == JUMP)
			{
				this->Position.y += CUBESIZE*3.0f;
				this->Position.x += (this->Position.x - prevPositon.x) * 2;
				this->Position.z += (this->Position.z - prevPositon.z) * 2;
			}
			if (collisionDetector(Position))
			{
				this->Position = prevPositon;
			}
		}
		*/
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= this->MouseSensitivity;
		yoffset *= this->MouseSensitivity;

		this->Yaw += xoffset;
		this->Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (this->Pitch > 89.0f)
				this->Pitch = 89.0f;
			if (this->Pitch < -89.0f)
				this->Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Eular angles
		this->updateCameraVectors();
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
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
	// Calculates the front vector from the Camera's (updated) Eular Angles
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		front.y = sin(glm::radians(this->Pitch));
		front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		this->Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		this->Up = glm::normalize(glm::cross(this->Right, this->Front));
	}
};