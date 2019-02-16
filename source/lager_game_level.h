#ifndef LAGER_GAMELEVEL_H
#define LAGER_GAMELEVEL_H
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "lager_game_object.h"
#include "sprite_renderer.h"
#include "resource_manager.h"

class Lager_GameLevel
{
public:
    std::vector<Lager_GameObject> Bricks;
    std::vector<std::vector<GLuint>> tileData;
    Lager_GameLevel() { }
    void      Load(const GLchar *file, GLuint levelWidth, GLuint levelHeight);
    void      Draw(SpriteRenderer &renderer);
    void      init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight);
    void      Clear(const GLchar *file, int level);
    void      new_level(int levels);
    int objects = 0;    
    Lager_GameObject block_features(Lager_GameObject obj);
    bool features = false;
};

#endif
