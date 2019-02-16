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

enum GameState {
    PLACE,
    DELETE,
    MOVE,
    PAINT,
    CHOOSE,
    MAP
};
enum Status_Bar
{
    SHOW,
    HIDE
};

enum Game_Menu
{
    MAIN_MENU,
    SOUND_MENU,
    VIDEO_MENU,
    CONTROL_MENU
};

const glm::vec2 PLAYER_SIZE(25, 30);
const GLfloat PLAYER_VELOCITY(325.0f);

class Game
{
public:
    GameState State;
    Status_Bar status_bar_state;
    Game_Menu GameMenu;
    GLboolean Keys[1024];
    GLuint Width, Height; 
    std::vector<GameLevel> Levels;
    std::vector<GameObject> Current_Level;
    int                    Level;
    bool		    Exit;
    bool resize;
    int Size_Width = 1400, Size_Height = 900; 
    //Constructors
    Game(GLuint width, GLuint height);
    ~Game();

    //Runs before the game loop starts 
    void Init();

    void ProcessInput(double dt);
    void Update(double dt);
    void Render();
    void Load_Textures();
    void Load_Levels();
    void draw_grid(SpriteRenderer &Renderer, int start_x, int start_y, int square_width, int square_height, int num_x, int num_y);
    void draw_map(SpriteRenderer &Renderer);
    void draw_map_cursor(SpriteRenderer &Renderer);
    void draw_status_bar(SpriteRenderer &Renderer);
    void mouse_pos(double mouse_x, double mouse_y, double width_ratio, double height_ratio, double dt);
    void show_blocks(SpriteRenderer &Renderer);
    void Write(const GLchar *file, const GLchar *file_2, int n, int mouse_x, int mouse_y);
    void Delete_Block(const GLchar *file, const GLchar *file_2, int x_pos, int y_pos, std::vector<GameObject> temp_bricks, int n);
    void Load_Blocks();
    void Load_Level(const GLchar *file);
    void Draw_Level(SpriteRenderer &renderer);
    void draw_text_editor(SpriteRenderer &renderer);
    void Text_Label(std::string temp_string, int x_pos, int y_pos, glm::vec3 color);
    void Print_Text();
    void Write_Change(SpriteRenderer &renderer, const GLchar *file, const GLchar *file_2, int x_pos, int y_pos, int n);
    void Fill_Textboxes();
    std::string Decimals(double input, int deci);
    void Reverse();
};


#endif
