#include <cstdlib>
#include <iostream>

#include "GLIncludes.h"
#include "debug_macros.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_PURE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "load_manager.hpp"
#include "director.hpp"
#include "global_variables.hpp"
#include "demo_level.hpp"
#include "test_level.hpp"
#include "time_manager.hpp"
#include "render_engine.hpp"
#include "material_manager.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "image.hpp"
#include "texture.hpp"
#include "render_texture.hpp"
#include "water_surface.hpp"
#include "collision_manager.hpp"
#include "level_manager.hpp"
#include "tutorial_level.hpp"
#include "menu.hpp"

using namespace std;
//
//TODO: Encapsulate ALL opengl functions in a library with error checking

void setupGLFW();
void setupGLEW();
void loadContent();
void createScenes();

int main()
{
    setupGLFW();
    setupGLEW();

    // store the initally calculated window viewport,
    // may not be window size on retina displays
    GLint windowViewport[4];
    glGetIntegerv(GL_VIEWPORT, windowViewport);
    Global::FbWidth = windowViewport[2] - windowViewport[0];
    Global::FbHeight = windowViewport[3] - windowViewport[1];

    RenderEngine::setup();

    loadContent();
    LoadManager::loadSound("background.wav");//FEZ OST "Puzzle"
    LoadManager::loadSound("roygbiv_pece_cover.wav");
    //LoadManager::loadSound("windowlicker_kyle_sullivan_remix.wav");
    createScenes();

    Menu::setup();

    /* OpenGL 3.3 Vertex Array Object
    // Each mesh should have one vao to improve performance
    // Responsible to hold all vertex data
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    */

    TimeManager::setTimeStamp();
    TimeManager::setDeltaTime();

    int FPS = 0;
    double timeStamp = TimeManager::getTimeStamp();

    
    
    double escapeCoolDown = 0;
    do{

        if(LevelManager::levelFinished) {
            LevelManager::nextLevel();
            Menu::setNewLevel(true);
        }

        Director::updateScene();
        glViewport(windowViewport[0], windowViewport[1], windowViewport[2], windowViewport[3]);
        Director::renderScene();

        if(TimeManager::getTimeStamp() - timeStamp >= 1.0f){
            DEBUG("FPS: " << FPS);
            DEBUG("Frame Time: " << 1.0f/FPS);
            timeStamp = TimeManager::getTimeStamp();
            FPS = 0;
        }

        TimeManager::setDeltaTime();
        TimeManager::setTimeStamp();
        FPS++;
       
       
        if (Menu::isActive() || Menu::getCurrentPage() == "UserGuide") {
            Menu::update();
        }
        
        if (glfwGetKey(Global::window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !Menu::isActive() &&
            TimeManager::getTimeStamp() - Menu::getLastEscape() > .5 && Menu::getCurrentPage() != "UserGuide") {
            escapeCoolDown = 0.4;
            RenderEngine::removeRenderElement("text");
            RenderEngine::addRenderElement("text", RenderElementPtr(new TextRender()), 10);
            Menu::displayMenu();
        } 
        else if (glfwGetKey(Global::window, GLFW_KEY_ESCAPE) == GLFW_PRESS && Menu::isActive() &&
            TimeManager::getTimeStamp() - Menu::getLastEscape() > .5 && escapeCoolDown <= 0) {
            break;
        }
        if(escapeCoolDown > 0) escapeCoolDown -= TimeManager::getDeltaTime();
       
        // Swap buffers
        glfwSwapBuffers(Global::window);
        glfwPollEvents(); 
    }
    while (Menu::getCurrentPage() != "");

    /* OpenGL 3.3 VAO
    glDeleteVertexArrays(1, &vao);*/

        // Close OpenGL window and terminate GLFW
    glfwTerminate();

    LoadManager::clearSounds();
    return 0;
}


void createScenes(){
    INFO("Creating Scenes...");
    LevelManager::setupLevel(1, true);
}

/**
* Load Files
*/

void loadMeshes(){
    INFO("Loading all meshes...");

    MeshPtr mesh;

    LoadManager::loadMesh("cube.obj");
    mesh = LoadManager::getMesh("cube.obj");
    mesh->resize();
    mesh->calculateNormals();
    mesh->generateVertexBuffer();
    mesh->generateIndexBuffer();
    mesh->generateNormalBuffer();
    mesh->generateTextureCoordinateBuffer();
    mesh->calculateLimits();
    mesh->calculateTangents();
    mesh->generateTangentBuffer();

    LoadManager::loadMesh("plane.obj");
    mesh = LoadManager::getMesh("plane.obj");
    mesh->resize();
    mesh->loadObjNormals();
    mesh->generateVertexBuffer();
    mesh->generateIndexBuffer();
    mesh->generateNormalBuffer();
    mesh->generateTextureCoordinateBuffer();
    mesh->calculateLimits();
    mesh->calculateTangents();
    mesh->generateTangentBuffer();

    LoadManager::loadMesh("particles.obj");
    mesh = LoadManager::getMesh("particles.obj");
    mesh->loadObjNormals();
    mesh->generateVertexBuffer();
    mesh->generateIndexBuffer();
    mesh->generateNormalBuffer();
    mesh->generateTextureCoordinateBuffer();
    mesh->calculateLimits();
    mesh->calculateTangents();
    mesh->generateTangentBuffer();

    LoadManager::loadMesh("plane2.obj");
    mesh = LoadManager::getMesh("plane2.obj");
    mesh->resize();
    mesh->calculateNormals();
    mesh->generateVertexBuffer();
    mesh->generateIndexBuffer();
    mesh->generateNormalBuffer();
    mesh->calculateLimits();

    LoadManager::loadMesh("grid.obj");
    mesh = LoadManager::getMesh("grid.obj");
    mesh->resize();
    mesh->calculateNormals();
    mesh->generateVertexBuffer();
    mesh->generateIndexBuffer();
    mesh->generateNormalBuffer();
    mesh->generateTextureCoordinateBuffer();
    mesh->calculateLimits();
    mesh->calculateTangents();
    mesh->generateTangentBuffer();

    LoadManager::loadMesh("stream.obj");
    mesh = LoadManager::getMesh("stream.obj");
    mesh->resize();
    mesh->calculateNormals();
    mesh->generateVertexBuffer();
    mesh->generateIndexBuffer();
    mesh->generateNormalBuffer();
    mesh->calculateLimits();

    LoadManager::loadMesh("sphere.obj");
    mesh = LoadManager::getMesh("sphere.obj");
    mesh->resize();
    mesh->calculateNormals();
    mesh->generateVertexBuffer();
    mesh->generateIndexBuffer();
    mesh->generateNormalBuffer();
    mesh->generateTextureCoordinateBuffer();
    mesh->calculateLimits();
    mesh->calculateTangents();
    mesh->generateTangentBuffer();

    LoadManager::loadMesh("gun.obj");
    mesh = LoadManager::getMesh("gun.obj");
    mesh->resize();
    mesh->calculateNormals();
    mesh->generateVertexBuffer();
    mesh->generateIndexBuffer();
    mesh->generateNormalBuffer();
    mesh->generateTextureCoordinateBuffer();
    mesh->calculateLimits();
    mesh->calculateTangents();
    mesh->generateTangentBuffer();

    LoadManager::loadMesh("interpcube.obj");
    mesh = LoadManager::getMesh("interpcube.obj");
    //mesh->resize(); // Don't resize this
    mesh->calculateNormals();
    mesh->generateVertexBuffer();
    mesh->generateIndexBuffer();
    mesh->generateNormalBuffer();
}

void loadTextures(){
    INFO("Loading all textures...");

    LoadManager::loadImage("blank_voxel.png");
    LoadManager::loadTexture("VoxelTexture", LoadManager::getImage("blank_voxel.png"));

    LoadManager::loadImage("sky.bmp");
    LoadManager::loadTexture("Sky", LoadManager::getImage("sky.bmp"));

    //string normalMapPath("normal-map-2.png");
    //string normalMapPath("chesterfield_normal_map.png");
    //string normalMapPath("n3.png");
    string normalMapPath("n4.png");
    //string normalMapPath("n5.png");
    LoadManager::loadImage(normalMapPath);
    LoadManager::loadTexture("PaddedNormalMap", LoadManager::getImage(normalMapPath));

    LoadManager::loadImage("colorkey_gun_texture.png");
    LoadManager::loadTexture("GunTexture", LoadManager::getImage("colorkey_gun_texture.png"));

    LoadManager::loadImage("regular_normal.png");
    LoadManager::loadTexture("RegularNormalMap", LoadManager::getImage("regular_normal.png"));

    LoadManager::loadImage("n5.png");
    LoadManager::loadTexture("OldPaintNormalMap", LoadManager::getImage("n5.png"));

    LoadManager::loadImage("grey.png");
    LoadManager::loadTexture("GreyTexture", LoadManager::getImage("grey.png"));

    LoadManager::loadImage("water_normal_map.png");
    LoadManager::loadTexture("WaterNormalMap", LoadManager::getImage("water_normal_map.png"));

    LoadManager::loadImage("pure_white.png");
    LoadManager::loadTexture("PureWhiteTexture", LoadManager::getImage("pure_white.png"));

    LoadManager::loadImage("drain_voxel.png");
    LoadManager::loadTexture("DrainTexture", LoadManager::getImage("drain_voxel.png"));

    WaterSurface::loadShaders();
    LoadManager::loadRenderTexture("waterData");
    LoadManager::loadRenderTexture("waterColor");
    LoadManager::loadRenderTexture("waterBlock");
}

void createMaterials(){
    INFO("Creating materials...");

    /*More Materials: http://devernay.free.fr/cours/opengl/materials.html */

    MaterialPtr material(new Material(glm::vec3(0.5, 0.5, 0.5),
                            glm::vec3(0.5, 0.5, 0.6),
                            glm::vec3(0.1, 0.1, 0.1),
                            100.0f));
    MaterialManager::addMaterial("FlatGrey", material);

    MaterialPtr material2(new Material(glm::vec3(0.3, 0.3, 0.8),
                            glm::vec3(0.0, 0.3, 0.9),
                             glm::vec3(0.3, 0.3, 0.8),
                            4.0f));
    MaterialManager::addMaterial("FlatBlue", material2);

    MaterialPtr material3(new Material(glm::vec3(0.8, 0.3, 0.3),
                            glm::vec3(0.0, 0.3, 0.3),
                             glm::vec3(0.8, 0.3, 0.3),
                            4.0f));
    MaterialManager::addMaterial("FlatRed", material3);

    MaterialPtr material4(new Material(glm::vec3(0.8, 0.3, 0.8),
                            glm::vec3(0.0, 0.3, 0.0),
                             glm::vec3(0.8, 0.3, 0.8),
                            4.0f));
    MaterialManager::addMaterial("FlatPurple", material4);

    MaterialPtr material5(new Material(glm::vec3(1.0, 1.0, 1.0),
                            glm::vec3(0.0, 0.0, 0.0),
                             glm::vec3(0.0, 0.0, 0.0),
                            4.0f));
    MaterialManager::addMaterial("None", material5);

    MaterialPtr material6(new Material(glm::vec3(0.8, 0.3, 0.8),
                            glm::vec3(0.9, 0.3, 0.9),
                            glm::vec3(0.8, 0.3, 0.8),
                            4.0f));
    MaterialManager::addMaterial("Purple", material6);

    MaterialPtr material7(new Material(glm::vec3(0.1, 0.1, 0.1),
                                       glm::vec3(0.13, 0.13, 0.13),
                                       glm::vec3(0.05, 0.05, 0.05),
                                       4.0f));
    MaterialManager::addMaterial("Black", material7);

    MaterialPtr material8(new Material(glm::vec3(0.7, 0.7, 0.7),
                                       glm::vec3(0.9, 0.9, 0.9),
                                       glm::vec3(0.6, 0.6, 0.6),
                                       4.0f));
    MaterialManager::addMaterial("White", material8);

	MaterialPtr material9(new Material(glm::vec3(0.8, 0.3, 0.3),
									   glm::vec3(0.9, 0.3, 0.1),
									   glm::vec3(0.8, 0.3, 0.1),
									   4.0f));
	MaterialManager::addMaterial("Red", material9);

    MaterialPtr removeBlockMat(new Material(glm::vec3(0.5, 0.5, 0.5),
                                            glm::vec3(0.0, 0.0, 0.0),
                                            glm::vec3(0.1, 0.1, 0.1),
                                            4.0f));
    MaterialManager::addMaterial("removeBlock", removeBlockMat);
}

void loadContent(){
    loadMeshes();
    loadTextures();
    createMaterials();
}

/**
* GLFW and GLEW setup
*/

double lastX = Global::ScreenWidth/2.0f, lastY = Global::ScreenHeight/2.0f;
double mouse_sensitivity = 1.0f;
void mouse_movement_callback(GLFWwindow *, double xpos, double ypos){
    if(Director::getScene()->hasMainCamera()
     && !(Director::getScene()->getCamera()->fixedMouse())){
        CameraPtr cam = Director::getScene()->getCamera();

        double xoffset = (xpos - lastX)*mouse_sensitivity;
        double yoffset = (ypos - lastY)*mouse_sensitivity;

        lastX = xpos;
        lastY = ypos;

        float phi = cam->getPhi(), theta = cam->getTheta();
        phi   -= yoffset * 360.0f / Global::ScreenHeight;
        theta += xoffset * 360.0f / Global::ScreenWidth;

        if (phi > 80.0f)  phi =  80.0f;
        if (phi < -80.0f) phi = -80.0f;

        cam->setAngles(theta, phi);
    }
}

void error_callback(int, const char* description)
{
    ASSERT(false, description);
}

void setupGLFW(){
    INFO("Initializing GLFW...");

    // Initialise GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        exit(EXIT_FAILURE);
    }

    glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    /*OpenGL 3.3 Profile Context
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

    // Open a window and create its OpenGL context
    Global::window = glfwCreateWindow(Global::ScreenWidth, Global::ScreenHeight,
                "Flood Fill", NULL, NULL);
    if (Global::window == NULL){
        DEBUG("Failed to open GLFW window.");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(Global::window);
    glfwSetInputMode(Global::window, GLFW_STICKY_KEYS, GL_TRUE);

    glfwSetCursorPosCallback(Global::window, mouse_movement_callback);
}

void setupGLEW(){
    INFO("Initializing GLEW...");
    // Initialize glew
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        DEBUG("Unable to initialize glew.");
        glfwDestroyWindow(Global::window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
}
