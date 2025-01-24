#ifndef CAMERA_H
#define CAMERA

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "time.h"

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values

const float YAW = -90.0f;
const float PITCH = 50.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


class Camera {
public:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 WorldUp;
	glm::vec3 Right;

	float Yaw;
	float Pitch;

	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		UpdateCameraVectors();
        
        
	}
	// constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		UpdateCameraVectors();
	}

	glm::mat4 GetViewMatrix() {
		return glm::lookAt(Position, Position + Front, Up);
	}
    
    /*
    glm::mat4 GetProjection()
    {
        
    }
    */
     
    void SetProjection()
    {
        
    }

	void CameraFollow(glm::vec3 targetPosition) {
        double deltaTime = Time::deltaTime;
        glm::vec3 offset =  glm::vec3(0.0f, -2.0f, 0.0f);;

		glm::vec3 desiredPosition = glm::vec3(targetPosition.x, targetPosition.y, this->Position.z);
        desiredPosition += offset;
		float interValue = 2.5f * deltaTime;

		desiredPosition = VectorInterpolation(this->Position, desiredPosition, interValue);
		this->Position = desiredPosition;
	};
    
    

	// processes input received from a mouse input system. Expects the offset value in both the x and y direction.

private:
	void UpdateCameraVectors() {
		glm::vec3 direction;

		direction.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		direction.y = sin(glm::radians(Pitch));
		direction.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        
		Front = glm::normalize(direction);

		Right = -1.0f * glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}

	glm::vec3 VectorInterpolation(glm::vec3 v1, glm::vec3 v2, float t) {
		return (1 - t) * v1 + t * v2;
	};
    
};

#endif // !CAMERA_H

