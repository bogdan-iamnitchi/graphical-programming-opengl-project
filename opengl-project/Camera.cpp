#include "Camera.hpp"
#include <iostream>
#include <GL/glew.h>

namespace gps
{

    // Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp, float speed, float yaw, float pitch, float zoom)
    {
         //init the yaw and pitch angles
        this->yaw = yaw;
        this->pitch = pitch;
        this->zoom = zoom;
        this->speed = speed;

        this->worldUpDirection = cameraUp;

        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;

        // TODO - Update the rest of camera parameters
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraUpDirection, cameraFrontDirection));

    }

    glm::vec3 Camera::getPosition() {
        return this->cameraPosition;
    }

    glm::vec3 Camera::getFront() {
        return this->cameraFrontDirection;
    }

    void Camera::setPosition(glm::vec3 position) {
        this->cameraPosition = position;
    }

    void Camera::setTarget(glm::vec3 target) {
        this->cameraTarget = target;
    }

    void Camera::setSpeed(float speed) {
        if(speed > 0.0f) {
            this->speed = speed;
        }
    }

    void Camera::setYaw(float yaw) {
        this->yaw = yaw;
    }

    void Camera::setPitch(float pitch) {
        this->pitch =  pitch;
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
        float actualSpeed = deltaTime * speed;

        switch (direction)
        {
            case MOVE_FORWARD:
                cameraPosition += cameraFrontDirection * actualSpeed;
                break;
            case MOVE_BACKWARD:
                cameraPosition -= cameraFrontDirection * actualSpeed;
                break;
            case MOVE_LEFT:
                cameraPosition -= cameraRightDirection * actualSpeed;
                break;
            case MOVE_RIGHT:
                cameraPosition += cameraRightDirection * actualSpeed;
                break;
            case MOVE_UP:
                cameraPosition += cameraUpDirection * actualSpeed;
                break;
            case MOVE_DOWN:
                cameraPosition -= cameraUpDirection * actualSpeed;
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