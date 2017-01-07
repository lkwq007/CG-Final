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
const GLfloat GRAVITY = 2.0f;

bool onGround=false;
GLfloat dropSpeed = 0.0f;

bool collisionDetector(glm::vec3 position)
{
	int x, y, z;
	bool result;
	position = position / CUBESIZE/ 2.0f;
	x = position.x;
	y = position.y;
	z = position.z;
	return !(canMoveIn(cubeAttribute[x][z][y])&&canMoveIn(cubeAttribute[x][z][y - 1]));
}

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

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
	{
		this->Position = position;
		this->WorldUp = up;
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->updateCameraVectors();
	}
	// Constructor with scalar values
	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
	{
		this->Position = glm::vec3(posX, posY, posZ);
		this->WorldUp = glm::vec3(upX, upY, upZ);
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->updateCameraVectors();
	}

	// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
	}

	void ProcessFloated(GLfloat deltaTime)
	{
		int x, y, z, yUpdated,i;
		GLfloat temp;
		x = this->Position.x / CUBESIZE / 2.0f;
		y = this->Position.y / CUBESIZE / 2.0f;
		z = this->Position.z / CUBESIZE / 2.0f;
		if (y < 2)
		{
			onGround = true;
			return;
		}
		cout << cubeAttribute[x][z][y-2]<< cubeAttribute[x][z][y-1]<< cubeAttribute[x][z][y]<<" ";
		if (canMoveIn(cubeAttribute[x][z][y-2]))
		{
			onGround = false;
		}
		else
		{
			onGround = true;
			this->Position.y = (y + 0.8)*CUBESIZE*2.0f;
			dropSpeed = 0.0f;
		}
		
		if (onGround == false)
		{
			dropSpeed += deltaTime*GRAVITY;
			temp = this->Position.y - dropSpeed*deltaTime;
			yUpdated = temp / CUBESIZE / 2.0f;
			if (yUpdated < 1)
			{
				yUpdated = 1;
			}
			for (i = y; i >= yUpdated-1; i--)
			{
				if (!canMoveIn(cubeAttribute[x][z][i]))
				{
					break;
				}
			}
			if (i == yUpdated - 2)
			{
				this->Position.y = temp;
			}
			else
			{
				this->Position.y = (i + 2.8)*CUBESIZE*2.0f;
				onGround = true;
				dropSpeed = 0.0f;
			}
		}
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
	{
		GLfloat velocity = this->MovementSpeed * deltaTime;
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