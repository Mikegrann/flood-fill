#ifndef LEVEL_MANAGER_HPP
#define LEVEL_MANAGER_HPP

#include "glm/glm.hpp"

#include "pointer_macros.h"
#include <string>

class LevelManager{

public:
    static void resetLevel();
    static void nextLevel();
    static bool levelFinished;
    static void setupLevel(int level);
    static void setupLevel(int level, bool includeCinema);
    static void removeLevel(int level);
    static int getCurrentLevel();

private:
    static int curlevel; 
    static float timeStamp;
    static std::string currentSound;
};

#endif
