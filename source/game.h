#ifndef GAME_H
#define GAME_H
#include <vector>
#include <tuple>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "game_object.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "text_renderer.h"
#include "physics.h"
#include "game_level.h"
#include "level_editor.h"

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_LEVEL_EDITOR,
    GAME_WIN,
    GAME_DEATH,
    START_MENU,
    SOKOBAN,
    GAME_MAP,
    SETTINGS
};
//Sokoban Movement
enum Movement
{
    up,
    down,
    right,
    left,
    still    
};

enum Game_Menu
{
    MAIN_MENU,
    SOUND_MENU,
    VIDEO_MENU,
    CONTROL_MENU
};

enum SokobanState
{
    SOKOBAN_ACTIVE,
    SOKOBAN_WIN,
    SOKOBAN_DEATH
};

enum SettingState
{
    RESOLUTION,
    SOUND,
    KEYBINDINGS
};

const glm::vec2 PLAYER_SIZE(25, 30);
const GLfloat PLAYER_VELOCITY(275.0f);

class Game
{
public:
    GameState State;
    SokobanState Sokoban_State;
    Game_Menu GameMenu;
    GLboolean Keys[1024];
    GLuint Width, Height;
    bool resize;
    int Size_Width = 1400, Size_Height = 900; 
    Movement               mov;
    std::vector<GameLevel> Levels;
    std::vector<GameLevel> Sokoban_Levels;
    int                    Level;
    int                    Sokoban_Level;
    bool		    Exit;
    //Constructors
    Game(GLuint width, GLuint height);
    ~Game();

    //Runs before the game loop starts 
    void Init();

    void ProcessInput(double dt);
    void Update(double dt);
    void Render();
    void Load_Textures();
    void Player_Jump(double dt);
    bool main_player_coll(double dt);
    void Reset_Player();
    bool wall_collision(double dt);
    void Load_Level(const GLchar *file);
    void Load_Levels();
    void setup_blocks();
    void sokoban_update(double dt);
    void sokoban_input(double dt);
    void sokoban_render();
    bool sokoban_check_win();
    bool sokoban_check_left(GameObject &player);
    bool sokoban_check_right(GameObject &player);
    bool sokoban_check_up(GameObject &player);
    bool sokoban_check_down(GameObject &player);
    void Load_Sokoban_Goal();
    void Load_Sokoban_Levels();
    void draw_map(SpriteRenderer &Renderer);
    void draw_map_cursor(SpriteRenderer &Renderer);
    void Load_Block_Functions();
    void Run_Block_Functions(double dt);
    bool fake_collision();
    bool Check_Arcade_Collision();
    void Player_Jump_V2(double dt);
    void Reset_Jump();
    void Resize_Window();

    SettingState Setting_State;
}; 


#endif
