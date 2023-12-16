#ifndef Camera_hpp
#define Camera_hpp

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace gps
{

    enum MOVE_DIRECTION
    {
        MOVE_FORWARD,
        MOVE_BACKWARD,
        MOVE_RIGHT,
        MOVE_LEFT
    };

    const float YAW = -90.0f;
    const float PITCH = 0.0f;
    const float SPEED = 20.5f;
    const float SENSITIVITY = 0.025f;
    const float ZOOM = 45.0f;

    class Camera
    {

    public:
        // Camera constructor
        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp);
        // return the view matrix, using the glm::lookAt() function
        glm::mat4 getViewMatrix();
        // update the camera internal parameters following a camera move event
        void move(MOVE_DIRECTION direction, float speed);
        // update the camera internal parameters following a camera rotate event
        // yaw - camera rotation around the y axis
        // pitch - camera rotation around the x axis
        void rotate(float pitch, float yaw);

        void processMouseMovement(float xoffset, float yoffset, bool constrainPitch);

        void processScrollMovement(float yoffset);
        float getZoom();

    private:
        float yaw;
        float pitch;
        float zoom;

        glm::vec3 worldUpDirection;

        glm::vec3 cameraPosition;
        glm::vec3 cameraTarget;
        glm::vec3 cameraFrontDirection;
        glm::vec3 cameraRightDirection;
        glm::vec3 cameraUpDirection;
    };
}

#endif /* Camera_hpp */
