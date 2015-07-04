#ifndef TUNNEL_LEVEL_HPP
#define TUNNEL_LEVEL_HPP

#include "level_template.hpp"
#include "text.hpp"
#include "text_render.hpp"

#include "pointer_macros.h"
DEF_PTR(TunnelLevel, TunnelLevelPtr);

class TunnelLevel : public LevelTemplate{

public:
    TunnelLevel();
    TunnelLevel(bool _includeCinema);

    void setup();
    void update();
    void reset();

private:
    void createRenders();
    LightPtr l1;
    double timer;
    bool includeCinema;
    glm::vec3 gridCenter;
    TextPtr levelTitle;
    ObjectPtr sky;
};

#endif
