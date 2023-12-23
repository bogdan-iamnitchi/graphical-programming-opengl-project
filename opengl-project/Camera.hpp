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
        MOVE_LEFT,
        MOVE_UP,
        MOVE_DOWN
    };

    const float YAW = 90.0f;
    const float PITCH = -15.94f; // -15.94f
    const float SPEED = 5.5f;
    const float SENSITIVITY = 0.025f;
    const float ZOOM = 45.0f;

    class Camera
    {

    public:
        // Camera constructor
        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp, float speed=SPEED, float yaw=YAW, float pitch=PITCH, float zoom=ZOOM);
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

        void setSpeed(float speed);
        void setPitch(float pitch);
        void setYaw(float yaw);

        void setPosition(glm::vec3 position);
        void setTarget(glm::vec3 target);

        glm::vec3 getPosition();
        glm::vec3 getFront();

    private:
        float speed;
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
