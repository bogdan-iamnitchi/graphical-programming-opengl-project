#include "Camera.hpp"
#include <iostream>
#include <GL/glew.h>

namespace gps
{

    // Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp)
    {
         //init the yaw and pitch angles
        this->yaw = YAW;
        this->pitch = PITCH;
        this->zoom = ZOOM;

        this->worldUpDirection = cameraUp;

        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;

        // TODO - Update the rest of camera parameters
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraUpDirection, cameraFrontDirection));

    }
    
    float Camera::getZoom() {
        return this->zoom;
    }

    // return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix()
    {
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
    }

    // Update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float deltaTime)
    {
        float speed = deltaTime * SPEED;

        switch (direction)
        {
            case MOVE_FORWARD:
                cameraPosition += cameraFrontDirection * speed;
                break;
            case MOVE_BACKWARD:
                cameraPosition -= cameraFrontDirection * speed;
                break;
            case MOVE_LEFT:
                cameraPosition -= cameraRightDirection * speed;
                break;
            case MOVE_RIGHT:
                cameraPosition += cameraRightDirection * speed;
                break;
        }
    }

    // Update the camera internal parameters following a camera rotate event
    // yaw - camera rotation around the y-axis
    // pitch - camera rotation around the x-axis
    // Update the camera internal parameters following a camera rotate event
    // yaw - camera rotation around the y-axis
    // pitch - camera rotation around the x-axis
    void Camera::rotate(float pitch, float yaw)
    {
        glm::vec3 newDirection;

        newDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newDirection.y = sin(glm::radians(pitch));
        newDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        cameraFrontDirection = glm::normalize(newDirection);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, worldUpDirection));
        cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }

    void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) {
        xoffset *= SENSITIVITY;
        yoffset *= SENSITIVITY;

        yaw += xoffset;
        pitch += yoffset;

        if(constrainPitch) {
            if(pitch > 89.0f)
                pitch =  89.0f;
            if(pitch < -89.0f)
                pitch = -89.0f;
        }

        this->rotate(pitch, yaw);
    }

    void Camera::processScrollMovement(float yoffset) {
        zoom -= (float) yoffset;
        if (zoom < 1.0f)
            zoom = 1.0f;
        if (zoom > 45.0f)
            zoom = 45.0f; 
    }
}