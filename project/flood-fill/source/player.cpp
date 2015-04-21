#include "player.hpp"

#include <cstdlib>
#include <iostream>
#include "debug_macros.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_PURE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "GLIncludes.h"
#include "global_variables.hpp"
#include "bounding_box.hpp"
#include "time_manager.hpp"
#include "fluid_projectile.hpp"
#include "director.hpp"
#include "collision_manager.hpp"

Player::Player(Camera * _camera)
  : GameObject(), CollisionObject(), camera(_camera){}

void Player::setup() {
    INFO("Player Setup...");

	lastPosition = camera->getEye();
	
    setCollisionID(2);
    setCollideWithID(1);
	setCanCollide(true);
	
	INFO("Can Collide: " << canCollide());
    camera->fix(false, true, false);

    shootPressed = false;
    shootCount = 0;
}

void Player::update() {
    lastPosition = camera->getEye();

    float cameraSpeed = 5.0f*TimeManager::getDeltaTime();
    if(glfwGetKey(Global::window, GLFW_KEY_W) == GLFW_PRESS){
        camera->zoom(Camera::FORWARD_DIRECTION, cameraSpeed);
    }
    else if(glfwGetKey(Global::window, GLFW_KEY_S) == GLFW_PRESS){
        camera->zoom(Camera::BACKWARD_DIRECTION, cameraSpeed);
    }
    if(glfwGetKey(Global::window, GLFW_KEY_A) == GLFW_PRESS){
        camera->strafe(Camera::LEFT_DIRECTION, cameraSpeed);
    }
    else if(glfwGetKey(Global::window, GLFW_KEY_D) == GLFW_PRESS){
        camera->strafe(Camera::RIGHT_DIRECTION, cameraSpeed);
    }

	setPosition(camera->getEye());

    if(glfwGetKey(Global::window, GLFW_KEY_X) == GLFW_PRESS && !shootPressed){
        FluidProjectile *fluidProjectile = new FluidProjectile(
            camera->getEye(), -glm::normalize(camera->getViewVector()));
        fluidProjectile->setup();
        Director::getScene()->addGameObject("fluidProjectile", fluidProjectile);
        CollisionManager::addCollisionObjectToList(fluidProjectile);
        shootPressed = true;
        shootCount++;
    }
    if(glfwGetKey(Global::window, GLFW_KEY_X) == GLFW_RELEASE) {
        shootPressed = false;
    }
}

void Player::collided(CollisionObject * collidedWith){

  switch (collidedWith->getCollisionID()) {
  case 1:
	INFO("DETECTING COLLISION WITH BLOCK!");
	camera->setEye(lastPosition);
	break;
  default:
	break;
  }

}

