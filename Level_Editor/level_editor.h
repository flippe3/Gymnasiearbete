#ifndef LEVEL_EDITOR_H
#define LEVEL_EDITOR_H
#include "game.h"

enum Edit_Menu
{
    MAIN_GAME,
    SOKOBAN,
    MINE_SWEEPER,
    MENUS
};

class Level_Editor
{
public:
    Edit_Menu edit_state;
    GLuint Width, Height;
    Level_Editor(GLuint width, GLuint height);
    ~Level_Editor();
    
    void Render(SpriteRenderer *Renderer);
    void Input(double dt);    
};

#endif
