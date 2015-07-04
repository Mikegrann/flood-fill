#ifndef BOX_HPP
#define BOX_HPP

#include "glm/glm.hpp"

#include "game_object.hpp"
#include "collision_object.hpp"
#include "object.hpp"

#include "pointer_macros.h"
DEF_PTR(Box, BoxPtr);

class Box : public GameObject, public CollisionObject{

public:

  Box(glm::vec3 _position, glm::vec3 _movementDirection, float _speed);

  void setup();
  void update();

  void collided(CollisionObjectPtr collidedWith);

private:

  ObjectPtr box;
  glm::vec3 position;
  glm::vec3 movementDirection; 
  glm::vec3 size;
  
  float speed;

};

#endif
