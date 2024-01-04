#if defined(__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>                  //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp>   //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp>         //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"

#include <iostream>
#include "SkyBox.hpp"

#include <irrKlang.h>

irrklang::ISoundEngine *engine;
irrklang::ISound *eagleSound;
irrklang::ISound *backgroundSound;

// WINDOW---------------------------------------------------------------
gps::Window myWindow;
int glWindowWidth = 1280;
int glWindowHeight = 720;

// mTRANSFORM MATRICES--------------------------------------------------
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// STATIC SCENE---------------------------------------------------------
gps::Model3D castele;
gps::Model3D cladiri;
gps::Model3D map;
gps::Model3D village;

// SKY BOX
gps::SkyBox mySkyBox;

// INTERACTION OBJECTS-------------------------------------------
gps::Model3D vulturi;
gps::Model3D barca;
gps::Model3D bilaDragon_stanga;
gps::Model3D bilaDragon_dreapta;
gps::Model3D eliceMoara;

// PLAYER
gps::Model3D warrior;
gps::Model3D shield;
gps::Model3D sulita;

// LIGHTS--------------------------------------------------------
gps::Model3D luminaCastel;
gps::Model3D luminaInsula;

// SHADERS-------------------------------------------------
// shadows
gps::Shader myBasicShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;
gps::Shader skyboxShader;

// scene
gps::Shader staticSceneShader;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;

// FOR SHADOW MAPPING --------------------------------------
gps::Model3D screenQuad;
GLuint shadowMapFBO;
GLuint depthMapTexture;

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;
bool showDepthMap = false;

// FOR COMPUTING LIGHT --------------------------------------
glm::vec3 lightDir;
glm::mat4 lightRotation;

// CONTROLS---------------------------------------------------
GLboolean pressedKeys[1024];
GLfloat angleY = 0.0f;
GLfloat lightAngle = 0.0f;

int activateDirLight = 1;
int activatePointLight = 1;
int activateShadows = 1;
int activateFog = 0;

// ANIMATION-------------------------------------------------
float circleRadius = 90.0f;
float centerX = 40.0f;
float centerZ = 120.0f;
float angularSpeed = 0.4f;
bool startCameraAnimation = false;

float boatX = 0.0f;
float boatSpeed = 5.0f;
bool rotateBoat = false;
bool startBoatAnimation = false;

float angleVulturi = 0.0f;
float vulturiSpeed = 50.0f;
bool startVulturiAnimation = false;

float angleElice = 0.0f;
float eliceSpeed = 50.0f;
bool startEliceAnimation = true;

float bilaStangaZ = 0.0f;
float bilaDreaptaZ = 0.0f;
float bilaSpeed = 10.0f;
bool startBilaStangaAnimation = false;
bool startBilaDreaptaAnimation = false;

bool showWeapons = false;
float angleSulita = 0.0f;
float shieldX = 0.0f;
float shieldSpeed = 0.1f;
float sulitaSpeed = 55.0f;
bool startArmAnimation = false;
bool startDisarmAnimation = false;

// POSITIONS-------------------------------------------------
glm::vec3 warriorPos = glm::vec3(-7.69f, 32.7451f, 31.7f);
glm::vec3 shieldPos = glm::vec3(-7.55356f, 32.7138f, 31.9708f);
glm::vec3 sulitaPos = glm::vec3(-7.85784f, 32.7352f, 31.9592f);

glm::vec3 boatPos = glm::vec3(-72.3844f, 18.7286f, 57.7933f);
glm::vec3 vulturiPos = glm::vec3(-7.11769f, 36.4749f, 133.062f);
glm::vec3 elicePos = glm::vec3(26.0779f, 34.0945f, 84.8142f);

glm::vec3 bilaStangaPos = glm::vec3(1.37323f, 39.1904f, 133.922f);
glm::vec3 bilaDreaptaPos = glm::vec3(-16.4747f, 39.1904f, 134.764f);

// lights
glm::vec3 luminaWarPos = glm::vec3(warriorPos.x, warriorPos.y + 1.0f, warriorPos.z + 1.0f);
glm::vec3 luminaCastelPos = glm::vec3(-7.3179f, 56.166f, 163.15f);
glm::vec3 luminaInsulaPos = glm::vec3(-102.221f, 39.92f, 124.985f);

// CAMERA-------------------------------------------------
float cameraSpeed = 8.5f;

gps::Camera myCamera(
    glm::vec3(warriorPos.x, warriorPos.y + 1.0f, warriorPos.z - 3.5f), // position
    glm::vec3(warriorPos.x, warriorPos.y + 1.0f, warriorPos.z + 3.5f), // target
    glm::vec3(0.0f, 1.0f, 0.0f),                                       // up
    cameraSpeed);

float lastX = glWindowWidth / 2.0f;
float lastY = glWindowHeight / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

bool showCursor = false;

//-----------------------------------SOUNDS---------------------------------------------------------

void initSoundEngine()
{
    engine = irrklang::createIrrKlangDevice();
    if (!engine)
        return; // error starting up the engine
}

void playEagleSound()
{
    eagleSound = engine->play2D("sounds/eagle_sound.wav", GL_TRUE);
}

void playBackgroundSound()
{
    backgroundSound = engine->play2D("sounds/background_sound.wav", GL_TRUE);
}

void stopBackgroundSound()
{
    engine->stopAllSounds();
}

void playDragonSound()
{
    engine->play2D("sounds/dragon_fire.wav", GL_FALSE, GL_FALSE, GL_TRUE);
}

void playPirateSound()
{
    engine->play2D("sounds/pirate_sound.wav", GL_TRUE);
}

void playWeaponSound()
{
    engine->play2D("sounds/weapon_sound.wav", GL_FALSE, GL_FALSE, GL_TRUE);
}

//-------------------------------------------------------------------------------------------------

GLenum glCheckError_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow *window, int width, int height)
{
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
    // TODO
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos)
{

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    myCamera.processMouseMovement(xoffset, yoffset, true);

    // update view matrix
    view = myCamera.getViewMatrix();
}

void scrollCallback(GLFWwindow *window, double xpos, double ypos)
{
    myCamera.processScrollMovement(ypos);

    // update projection matrix
    projection = glm::perspective(glm::radians(myCamera.getZoom()),
                                  (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
                                  0.1f, 1000.0f);
}

void keyboardCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    //glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS)
    {
        showCursor = !showCursor;
        if (showCursor) {
            glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }

    if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
    {
        showDepthMap = !showDepthMap;
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        startCameraAnimation = !startCameraAnimation;
        if (startCameraAnimation == false)
        {
            myCamera.setSpeed(25.0f);
        }
    }
    if (key == GLFW_KEY_KP_8 && action == GLFW_PRESS)
    {
        startBoatAnimation = !startBoatAnimation;

        if (startBoatAnimation == true)
        {
            stopBackgroundSound();
            playPirateSound();
        }
        else
        {
            engine->stopAllSounds();
            playBackgroundSound();
        }
    }
    if (key == GLFW_KEY_KP_5 && action == GLFW_PRESS)
    {
        startVulturiAnimation = !startVulturiAnimation;
        if (startVulturiAnimation == true)
        {
            playEagleSound();
        }
        else
        {
            engine->stopAllSounds();
            playBackgroundSound();
        }
    }
    if (key == GLFW_KEY_M && action == GLFW_PRESS)
    {
        startEliceAnimation = !startEliceAnimation;
    }
    if (key == GLFW_KEY_KP_4 && action == GLFW_PRESS)
    {
        startBilaStangaAnimation = !startBilaStangaAnimation;
        if (startBilaStangaAnimation == true)
        {
            playDragonSound();
        }
    }
    if (key == GLFW_KEY_KP_6 && action == GLFW_PRESS)
    {
        startBilaDreaptaAnimation = !startBilaDreaptaAnimation;
        if (startBilaDreaptaAnimation == true)
        {
            playDragonSound();
        }
    }

    if (key == GLFW_KEY_KP_1 && action == GLFW_PRESS)
    {
        if (showWeapons == true)
        {
            startArmAnimation = !startArmAnimation;
        }
    }
    if (key == GLFW_KEY_KP_2 && action == GLFW_PRESS)
    {
        showWeapons = !showWeapons;
        if (showWeapons == true)
        {
            playWeaponSound();
        }
    }
    if (key == GLFW_KEY_KP_3 && action == GLFW_PRESS)
    {
        if (showWeapons == true)
        {
            startDisarmAnimation = !startDisarmAnimation;
        }
    }

    if (key == GLFW_KEY_8 && action == GLFW_PRESS)
    {
        activateDirLight == 0 ? activateDirLight = 1 : activateDirLight = 0;
        myBasicShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "activateDirLight"), activateDirLight);
    }
    if (key == GLFW_KEY_9 && action == GLFW_PRESS)
    {
        activatePointLight == 0 ? activatePointLight = 1 : activatePointLight = 0;
        myBasicShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "activatePointLight"), activatePointLight);
    }
    if (key == GLFW_KEY_0 && action == GLFW_PRESS)
    {
        activateShadows == 0 ? activateShadows = 1 : activateShadows = 0;
        myBasicShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "activateShadows"), activateShadows);
    }
    if (key == GLFW_KEY_7 && action == GLFW_PRESS)
    {
        activateFog == 0 ? activateFog = 1 : activateFog = 0;
        myBasicShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "activateFog"), activateFog);
    }

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            pressedKeys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            pressedKeys[key] = false;
        }
    }
}

void cameraMovement()
{
    if (pressedKeys[GLFW_KEY_KP_ADD])
    {
        cameraSpeed += 0.5f;
        myCamera.setSpeed(cameraSpeed);
    }
    if (pressedKeys[GLFW_KEY_KP_SUBTRACT])
    {
        cameraSpeed -= 0.5f;
        myCamera.setSpeed(cameraSpeed);
    }

    if (pressedKeys[GLFW_KEY_W])
    {
        myCamera.move(gps::MOVE_FORWARD, deltaTime);
        // update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_S])
    {
        myCamera.move(gps::MOVE_BACKWARD, deltaTime);
        // update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_A])
    {
        myCamera.move(gps::MOVE_LEFT, deltaTime);
        // update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_D])
    {
        myCamera.move(gps::MOVE_RIGHT, deltaTime);
        // update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_Q])
    {
        angleY -= 1.0f;
        // update model matrix for scene
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0, 1, 0));
        // update normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_E])
    {
        angleY += 1.0f;
        // update model matrix for scene
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0, 1, 0));
        // update normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_UP])
    {
        myCamera.move(gps::MOVE_UP, deltaTime);
        // update view matrix
        view = myCamera.getViewMatrix();

        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_DOWN])
    {
        myCamera.move(gps::MOVE_DOWN, deltaTime);
        // update view matrix
        view = myCamera.getViewMatrix();

        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }
}

void visualizationModes()
{
    if (pressedKeys[GLFW_KEY_1])
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    if (pressedKeys[GLFW_KEY_2])
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (pressedKeys[GLFW_KEY_3])
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }
    if (pressedKeys[GLFW_KEY_4])
    {
        glShadeModel(GL_SMOOTH);
    }
    if (pressedKeys[GLFW_KEY_5])
    {
        glShadeModel(GL_FLAT);
    }
}

void processMovement()
{
    cameraMovement();     // check for camera movement
    visualizationModes(); // check for visualization modes

    if (pressedKeys[GLFW_KEY_E])
    {
        angleY += 1.0f;
    }
    if (pressedKeys[GLFW_KEY_Q])
    {
        angleY -= 1.0f;
    }

    if (pressedKeys[GLFW_KEY_L])
    {
        lightAngle += 1.0f;
    }
    if (pressedKeys[GLFW_KEY_J])
    {
        lightAngle -= 1.0f;
    }
}

void initOpenGLWindow()
{
    myWindow.Create(glWindowWidth, glWindowHeight, "OpenGL Project Core");
}

void setWindowCallbacks()
{
    glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    glfwSetScrollCallback(myWindow.getWindow(), scrollCallback);

    // glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initOpenGLState()
{
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS);    // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE);  // cull face
    glCullFace(GL_BACK);     // cull back face
    glFrontFace(GL_CCW);     // GL_CCW for counter clock-wise
}

void initModels()
{
    // for showing depth map
    screenQuad.LoadModel("models/objects/quad/quad.obj");

    // static scene
    castele.LoadModel("scena/castele/main.obj");
    cladiri.LoadModel("scena/cladiri/main.obj");
    map.LoadModel("scena/map/main.obj");
    village.LoadModel("scena/village/main.obj");

    // warrior
    warrior.LoadModel("scena/objects/warrior/warrior.obj");
    shield.LoadModel("scena/objects/warrior/shield.obj");
    sulita.LoadModel("scena/objects/warrior/sulita.obj");

    // lights
    luminaCastel.LoadModel("scena/objects/lumini/lumina-castel.obj");
    luminaInsula.LoadModel("scena/objects/lumini/lumina-insula.obj");

    // others
    barca.LoadModel("scena/objects/barca/barca.obj");
    vulturi.LoadModel("scena/objects/vulturi/vulturi.obj");
    bilaDragon_stanga.LoadModel("scena/objects/bile-dragoni/bila-dragon-stanga.obj");
    bilaDragon_dreapta.LoadModel("scena/objects/bile-dragoni/bila-dragon-dreapta.obj");
    eliceMoara.LoadModel("scena/village/elice-moara.obj");
}

void initShaders()
{
    //------------------------------------------SHADOWS-------------------------------------------------

    // myBasicShader.loadShader("shaders/shadows/shaderStart.vert", "shaders/shadows/shaderStart.frag");
    // myBasicShader.useShaderProgram();
    myBasicShader.loadShader("shaders/basic.vert", "shaders/basic.frag");
    myBasicShader.useShaderProgram();

    depthMapShader.loadShader("shaders/shadows/depthMapShader.vert", "shaders/shadows/depthMapShader.frag");
    depthMapShader.useShaderProgram();

    screenQuadShader.loadShader("shaders/shadows/screenQuad.vert", "shaders/shadows/screenQuad.frag");
    screenQuadShader.useShaderProgram();

    //------------------------------------------SCENE-------------------------------------------------

    skyboxShader.loadShader("shaders/scene/skyboxShader.vert", "shaders/scene/skyboxShader.frag");
    skyboxShader.useShaderProgram();

    lightShader.loadShader("shaders/scene/lightCube.vert", "shaders/scene/lightCube.frag");
    lightShader.useShaderProgram();
}

void initUniforms()
{
    myBasicShader.useShaderProgram();

    // create model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

    // create projection matrix
    projection = glm::perspective(glm::radians(45.0f),
                                  (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
                                  0.1f, 1000.0f);
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // setup a material for the scene
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "material.shininess"), 32.0f);

    // setup light for scene
    // set the light direction (direction towards the light)
    lightDir = luminaWarPos - warriorPos;
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "dirLight.direction"), 1, glm::value_ptr(lightDir));
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "dirLight.ambient"), 0.2f, 0.2f, 0.2f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "dirLight.diffuse"), 0.5f, 0.5f, 0.5f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "dirLight.specular"), 1.0f, 1.0f, 1.0f);

    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.position"), 1, glm::value_ptr(luminaInsulaPos));
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.constant"), 1.0f);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.linear"), 0.045f);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.quadratic"), 0.0075f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.ambient"), 0.2f, 0.2f, 0.2f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.diffuse"), 0.5f, 0.5f, 0.5f);
    glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLight.specular"), 1.0f, 1.0f, 1.0f);

    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "activateDirLight"), activateDirLight);
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "activatePointLight"), activatePointLight);
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "activateShadows"), activateShadows);

    lightShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void initFBO()
{
    // TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
    glGenFramebuffers(1, &shadowMapFBO);

    // create depth texture for FBO
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    // attach texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void initSkyBox()
{
    std::vector<const GLchar *> faces;

    faces.push_back("skybox/alps/alps_rt.tga");
    faces.push_back("skybox/alps/alps_lf.tga");
    faces.push_back("skybox/alps/alps_up.tga");
    faces.push_back("skybox/alps/alps_dn.tga");
    faces.push_back("skybox/alps/alps_bk.tga");
    faces.push_back("skybox/alps/alps_ft.tga");

    mySkyBox.Load(faces);
}

glm::mat4 computeLightSpaceTrMatrix()
{
    glm::vec3 pozitieLumina = glm::inverseTranspose(glm::mat3(lightRotation)) * lightDir;
    pozitieLumina += glm::vec3(warriorPos.x, warriorPos.y - 0.5f, warriorPos.z);

    glm::mat4 lightView = glm::lookAt(pozitieLumina, warriorPos, glm::vec3(0.0f, 1.0f, 0.0f));

    const GLfloat near_plane = 0.1f, far_plane = 4.0f;
    glm::mat4 lightProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, near_plane, far_plane);

    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

    return lightSpaceTrMatrix;
}

void drawWarrior(gps::Shader shader, bool depthPass)
{
    shader.useShaderProgram();

    model = glm::mat4(1.0f);
    model = glm::translate(model, warriorPos);
    model = glm::rotate(model, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, -warriorPos);

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass)
    {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    warrior.Draw(shader);
}

void drawShield(gps::Shader shader, bool depthPass)
{
    shader.useShaderProgram();

    model = glm::mat4(1.0f);
    model = glm::translate(model, warriorPos);
    model = glm::rotate(model, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, -warriorPos);

    model = glm::translate(model, glm::vec3(shieldX, 0.0f, 0.0f));

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass)
    {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    shield.Draw(shader);
}

void drawSulita(gps::Shader shader, bool depthPass)
{
    shader.useShaderProgram();

    model = glm::mat4(1.0f);
    model = glm::translate(model, warriorPos);
    model = glm::rotate(model, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, -warriorPos);

    model = glm::translate(model, sulitaPos);
    model = glm::rotate(model, glm::radians(angleSulita), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::translate(model, -sulitaPos);

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass)
    {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    sulita.Draw(shader);
}

void drawPlayer(gps::Shader shader, bool depthPass)
{
    drawWarrior(shader, depthPass);

    if (showWeapons)
    {
        drawShield(shader, depthPass);
        drawSulita(shader, depthPass);
    }
}

void drawBoat(gps::Shader shader)
{
    shader.useShaderProgram();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(boatX, 0.0f, 0.0f));

    if (rotateBoat)
    {
        model = glm::translate(model, boatPos);
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, -boatPos);
    }

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    barca.Draw(shader);
}

void drawVulturi(gps::Shader shader)
{
    shader.useShaderProgram();

    model = glm::mat4(1.0f);
    model = glm::translate(model, vulturiPos);
    model = glm::rotate(model, glm::radians(angleVulturi), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, -vulturiPos);

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    vulturi.Draw(shader);
}

void drawEliceMoara(gps::Shader shader)
{
    shader.useShaderProgram();

    model = glm::mat4(1.0f);
    model = glm::translate(model, elicePos);
    model = glm::rotate(model, glm::radians(angleElice), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, -elicePos);

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    eliceMoara.Draw(shader);
}

void drawBilaStanga(gps::Shader shader)
{
    shader.useShaderProgram();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, bilaStangaZ));

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    bilaDragon_stanga.Draw(shader);
}

void drawBilaDreapta(gps::Shader shader)
{
    shader.useShaderProgram();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, bilaDreaptaZ));

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    bilaDragon_dreapta.Draw(shader);
}

void drawLightCube1(gps::Shader shader)
{
    shader.useShaderProgram();

    model = glm::translate(glm::mat4(1.0f), warriorPos);
    model *= lightRotation;
    model = glm::translate(model, -warriorPos);

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    luminaCastel.Draw(shader);
}

void drawLightCube2(gps::Shader shader)
{
    shader.useShaderProgram();

    model = glm::mat4(1.0f);

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    luminaInsula.Draw(shader);
}

void renderWithDepthPass(gps::Shader shader)
{
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    //-------------------------DEPTH MAP SHADER-----------------------
    shader.useShaderProgram();

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "lightSpaceTrMatrix"),
                       1,
                       GL_FALSE,
                       glm::value_ptr(computeLightSpaceTrMatrix()));

    // draw the player and signal depth pass true
    drawPlayer(shader, true);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderWithShadowMap(gps::Shader shader)
{
    glViewport(0, 0, (float)myWindow.getWindowDimensions().width, (float)myWindow.getWindowDimensions().height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //-------------------------BASIC SHADER-----------------------
    shader.useShaderProgram();

    lightRotation = glm::mat4(1.0f);
    lightRotation = glm::rotate(lightRotation, glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));

    glUniform3fv(glGetUniformLocation(shader.shaderProgram, "dirLight.direction"), 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(lightRotation)) * lightDir));

    // bind the shadow map
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glUniform1i(glGetUniformLocation(shader.shaderProgram, "shadowMap"), 3);

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "lightSpaceTrMatrix"),
                       1,
                       GL_FALSE,
                       glm::value_ptr(computeLightSpaceTrMatrix()));

    // draw the player and signal depth pass false
    drawPlayer(shader, false);
}

void renderDepthMap(gps::Shader shader)
{
    glViewport(0, 0, (float)myWindow.getWindowDimensions().width, (float)myWindow.getWindowDimensions().height);
    glClear(GL_COLOR_BUFFER_BIT);

    //-------------------------USING SCREEN QUAD SHADER -----------------------
    shader.useShaderProgram();

    // bind the depth map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glUniform1i(glGetUniformLocation(shader.shaderProgram, "depthMap"), 0);

    glDisable(GL_DEPTH_TEST);
    screenQuad.Draw(shader);
    glEnable(GL_DEPTH_TEST);
}

void drawScene(gps::Shader shader)
{
    shader.useShaderProgram();

    model = glm::mat4(1.0f);
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

    castele.Draw(shader);
    cladiri.Draw(shader);
    map.Draw(shader);
    village.Draw(shader);
}

//------------------------------------------ANIMATION-------------------------------------------------

void cameraAnimation()
{
    // Rotate around the target point
    float camX = centerX + sin(glfwGetTime() * angularSpeed) * circleRadius;
    float camZ = centerZ + cos(glfwGetTime() * angularSpeed) * circleRadius;

    myCamera.setPosition(glm::vec3(camX, 90.0f, camZ));
    myCamera.setTarget(glm::vec3(centerX, 0.0f, centerZ));

    float yaw = glm::degrees(atan2(0.0f - myCamera.getPosition().z, 0.0f - myCamera.getPosition().x));
    myCamera.setYaw(yaw);
    myCamera.setPitch(-30.0f);
    myCamera.rotate(-30.0f, yaw);

    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    // compute normal matrix for scene
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
}

void boatAnimation()
{
    boatX += boatSpeed * deltaTime;

    if (boatX > 56.0f || boatX < 0.0f)
    {
        boatSpeed = -boatSpeed;
        rotateBoat = !rotateBoat;
    }
}

void vulturiAnimation()
{
    angleVulturi += vulturiSpeed * deltaTime;
}

void eliceMoaraAnimation()
{
    angleElice += eliceSpeed * deltaTime;
}

void bilaStangaAnimation()
{
    bilaStangaZ -= bilaSpeed * deltaTime;

    if (bilaStangaZ < -20.0f)
    {
        bilaStangaZ = 0.0f;
        startBilaStangaAnimation = false;
    }
}

void bilaDreaptaAnimation()
{
    bilaDreaptaZ -= bilaSpeed * deltaTime;

    if (bilaDreaptaZ < -20.0f)
    {
        bilaDreaptaZ = 0.0f;
        startBilaDreaptaAnimation = false;
    }
}

// sulita 90 -> 163 grade x
// shield 7,5536 ->7,6708
void armAnimation()
{
    if (shieldX < (7.5536f - 7.708f))
    {
        shieldX = shieldX;
    }
    else
    {
        shieldX -= shieldSpeed * deltaTime;
    }

    if (angleSulita > (163.0f - 90.0f))
    {
        angleSulita = angleSulita;
    }
    else
    {
        angleSulita += sulitaSpeed * deltaTime;
    }

    if (shieldX < (7.5536f - 7.708f) && angleSulita > (163.0f - 90.0f))
    {
        startArmAnimation = false;
    }
}

void disarmAnimation()
{
    if (shieldX > 0.0f)
    {
        shieldX = shieldX;
    }
    else
    {
        shieldX += shieldSpeed * deltaTime;
    }

    if (angleSulita < 0.0f)
    {
        angleSulita = angleSulita;
    }
    else
    {
        angleSulita -= sulitaSpeed * deltaTime;
    }

    if (shieldX > 0.0f && angleSulita < 90.0f)
    {
        startDisarmAnimation = false;
    }
}

//------------------------------------------RENDER-------------------------------------------------

bool sceneLoaded = true;

void renderScene()
{
    // RENDER ANIMATION-------------------------------------------------

    if (sceneLoaded == true)
    {
        playBackgroundSound();
        sceneLoaded = false;
    }

    if (startCameraAnimation)
    {
        cameraAnimation();
    }
    if (startBoatAnimation)
    {
        boatAnimation();
    }
    if (startVulturiAnimation)
    {
        vulturiAnimation();
    }
    if (startEliceAnimation)
    {
        eliceMoaraAnimation();
    }
    if (startBilaStangaAnimation)
    {
        bilaStangaAnimation();
    }
    if (startBilaDreaptaAnimation)
    {
        bilaDreaptaAnimation();
    }

    if (startArmAnimation)
    {
        armAnimation();
    }
    if (startDisarmAnimation)
    {
        disarmAnimation();
    }

    // RENDER SCENE-------------------------------------------------

    renderWithDepthPass(depthMapShader);

    // render depth map on screen - toggled with the M key
    if (showDepthMap)
    {
        renderDepthMap(screenQuadShader);
    }
    else
    {
        // final scene rendering pass (with shadows)
        renderWithShadowMap(myBasicShader);

        drawScene(myBasicShader);

        // draw objects
        drawBoat(myBasicShader);
        drawVulturi(myBasicShader);
        drawEliceMoara(myBasicShader);

        if (startBilaStangaAnimation)
        {
            drawBilaStanga(myBasicShader);
        }
        if (startBilaDreaptaAnimation)
        {
            drawBilaDreapta(myBasicShader);
        }

        // draw a white cube around the light
        drawLightCube1(lightShader);
        drawLightCube2(lightShader);

        // skybox
        mySkyBox.Draw(skyboxShader, view, projection);
    }
}

void cleanup()
{
    myWindow.Delete();
    // cleanup code for your own data
    engine->drop();
}

int main(int argc, const char *argv[])
{

    try
    {
        initOpenGLWindow();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
    initModels();
    initShaders();
    initUniforms();
    setWindowCallbacks();

    // init sound engine
    initSoundEngine();
    // init fbo
    initFBO();
    // init skybox
    initSkyBox();

    glCheckError();
    // application loop
    while (!glfwWindowShouldClose(myWindow.getWindow()))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processMovement();
        renderScene();

        glfwPollEvents();
        glfwSwapBuffers(myWindow.getWindow());

        glCheckError();
    }

    cleanup();

    return EXIT_SUCCESS;
}
