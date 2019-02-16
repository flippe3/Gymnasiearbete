#ifndef LAGER_GAME_H
#define LAGER_GAME_H
#include <vector>
#include <tuple>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "lager_game_object.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "text_renderer.h"
#include "lager_game_level.h"

enum Lager_GameState {
    LAGER_GAME_ACTIVE,
    LAGER_GAME_MENU,
    LAGER_GAME_LEVEL_EDITOR,
    LAGER_GAME_WIN,
    LAGER_GAME_DEATH,
    LAGER_START_MENU,
    LAGER_LEVEL_MAP
};

enum Movement
{
    up,
    down,
    right,
    left,
    still    
};

class Lager_Game
{
public:
    Lager_Game(GLuint idth, GLuint height);
    ~Lager_Game();
    
    GLuint                 Width, Height;
    Lager_GameState              Lager_State;
    GLboolean              Keys[1024];
    std::vector<Lager_GameLevel> Levels;
    int                    Level;
    Movement               mov;
    bool Exit;
    bool check_up(Lager_GameObject &player);
    bool check_down(Lager_GameObject &player);
    bool check_left(Lager_GameObject &player);
    bool check_right(Lager_GameObject &player);
    
    void Draw_Grid(SpriteRenderer &Renderer, int start_x, int start_y, int square_width, int square_height, int num_x, int num_y);
    void Init();
    void Render();
    void Input(double dt);
    void Update(double dt);
    bool game_exit = false;
};    
#endif
