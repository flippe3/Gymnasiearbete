#ifndef GAMELEVEL_H
#define GAMELEVEL_H
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "game_object.h"
#include "sprite_renderer.h"
#include "resource_manager.h"

class GameLevel
{
public:
    std::vector<GameObject> Bricks;
    std::vector<std::vector<GLuint>> tileData;
    GameLevel() { }
    void			Load(const GLchar *file, GLuint levelWidth, GLuint levelHeight);
    void			init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight);
    void			Clear(const GLchar *file, int level);
    void			new_level(int levels);
    void                        Draw(SpriteRenderer &renderer);
    int objects =	0;    
    GameObject		block_features(GameObject obj);
    bool features = false;
};

#endif
