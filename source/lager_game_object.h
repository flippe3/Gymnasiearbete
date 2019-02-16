#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "sprite_renderer.h"


// Container object for holding all state relevant for a single
// game object entity. Each object in the game likely needs the
// minimal of state as described within GameObject.
class Lager_GameObject
{
public:
    // Object state
    glm::vec2   Position, Size, Saved_Pos, Velocity, Spawn;
    glm::vec3   Color, Movement, falling_block;
    GLfloat     Rotation = 0.0f, Blink = 0.0f, blink_show = 0.0f, blink_hide = 0.0f;
    
//    GLboolean   IsSolid;
    
    bool   fall_through;
    bool   death;
    bool   move;
    bool   SECRET = false;
    bool   boost;
    bool   reverse_gravity;
    bool   more_gravity;
    bool   less_gravity;
    bool   faster_velocity;
    bool   repeat_falling;
    bool   moving_block;
    
    GLboolean   destroyed;
    // Render state
    Texture2D   Sprite;	
    // Constructor(s)
    Lager_GameObject();
    Lager_GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec3 movement = glm::vec3(0.0f), glm::vec2 saved_pos = glm::vec2(0.0f),glm::vec2 velocity = glm::vec2(0.0f, 0.0f), GLfloat blink = 0.0f, glm::vec3 falling_block = glm::vec3(0, 0, 0));
    // Draw sprite
    virtual void Draw(SpriteRenderer &renderer);
};

#endif
