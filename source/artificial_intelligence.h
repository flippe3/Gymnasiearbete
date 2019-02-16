#ifndef ARTIFICIAL_INTELLIGENCE_H
#define ARTIFICIAL_INTELLIGENCE_H

#include "game.h"

class AI
{
public:
    AI();
    ~AI();

    void Test_AI(GameObject player, GameObject robot, double dt);
};

#endif
