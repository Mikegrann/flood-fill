#ifndef FLUID_PROJECTILE_HPP
#define FLUID_PROJECTILE_HPP

#include "glm/glm.hpp"
#include <set>

#include "game_object.hpp"
#include "collision_object.hpp"
#include "object.hpp"
#include "uniform_3d_grid.hpp"

#include "pointer_macros.h"
DEF_PTR(FluidProjectile, FluidProjectilePtr);

class FluidProjectile : public GameObject, public CollisionObject{

public:

  FluidProjectile(glm::vec3 _position, glm::vec3 _movementDirection,
    int _colorMask);

  void setup();
  void update();

  void collided(CollisionObjectPtr collidedWith);

private:

  void createWaterSurfaceAt(Uniform3DGridPtr<int> grid, glm::vec3 newPos);

  ObjectPtr fluidProjectile;
  ObjectPtr fluidParticles;
  glm::vec3 position;
  glm::vec3 oldPosition;
  glm::vec3 origPosition;
  glm::vec3 movementDirection; 
  glm::vec3 size;
  std::string color;
  int colorMask;
  bool hasCollided;
  bool createdSurface;
  double timer;
  float angle;
  float totalTime;

  
};

#endif
