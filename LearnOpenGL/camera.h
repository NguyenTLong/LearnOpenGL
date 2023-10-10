#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
using namespace glm;
enum Camera_Movement
{

	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float	YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = .1f;
const float ZOOM = 80.0f;

class Camera
{

public:
	vec3 Position;
	vec3 Front;
	vec3 Up;
	vec3 Right;
	vec3 WorldUp;

	float Yaw;
	float Pitch;

	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	Camera(vec3 position = vec3(.0f, .0f, .0f), vec3 up = vec3(.0f, 1.0f, .0f), float yaw = YAW, float pitch = PITCH) : Front(vec3(.0f, .0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	mat4 GetViewMatrix()
	{
		return lookAt(Position, Position + Front, Up);
	}

	void ProcessKeyboard(Camera_Movement direction, float deltatime)
	{
		float velocity = MovementSpeed * deltatime;
		switch (direction)
		{
		case FORWARD:
			Position += Front * velocity;
			break;
		case BACKWARD:
			Position -= Front * velocity;
			break;
		case LEFT:
			Position -= Right * velocity;
			break;
		case RIGHT:
			Position += Right * velocity;
			break;
		}
	}
	
	void ProcessMouseMovement(float xOffSet, float yOffset, GLboolean constrainPitch = true)
	{
		xOffSet *= MouseSensitivity;
		yOffset *= MouseSensitivity;

		Yaw += xOffSet;
		Pitch += yOffset;

		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}
		
		updateCameraVectors();

	}

	void ProccessMouseScroll(float yOffset)
	{
		Zoom -= yOffset;
		if (Zoom < 1.0f)
			Zoom = 1.0f;
		if (Zoom > 90.0f)
			Zoom = 90.0f;
	}

private:
	void updateCameraVectors()
	{
		vec3 front;
		front.x = cos(radians(Yaw)) * cos(radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(radians(Yaw)) * cos(radians(Pitch));
		Front = normalize(front);

		Right = normalize(cross(Front, WorldUp));
		Up = normalize(cross(Right, Front));
	}

};

#endif // !CAMERA_H
