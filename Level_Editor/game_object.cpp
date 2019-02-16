#include "game_object.h"


GameObject::GameObject() 
    : Position(0, 0), Size(1, 1), Velocity(0.0f), Color(1.0f), Movement(0), Saved_Pos(0.0f), Rotation(0.0f), Sprite(), death(false), fall_through(false), boost(false), less_gravity(false), destroyed(false), move(false), Blink(0.0f) { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 movement, glm::vec2 saved_pos,glm::vec2 velocity, GLfloat blink) 
    : Position(pos), Size(size), Velocity(velocity), Color(color), Rotation(0.0f), Sprite(sprite), Movement(movement), Saved_Pos(saved_pos),death(false), fall_through(false), boost(false), less_gravity(false), destroyed(false), move(false), Blink(0.0f) { }

void GameObject::Draw(SpriteRenderer &renderer)
{
    renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}
  
 
