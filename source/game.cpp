#include <algorithm>
#include <iostream>

#include "game.h"
#include <fstream>
#include <sstream>
#include <utility>
//Creating the objects we need
SpriteRenderer    *Renderer;
GameObject        *Player;
TextRenderer      *Text;
TextRenderer      *Main_Menu;
TextRenderer      *Names;
TextRenderer      *Settings;
GameObject        *Sokoban_Player;
//Now you can use all the physics
physics Physics;
bool pressed_jump = false;
bool fixing_float = false;
bool on_arcade = false;
bool FLYING = false;
glm::mat4 freeze_projection;
bool first_spawn = false;

bool on_monster = false;
int monsterMovement = 1;

double t_timer = 0.0;
double reset_timer = 0.0;
double level_timer = 0.0;
double delete_on_wall = 0.0;
bool on_wall_left;
bool on_wall_right;
bool wall_jump_left = false;
bool wall_jump_right = false;
int screen_width_editor = 1400;
int screen_height_editor = 900;
bool wall_jump = false;
bool first_wall_jump = false;
double jump_time = 0.0;
double old_jump_time = 0.0;
bool jump_down;
std::vector<int> number_blocks;
int level_count = 0;
std::vector<GameObject> temp_lvl;
std::vector<Texture2D> block_type;
std::vector<glm::vec2> sokoban_spawn_point;
std::vector<glm::vec2> sokoban_goal;
bool state_before = false;
double map_d_timer = 0.0;
double map_a_timer = 0.0;
double map_enter_timer = 0.0;
int start_jump_x = 0;
bool can_move = false;
int steps = 0;
int pushes = 0;
int time_round = 0;
double level_time = 0.0;
bool stop_move = false;
double run = 1.0;
int volume_percent = 100;

int start_menu_arrow_pos = 650;
double down_start_pos = 0.0;
int point;
glm::vec2 map_cursor(0, 0);
//Type of block
std::vector<GameObject*> Break_Block;
std::vector<double> Break_Block_Timer;
std::vector<GameObject> Boost_Block;
std::vector<GameObject*> Less_Gravity_Block;
std::vector<GameObject*> Blink_Block;
std::vector<GameObject*> Moving_Blocks;
glm::vec2 spawn_point = glm::vec2(50, 850);

int width_res[12] = {640, 800, 1024, 1152, 1280, 1281, 1280, 1400 ,1440, 1400, 1600, 1920};
int height_res[12] = {480, 600, 768, 768, 720, 854, 960, 900, 960, 1050, 1050, 1080};

double jump_timer = 0.0;

double air_friction = 1.0;
int ticks = 0;
int res = 7;

bool hold_left = false;
bool hold_right = false;

bool first_wall_collide = false;
bool gliding = false;
std::vector<double> blink_timer;

bool lock_jump_right = false;
bool lock_jump_left = false;

double wall_jump_timer = 0.0;
double glider_timer = 0.0;
//Initializing the constructor
Game::Game(GLuint width, GLuint height) :
        State(GAME_ACTIVE), Keys(), Width(width), Height(height), Setting_State(RESOLUTION){}

//Free memory when calling the destructor
Game::~Game()
{
        delete Renderer;
        delete Player;
        delete Text;
        delete Main_Menu;
        delete Names;
        delete Settings;
        delete Sokoban_Player;
}

void Game::Init()
{
        std::ifstream read_levels;
        read_levels.open("../new_level_type/num_levels.lvl");
        int amount_of_levels;
        while(read_levels >> amount_of_levels)
        {
                level_count = amount_of_levels;
        }
        read_levels.close();

        // Load shaders
        ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");
        //Setup the "camera"
        glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
        // Configure shaders
        ResourceManager::GetShader("sprite").Use().SetInteger("sprite", 0);
        ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

        Load_Textures();
        setup_blocks();
        /*this->Size_Width = width_res[3];
          this->Size_Height = height_res[3];
          this->resize = true;
        */
        //Starting the renderer
        Shader load_player;
        load_player = ResourceManager::GetShader("sprite");
        Renderer = new SpriteRenderer(load_player);

        //Setup the text_renderer
        Text = new TextRenderer(this->Width, this->Height);
        Text->Load("../fonts/Pixeled.ttf", 30);

        Main_Menu = new TextRenderer(this->Width, this->Height);
        Main_Menu->Load("../fonts/Sketch College.ttf", 140);

        Names = new TextRenderer(this->Width, this->Height);
        Names->Load("../fonts/Sketch College.ttf", 90);

        Settings = new TextRenderer(this->Width, this->Height);
        Settings->Load("../fonts/Colleged.ttf", 60);

        //@FIX, should probably not always have the game active when the game reloads

        //Creating a player and placing it ASD in the middle TJENA JEP
        glm::vec2 player_pos = glm::vec2(40, this->Height - 120);
        Player = new GameObject(player_pos, PLAYER_SIZE, ResourceManager::GetTexture("player"));
        Player->Color = glm::vec3(1.0f, 1.0f, 1.0f);

        Physics.fake_gravity = -1000.0f;
        Physics.fake_vel = 0.0f;

        this->Levels.clear();
        Load_Levels();
        Physics.Main_Char_Init(*Player, 300.0f, 0.35f);
        Load_Level("../new_level_type/0.lvl");

        //INIT SOKOBAN
        glm::vec2 sokoban_player_pos = sokoban_spawn_point[this->Sokoban_Level];
        glm::vec2 sokoban_player_size = glm::vec2(50, 50);

        Sokoban_Player = new GameObject(sokoban_player_pos, sokoban_player_size, ResourceManager::GetTexture("sokoban_player"));
        this->mov = still;
        Load_Sokoban_Goal();
        this->State = START_MENU;
        this->Setting_State = RESOLUTION;
}

void Game::Update(double dt)
{
        //Jumping it doesnt work but better than nothing
        if(this->State == GAME_ACTIVE)
        {
                Run_Block_Functions(dt);

                if(!FLYING)
                        Player_Jump_V2(dt);

                if (Player->Position.x <= 0)
                        Player->Position.x = 0;

                if(Player->Position.y - Player->Size.y >= this->Height)
                {
                        this->State = GAME_DEATH;
                        Reset_Player();
                        this->State = GAME_ACTIVE;
                }

                first_spawn = true;
        }
        if(this->State == SOKOBAN)
        {
                sokoban_update(dt);
        }
        if(this->State == GAME_DEATH)
        {
                Reset_Player();
                this->State = GAME_ACTIVE;
        }
        //Display fps
        //std::cout << Renderer->fps(dt) << std::endl;
}

int first = false;
double select_state_time = 0.0;
double select_time = 0.0;

void Game::ProcessInput(double dt)
{
        if(this->Keys[GLFW_KEY_ESCAPE])
        {
                start_menu_arrow_pos = 650;
                this->State = START_MENU;
        }

        if(this->State == SETTINGS)
        {
                select_state_time += dt;
                select_time += dt;
                if(this->Setting_State == RESOLUTION && select_state_time >= 0.15)
                {
                        if(this->Keys[GLFW_KEY_DOWN] && select_time >= 0.15)
                        {
                                if(res > 0)
                                        res--;
                                if(res == 0)
                                        res = 11;

                                this->Size_Width = width_res[res];
                                this->Size_Height = height_res[res];
                                this->resize = true;


                                select_time = 0.0;
                        }
                        if(this->Keys[GLFW_KEY_UP] && select_time >= 0.15)
                        {
                                if(res < 12)
                                        res++;
                                if(res == 12)
                                        res = 0;

                                this->Size_Width = width_res[res];
                                this->Size_Height = height_res[res];
                                this->resize = true;

                                select_time = 0.0;
                        }

                        if(this->Keys[GLFW_KEY_LEFT])
                        {
                                this->Setting_State = KEYBINDINGS;
                                select_state_time = 0.0;
                        }
                        else if(this->Keys[GLFW_KEY_RIGHT])
                        {
                                this->Setting_State = SOUND;
                                select_state_time = 0.0;
                        }
                }
                if(this->Setting_State == SOUND && select_state_time >= 0.15)
                {
                        if(this->Keys[GLFW_KEY_LEFT])
                        {
                                this->Setting_State = RESOLUTION;
                                select_state_time = 0.0;
                        }
                        else if(this->Keys[GLFW_KEY_RIGHT])
                        {
                                this->Setting_State = KEYBINDINGS;
                                select_state_time = 0.0;
                        }
                }
                if(this->Setting_State == KEYBINDINGS && select_state_time >= 0.15)
                {
                        if(this->Keys[GLFW_KEY_LEFT])
                        {
                                this->Setting_State = SOUND;
                                select_state_time = 0.0;
                        }
                        else if(this->Keys[GLFW_KEY_RIGHT])
                        {
                                this->Setting_State = RESOLUTION;
                                select_state_time = 0.0;
                        }
                }

                if(select_state_time >= 20)
                        select_state_time = 1.0;
                if(select_time >= 20)
                        select_time = 1.0;
        }

        if(this->State == START_MENU)
        {
                down_start_pos += dt;
                if((this->Keys[GLFW_KEY_S] || this->Keys[GLFW_KEY_DOWN]) && down_start_pos >= 0.2)
                {
                        if(start_menu_arrow_pos < this->Height - 100)
                                start_menu_arrow_pos += 50;
                        else
                                start_menu_arrow_pos = this->Height - 250;
                        down_start_pos = 0.0;
                }
                if((this->Keys[GLFW_KEY_W] || this->Keys[GLFW_KEY_UP]) && down_start_pos >= 0.2)
                {
                        if(start_menu_arrow_pos > this->Height - 250)
                                start_menu_arrow_pos -= 50;
                        else
                                start_menu_arrow_pos = this->Height - 100;
                        down_start_pos = 0.0;
                }

                if(start_menu_arrow_pos == this->Height - 250 && (this->Keys[GLFW_KEY_ENTER] || this->Keys[GLFW_KEY_SPACE]))
                {
                        Reset_Player();
                        this->State = GAME_ACTIVE;
                }
                if(start_menu_arrow_pos == this->Height - 200 && (this->Keys[GLFW_KEY_ENTER] || this->Keys[GLFW_KEY_SPACE]))
                {
                        this->State = GAME_MAP;
                }
                if(start_menu_arrow_pos == this->Height - 150 && (this->Keys[GLFW_KEY_ENTER] || this->Keys[GLFW_KEY_SPACE]))
                {
                        this->State = SETTINGS;
                }
                if(start_menu_arrow_pos == this->Height - 100 && (this->Keys[GLFW_KEY_ENTER] || this->Keys[GLFW_KEY_SPACE]))
                {
                        this->Exit = true;
                }
                if(down_start_pos >= 10.0)
                        down_start_pos = 1.0;
        }
        if(this->State == SOKOBAN)
        {
                if(this->Keys[GLFW_KEY_ESCAPE])
                        this->State = START_MENU;
                sokoban_input(dt);
        }
        if(this->State == GAME_ACTIVE)
        {
                jump_timer += dt;
                if(this->Keys[GLFW_KEY_LEFT_SHIFT])
                        run = 1.35;
                else
                        run = 1.0;
                if(this->Keys[GLFW_KEY_R])
                {
                        Reset_Player();
                }
                if(this->Keys[GLFW_KEY_ESCAPE])
                {
                        this->State = START_MENU;
                }

                if (this->Keys[GLFW_KEY_A] || this->Keys[GLFW_KEY_LEFT])
                {
                        if(FLYING)
                        {
                                Player->Velocity.x = PLAYER_VELOCITY * dt * 1.3;
                                Player->Position.x -= Player->Velocity.x;
                        }
                        else
                        {
                                hold_left = true;
                                if(Player->Position.y >= this->Height)
                                        Player->Position.y = this->Height - Player->Size.y;

                                if(Player->Position.x > 0 && !gliding && !lock_jump_right)
                                {
                                        Player->Velocity.x = PLAYER_VELOCITY * dt * run * air_friction;
                                        Player->Position.x -= Player->Velocity.x;
                                }
                        }
                }
                else
                        hold_left = false;

                if(this->Keys[GLFW_KEY_D] || this->Keys[GLFW_KEY_RIGHT])
                {
                        if(FLYING)
                        {
                                Player->Velocity.x = PLAYER_VELOCITY * dt * 1.3;
                                Player->Position.x += Player->Velocity.x;
                        }
                        else
                        {
                                hold_right = true;
                                if(Player->Position.x < this->Width - PLAYER_SIZE.x && !gliding && !lock_jump_left)
                                {
                                        Player->Velocity.x = PLAYER_VELOCITY * dt * run * air_friction;
                                        Player->Position.x += Player->Velocity.x;
                                }
                        }
                }
                else
                        hold_right = false;

                if ((this->Keys[GLFW_KEY_W] || this->Keys[GLFW_KEY_SPACE] || this->Keys[GLFW_KEY_UP]))
                {
                        wall_collision(dt);
                        if(!FLYING)
                        {
                                jump_time += dt;
                                if(Physics.is_grounded && jump_timer >= 0.05 && !fixing_float)
                                {
                                        pressed_jump = true;
                                        jump_timer = 0.0;
                                }
                                if(on_wall_right && jump_timer >= 0.05)
                                {
                                        wall_jump_left = true;
                                        first_wall_jump = true;
                                        jump_timer = 0.0;
                                }
                                if(on_wall_left && jump_timer >= 0.05)
                                {
                                        wall_jump_right = true;
                                        first_wall_jump = true;
                                        jump_timer = 0.0;
                                }
                        }
                        else if(FLYING)
                        {
                                Player->Velocity.y = PLAYER_VELOCITY * dt * 1.3;
                                Player->Position.y -= Player->Velocity.y;
                        }
                }
                if(this->Keys[GLFW_KEY_S] || this->Keys[GLFW_KEY_DOWN])
                {
                        if(FLYING)
                        {
                                Player->Velocity.y = PLAYER_VELOCITY * dt * 1.3;
                                Player->Position.y += Player->Velocity.y;
                        }
                }
                t_timer += dt;

                if(this->Keys[GLFW_KEY_T] && t_timer >= 0.2)
                {
                        if(!FLYING)
                                FLYING = true;
                        else
                                FLYING = false;
                        t_timer = 0.0;
                }
                reset_timer += dt;
                if(on_arcade)
                {
                        if(this->Keys[GLFW_KEY_F] && reset_timer >= 0.2)
                        {
                                Reset_Player();
                                reset_timer = 0.0;
                        }
                        if(this->Keys[GLFW_KEY_G])
                        {
                                std::cout << "GAME_LEVEL: " << this->Level << std::endl;
                                this->Sokoban_Level = this->Level;
                                time_round = 0;
                                steps = 0;
                                pushes = 0;
                                level_time = 0.0;
                                this->Sokoban_Levels.clear();
                                this->Load_Sokoban_Levels();
                                this->Load_Sokoban_Goal();
                                this->State = SOKOBAN;
                                this->Sokoban_State = SOKOBAN_ACTIVE;
                        }
                }

                if(on_monster)
                {
                        this->State = GAME_DEATH;
                        Reset_Player();
                }
                if(jump_timer >= 20)
                        jump_timer = 1.0;
        }
        if(this->State == GAME_MAP)
        {
                map_d_timer += dt;
                map_a_timer += dt;
                map_enter_timer += dt;
                if((this->Keys[GLFW_KEY_D] || this->Keys[GLFW_KEY_RIGHT])&& map_d_timer >= 0.2)
                {
                        if(point < level_count - 1)
                                point++;
                        else
                                point = 0;

                        map_d_timer = 0.0;
                }
                if((this->Keys[GLFW_KEY_A] || this->Keys[GLFW_KEY_LEFT])&& map_a_timer >= 0.2)
                {
                        if(point > 0)
                                point--;
                        else
                                point = level_count - 1;
                        map_a_timer = 0.0;
                }
                if(this->Keys[GLFW_KEY_ENTER] && map_enter_timer >= 0.2)
                {
                        this->Level = point;
                        this->State = GAME_ACTIVE;
                        Reset_Player();
                        map_enter_timer = 0.0;
                }

                if(map_d_timer >= 20)
                        map_d_timer = 1.0;
                if(map_a_timer >= 20)
                        map_a_timer = 1.0;
                if(map_enter_timer >= 20)
                        map_enter_timer = 1.0;

        }

        if(reset_timer >= 10.0)
                reset_timer = 0.0;
        if(t_timer >= 10.0)
                t_timer = 1.0;
}

void Game::Render()
{
        if(this->State == GAME_ACTIVE)
        {
                //Moving the camera after the player
                //Rendering Background
                Texture2D background_1;
                background_1 = ResourceManager::GetTexture("bakgrund");
                Renderer->DrawSprite(background_1, glm::vec2(0, 0), glm::vec2(this->Width + 150, this->Height), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
                //Rendering Level
                this->Levels[this->Level].Draw(*Renderer);
                //Rendering the player
                Player->Draw(*Renderer);

                if(on_arcade)
                {
                        Texture2D arcade_square;
                        arcade_square = ResourceManager::GetTexture("blank");
                        Renderer->DrawSprite(arcade_square, glm::vec2(200, 300), glm::vec2(1000, 300), 0.0f, glm::vec3(1.0f, 0.0f, 0.4f));
                        Text->RenderText("G for next level", (this->Width / 2) - 325, (this->Height / 2) - 25, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
                }
                //Rendering text
                //Text->RenderText("Y_V: " + std::to_string(Player->Velocity.y), 0, 0, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
                //Text->RenderText("Y_X: " + std::to_string(Player->Velocity.x), 0, 100, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }
        if(this->State == SOKOBAN)
        {
                sokoban_render();
        }
        if(this->State == GAME_WIN)
        {
                Texture2D background_1;
                background_1 = ResourceManager::GetTexture("blank");
                Renderer->DrawSprite(background_1, glm::vec2(0, 0), glm::vec2(this->Width + 150, this->Height), 0.0f, glm::vec3(0.4f, 0.4f, 0.4f));
                Text->RenderText("Congratz you don't totally suck", (this->Width / 2) - 200, (this->Height / 2) - 25, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }
        if(this->State == START_MENU)
        {
                Texture2D background_1;
                background_1 = ResourceManager::GetTexture("backgrund_blur");
                Renderer->DrawSprite(background_1, glm::vec2(0, 0), glm::vec2(this->Width + 150, this->Height), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

                Main_Menu->RenderText("Gymnasiearbete", this->Width / 2 - 640, this->Height / 2 - 150, 1.0, glm::vec3(0.0, 0.0, 0.0));
                Names->RenderText("Filip, Hannes, Jesper", this->Width / 2 - 540, this->Height / 2 - 20, 1.0, glm::vec3(0.0, 0.0, 0.0));

                Texture2D arrow;
                arrow = ResourceManager::GetTexture("arrow");

                Renderer->DrawSprite(arrow, glm::vec2(40, start_menu_arrow_pos), glm::vec2(45, 45), -(3.14 / 2), glm::vec3(1.0f, 1.0f, 1.0f));
                Text->RenderText("Start", 100, this->Height - 250, 1.0f, glm::vec3(0.0f, 0.0f, 0.6f));
                Text->RenderText("Map", 100, this->Height - 200, 1.0f, glm::vec3(0.0f, 0.0f, 0.6f));
                Text->RenderText("Settings", 100, this->Height - 150, 1.0f, glm::vec3(0.0f, 0.0f, 0.6f));
                Text->RenderText("Quit", 100, this->Height - 100, 1.0f, glm::vec3(0.0f, 0.0f, 0.6f));
        }
        if(this->State == GAME_MAP)
        {
                this->draw_map(*Renderer);
                this->draw_map_cursor(*Renderer);
        }
        if(this->State == SETTINGS)
        {
                Texture2D background_1;
                background_1 = ResourceManager::GetTexture("backgrund_blur");
                Renderer->DrawSprite(background_1, glm::vec2(0, 0), glm::vec2(this->Width + 150, this->Height), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

                Settings->RenderText("Settings", this->Width / 2 - 230, 150, 1.0, glm::vec3(0.0, 0.0, 0.0));

                Text->RenderText("RESOLUTION:", 100, this->Height - 600, 1.0f, glm::vec3(0.0f, 0.0f, 0.8f));
                Text->RenderText("SOUND:", 550, this->Height - 600, 1.0f, glm::vec3(0.0f, 0.0f, 0.8f));
                Text->RenderText("KEYBINDINGS:", 950, this->Height - 600, 1.0f, glm::vec3(0.0f, 0.0f, 0.8f));

                //Resolution
                Text->RenderText(std::to_string(this->Size_Width) + "x" + std::to_string(this->Size_Height), 100, this->Height - 540, 1.0f, glm::vec3(0.3f, 0.5f, 0.8f));

                //Sound
                Texture2D square;
                square = ResourceManager::GetTexture("blank");
                Renderer->DrawSprite(square, glm::vec2(525, this->Height - 550), glm::vec2(200, 5), 0.0, glm::vec3(0.3, 0.3, 0.3));
                Renderer->DrawSprite(square, glm::vec2(525, this->Height - 550), glm::vec2(5, 30), 0.0, glm::vec3(0.3, 0.3, 0.3));
                Renderer->DrawSprite(square, glm::vec2(525, this->Height - 520), glm::vec2(205, 5), 0.0, glm::vec3(0.3, 0.3, 0.3));
                Renderer->DrawSprite(square, glm::vec2(725, this->Height - 550), glm::vec2(5, 30), 0.0, glm::vec3(0.3, 0.3, 0.3));

                if(Setting_State == RESOLUTION)
                {
                        Renderer->DrawSprite(square, glm::vec2(90, this->Height - 610), glm::vec2(310, 5), 0.0, glm::vec3(255/148, 0.0, 255/211));
                        Renderer->DrawSprite(square, glm::vec2(90, this->Height - 610), glm::vec2(5, 55), 0.0, glm::vec3(255/148, 0.0, 255/211));
                        Renderer->DrawSprite(square, glm::vec2(90, this->Height - 560), glm::vec2(310, 5), 0.0, glm::vec3(255/148, 0.0, 255/211));
                        Renderer->DrawSprite(square, glm::vec2(395, this->Height - 610), glm::vec2(5, 55), 0.0, glm::vec3(255/148, 0.0, 255/211));
                }
                else if(Setting_State == SOUND)
                {
                        Renderer->DrawSprite(square, glm::vec2(540, this->Height - 610), glm::vec2(175, 5), 0.0, glm::vec3(255/148, 0.0, 255/211));
                        Renderer->DrawSprite(square, glm::vec2(540, this->Height - 610), glm::vec2(5, 55), 0.0, glm::vec3(255/148, 0.0, 255/211));
                        Renderer->DrawSprite(square, glm::vec2(540, this->Height - 560), glm::vec2(175, 5), 0.0, glm::vec3(255/148, 0.0, 255/211));
                        Renderer->DrawSprite(square, glm::vec2(715, this->Height - 610), glm::vec2(5, 55), 0.0, glm::vec3(255/148, 0.0, 255/211));
                }
                else if(Setting_State == KEYBINDINGS)
                {
                        Renderer->DrawSprite(square, glm::vec2(940, this->Height - 610), glm::vec2(320, 5), 0.0, glm::vec3(255/148, 0.0, 255/211));
                        Renderer->DrawSprite(square, glm::vec2(940, this->Height - 610), glm::vec2(5, 55), 0.0, glm::vec3(255/148, 0.0, 255/211));
                        Renderer->DrawSprite(square, glm::vec2(940, this->Height - 560), glm::vec2(320, 5), 0.0, glm::vec3(255/148, 0.0, 255/211));
                        Renderer->DrawSprite(square, glm::vec2(1260, this->Height - 610), glm::vec2(5, 55), 0.0, glm::vec3(255/148, 0.0, 255/211));
                }

                //if(volume_percent == 100)

        }
}

void Game::Load_Textures()
{
        ResourceManager::LoadTexture("../textures/blank.png", GL_TRUE, "blank");
        ResourceManager::LoadTexture("../textures/character.png", GL_TRUE, "player");
        ResourceManager::LoadTexture("../textures/Arcade.png", GL_TRUE, "arcade");
        ResourceManager::LoadTexture("../textures/stone.png", GL_TRUE, "stone");
        ResourceManager::LoadTexture("../textures/Bakgrund.png", GL_TRUE, "bakgrund");
        //ResourceManager::LoadTexture("../textures/background_stone.png", GL_TRUE, "bakgrund");
        ResourceManager::LoadTexture("../textures/background_blur.png", GL_TRUE, "backgrund_blur");
        ResourceManager::LoadTexture("../textures/monster.png", GL_TRUE, "monster");
        ResourceManager::LoadTexture("../textures/sokoban_character_up.png", GL_TRUE, "sokoban_player");
        ResourceManager::LoadTexture("../textures/grey.jpg", GL_TRUE, "sokoban_wallpaper");
        ResourceManager::LoadTexture("../textures/wall.png", GL_TRUE, "wall");
        ResourceManager::LoadTexture("../textures/box.png", GL_TRUE, "box");
        ResourceManager::LoadTexture("../textures/middle.png", GL_TRUE, "middle");
        ResourceManager::LoadTexture("../textures/arrow.png", GL_TRUE, "arrow");
        ResourceManager::LoadTexture("../textures/dirt_block.png", GL_TRUE, "grass_block");
        ResourceManager::LoadTexture("../textures/spikes.png", GL_TRUE, "spikes");
        ResourceManager::LoadTexture("../textures/only_dirt.png", GL_TRUE, "only_dirt");
        ResourceManager::LoadTexture("../textures/metal_block.png", GL_TRUE, "metal_block");
        ResourceManager::LoadTexture("../textures/saws.png", GL_TRUE, "saws");
}


bool first_fall = false;
int start_pos = 0;
//YOU NEED TO BE ABLE TO WALK AWAY FROM THE WALL
void Game::Player_Jump_V2(double dt)
{
        Check_Arcade_Collision();

        //Bottom or Above Collision Stop all jumping
        if(main_player_coll(dt) && !Physics.is_grounded)
        {
                Reset_Jump();
                pressed_jump = false;
                wall_jump_left = false;
                wall_jump_right = false;
                gliding = false;
                jump_down = false;
                fixing_float = false;
                first_wall_collide = false;
                first_wall_jump = true;
                lock_jump_right = false;
                lock_jump_left = false;

                Physics.is_grounded = true;
        }

        //Jump Up
        if(pressed_jump && !fixing_float)
        {
                first_wall_collide = false;
                gliding = false;
                lock_jump_right = false;
                lock_jump_left = false;
                Physics.Jump(*Player, dt);
                air_friction = 0.75;
                //Jump a certain height
                if(old_jump_time == jump_time && !jump_down && Player->Velocity.y > 150)
                {
                        Player->Velocity.y = 150;
                        jump_down = true;
                }
                old_jump_time = jump_time;
                Physics.is_grounded = false;
        }

        //Jump right
        if(wall_jump_right)
        {
                gliding = false;
                pressed_jump = false;
                first_wall_collide = false;
                if(first_wall_jump)
                {
                        lock_jump_right = true;
                        Physics.Main_Char_Init(*Player, 275.0f, 0.45f);
                        first_wall_jump = false;
                        Player->Velocity.x = 200;
                        start_pos = Player->Position.x;
                        wall_jump_timer = 0.1;
                }
                wall_jump_timer -= dt;
                Physics.Jump(*Player, dt);
                if(wall_jump_timer <= -0.1)
                        lock_jump_right = false;

                Player->Position.x += Player->Velocity.x * dt;
        }

        //Jump left
        if(wall_jump_left)
        {
                pressed_jump = false;
                gliding = false;
                first_wall_collide = false;
                if(first_wall_jump)
                {
                        lock_jump_left = true;
                        Physics.Main_Char_Init(*Player, 275.0f, 0.45f);
                        first_wall_jump = false;
                        Player->Velocity.x = 200;
                        wall_jump_timer = 0.1;
                        start_pos = Player->Position.x;
                }
                wall_jump_timer -= dt;
                Physics.Jump(*Player, dt);

                if(wall_jump_timer <= -0.1)
                        lock_jump_left = false;

                Player->Position.x -= Player->Velocity.x * dt;
        }

        if((wall_jump_right || wall_jump_left) && wall_collision(dt))
        {
                gliding = false;
                wall_jump_left = false;
                wall_jump_right = false;
                first_wall_jump = true;
                lock_jump_right = false;
                lock_jump_left = false;
        }

        //Glide
        if(wall_collision(dt) && !wall_jump_left && !wall_jump_right && !Physics.is_grounded)
        {
                gliding = true;
                lock_jump_right = false;
                lock_jump_left = false;
                pressed_jump = false;
                fixing_float = false;
                if(!first_wall_collide)
                {
                        Reset_Jump();
                        Player->Velocity.y = 50;
                        first_wall_collide = true;
                        glider_timer = 0.2;
                }
                else
                        Player->Velocity.y += 10 * dt;

                if(on_wall_right && hold_left)
                        glider_timer -= dt;
                else if(on_wall_left && hold_right)
                        glider_timer -= dt;
                if(glider_timer <= 0.0)
                        gliding = false;

                Player->Position.y += Player->Velocity.y * dt;
        }

        //Fall Down
        if(!pressed_jump && !main_player_coll(dt) && !wall_jump_left && !wall_jump_right && !wall_collision(dt))
        {
                gliding = false;
                lock_jump_right = false;
                lock_jump_left = false;

                fixing_float = true;
                Physics.floating(*Player, dt);
                Physics.is_grounded = false;
        }
}

void Game::Reset_Jump()
{
        Physics.Main_Char_Init(*Player, 300.0f, 0.35f);
        Player->Velocity.y = 700.0;
        Physics.fake_gravity = -1000.0;
        Physics.fake_vel = 0.0;
}

void Game::Reset_Player()
{
        std::string level_file = "../new_level_type/" + std::to_string(this->Level) + ".lvl";
        Load_Level(level_file.c_str());
        spawn_point = glm::vec2(50, 850);
        Load_Block_Functions();
        Player->Position = spawn_point;
}

bool Game::Check_Arcade_Collision()
{
        state_before = false;
        for (GameObject &box : this->Levels[this->Level].Bricks)
        {
                if(box.arcade)
                {
                        if(Physics.bottom_collision(*Player, box) && box.destroyed != GL_TRUE)
                                state_before = true;
                        if(Physics.above_collision(*Player, box) && box.destroyed != GL_TRUE)
                                state_before = true;
                        if(Physics.right_collision(*Player, box) && box.destroyed != GL_TRUE)
                                state_before = true;
                        if(Physics.left_collision(*Player, box) && box.destroyed != GL_TRUE)
                                state_before = true;
                }
        }
        on_arcade = state_before;
}

bool Game::main_player_coll(double dt)
{
        if(!FLYING)
        {
                for (GameObject &box : this->Levels[this->Level].Bricks)
                {
                        if(!box.arcade && !box.boost)
                        {
                                if(Physics.rectangle_circle_collision(*Player, box) && box.less_gravity != 0 && !box.destroyed)
                                {
                                        std::cout << "Checking circle physics" << std::endl;
                                        if(box.death)
                                                this->State = GAME_DEATH;
                                        Player->Velocity.y = 0;
                                        return true;
                                }

                                if (Physics.bottom_collision(*Player, box) && box.destroyed != GL_TRUE && box.less_gravity == 0)
                                {
                                        if(box.death)
                                                this->State = GAME_DEATH;
                                        else
                                                Player->Position.y = box.Position.y - Player->Size.y;

                                        if(box.Break != 0)
                                        {
                                                if(Break_Block.size() > 0)
                                                {
                                                        bool same_object = false;
                                                        for(GameObject *other_box : Break_Block)
                                                        {
                                                                if(&box == other_box)
                                                                {
                                                                        same_object = true;
                                                                        break;
                                                                }
                                                        }
                                                        if(!same_object)
                                                        {
                                                                Break_Block_Timer.push_back(0.0);
                                                                Break_Block.push_back(&box);
                                                        }
                                                }
                                                else
                                                {
                                                        Break_Block_Timer.push_back(0.0);
                                                        Break_Block.push_back(&box);
                                                }
                                        }
                                        return true;
                                }
                                if (Physics.above_collision(*Player, box) && box.destroyed != GL_TRUE && box.less_gravity == 0)
                                {
                                        if(box.death)
                                                this->State = GAME_DEATH;
                                        else
                                                Player->Position.y = box.Position.y + box.Size.y + 0.1;

                                        if(box.Break != 0)
                                        {
                                                if(Break_Block.size() > 0)
                                                {
                                                        bool same_object = false;
                                                        for(GameObject *other_box : Break_Block)
                                                        {
                                                                if(&box == other_box)
                                                                {
                                                                        same_object = true;
                                                                        break;
                                                                }
                                                        }
                                                        if(!same_object)
                                                        {
                                                                Break_Block_Timer.push_back(0.0);
                                                                Break_Block.push_back(&box);
                                                        }
                                                }
                                                else
                                                {
                                                        Break_Block_Timer.push_back(0.0);
                                                        Break_Block.push_back(&box);
                                                }
                                        }

                                        return true;
                                }
                        }
                }
        }
        return false;
}

bool Game::wall_collision(double dt)
{
        if(!FLYING)
        {

                for (GameObject &box : this->Levels[this->Level].Bricks)
                {
                        if(Physics.rectangle_circle_collision(*Player, box) && box.less_gravity != 0 && !box.destroyed)
                        {
                                if(box.death)
                                        this->State = GAME_DEATH;
                                Player->Velocity.x = 0;
                        }

                        if(!box.arcade && !box.boost)
                        {
                                if (Physics.right_collision(*Player, box) && box.destroyed != GL_TRUE  && box.less_gravity == 0)
                                {
                                        if(box.death)
                                                this->State = GAME_DEATH;
                                        else
                                                Player->Position.x = box.Position.x - Player->Size.x;

                                        if(box.Break != 0)
                                        {
                                                if(Break_Block.size() > 0)
                                                {
                                                        bool same_object = false;
                                                        for(GameObject *other_box : Break_Block)
                                                        {
                                                                if(&box == other_box)
                                                                {
                                                                        same_object = true;
                                                                        break;
                                                                }
                                                        }
                                                        if(!same_object)
                                                        {
                                                                Break_Block_Timer.push_back(0.0);
                                                                Break_Block.push_back(&box);
                                                        }
                                                }
                                                else
                                                {
                                                        Break_Block_Timer.push_back(0.0);
                                                        Break_Block.push_back(&box);
                                                }
                                        }

                                        if(!Physics.is_grounded)
                                                on_wall_right = true;
                                        return true;
                                }
                                else
                                {
                                        GameObject temp_player = *Player;
                                        temp_player.Position.x += 1;
                                        if (Physics.right_collision(temp_player, box) && box.destroyed != GL_TRUE  && box.less_gravity == 0)
                                        {
                                                if(box.death)
                                                        this->State = GAME_DEATH;
                                                on_wall_right = true;

                                                if(box.Break != 0)
                                                {
                                                        if(Break_Block.size() > 0)
                                                        {
                                                                bool same_object = false;
                                                                for(GameObject *other_box : Break_Block)
                                                                {
                                                                        if(&box == other_box)
                                                                        {
                                                                                same_object = true;
                                                                                break;
                                                                        }
                                                                }
                                                                if(!same_object)
                                                                {
                                                                        Break_Block_Timer.push_back(0.0);
                                                                        Break_Block.push_back(&box);
                                                                }
                                                        }
                                                        else
                                                        {
                                                                Break_Block_Timer.push_back(0.0);
                                                                Break_Block.push_back(&box);
                                                        }
                                                }

                                                return true;
                                        }
                                }
                                if (Physics.left_collision(*Player, box) && box.destroyed != GL_TRUE  && box.less_gravity == 0)
                                {
                                        if(!Physics.is_grounded)
                                                on_wall_left = true;
                                        if(box.death)
                                                this->State = GAME_DEATH;
                                        else
                                                Player->Position.x = box.Position.x + box.Size.x;

                                        if(box.Break != 0)
                                        {
                                                if(Break_Block.size() > 0)
                                                {
                                                        bool same_object = false;
                                                        for(GameObject *other_box : Break_Block)
                                                        {
                                                                if(&box == other_box)
                                                                {
                                                                        same_object = true;
                                                                        break;
                                                                }
                                                        }
                                                        if(!same_object)
                                                        {
                                                                Break_Block_Timer.push_back(0.0);
                                                                Break_Block.push_back(&box);
                                                        }
                                                }
                                                else
                                                {
                                                        Break_Block_Timer.push_back(0.0);
                                                        Break_Block.push_back(&box);
                                                }
                                        }
                                        return true;
                                }
                                else
                                {
                                        GameObject temp_player = *Player;
                                        temp_player.Position.x -= 1;
                                        if (Physics.left_collision(temp_player, box) && box.destroyed != GL_TRUE  && box.less_gravity == 0)
                                        {
                                                if(box.death)
                                                        this->State = GAME_DEATH;
                                                if(!Physics.is_grounded)
                                                        on_wall_left = true;

                                                if(box.Break != 0)
                                                {
                                                        if(Break_Block.size() > 0)
                                                        {
                                                                bool same_object = false;
                                                                for(GameObject *other_box : Break_Block)
                                                                {
                                                                        if(&box == other_box)
                                                                        {
                                                                                same_object = true;
                                                                                break;
                                                                        }
                                                                }
                                                                if(!same_object)
                                                                {
                                                                        Break_Block_Timer.push_back(0.0);
                                                                        Break_Block.push_back(&box);
                                                                }
                                                        }
                                                        else
                                                        {
                                                                Break_Block_Timer.push_back(0.0);
                                                                Break_Block.push_back(&box);
                                                        }
                                                }

                                                return true;
                                        }
                                }
                        }
                }
        }
        on_wall_left = false;
        on_wall_right = false;
        return false;
}

void Game::Load_Level(const GLchar *file)
{
        temp_lvl.clear();
        std::ifstream stream_2(file);
        double code;
        int in_block = 0;
        int block = 0;
        bool same_x;
        bool same_y;
        std::string line;
        if(stream_2)
        {
                while (std::getline(stream_2, line)) // Read each line from level file
                {
                        GameObject temp_obj;
                        bool arcade = false;
                        temp_obj.arcade = false;
                        std::istringstream sstream(line);
                        while (sstream >> code)
                        {
                                if(in_block == 0)
                                {
                                        temp_obj.Sprite = block_type[code];
                                        if(code == 1)
                                                temp_obj.arcade = true;
                                        if(code == 0)
                                                temp_obj.boost = true;
                                }
                                if(in_block == 1)
                                        temp_obj.Position.x = code;
                                if(in_block == 2)
                                        temp_obj.Position.y = code;
                                if(in_block == 3)
                                        temp_obj.Size.x = code;
                                if(in_block == 4)
                                        temp_obj.Size.y = code;
                                if(in_block == 5)
                                        temp_obj.death = code;
                                if(in_block == 6)
                                {
                                        //temp_obj.boost = code;
                                }
                                if(in_block == 7)
                                        temp_obj.less_gravity = code;
                                if(in_block == 8)
                                        temp_obj.Rotation = code;
                                if(in_block == 9)
                                        temp_obj.blink_show = (double)code;
                                if(in_block == 10)
                                        temp_obj.blink_hide = (double)code;
                                if(in_block == 11)
                                        temp_obj.Color.x = (double)code;
                                if(in_block == 12)
                                        temp_obj.Color.y = (double)code;
                                if(in_block == 13)
                                        temp_obj.Color.z = (double)code;
                                if(in_block == 14)
                                        temp_obj.Movement.x = code;
                                if(in_block == 15)
                                        temp_obj.Movement.y = code;
                                if(in_block == 16)
                                        temp_obj.Break = code;

                                in_block++;
                        }
                        in_block = 0;
                        block++;

                        temp_lvl.push_back(temp_obj);
                }
        }
        this->Levels[this->Level].Bricks = temp_lvl;
}


void Game::Load_Levels()
{
        this->Level = 0;
        this->Sokoban_Level = 0;

        this->Levels.clear();
        for(int i = 1; i <= level_count; i++)
        {
                GameLevel temp;
                //std::string a = "../new_level_type/" + std::to_string(i) + ".lvl";
                //temp.Load(a.c_str(), 1400, 900);*/
                std::ifstream read_level;
                read_level.open("../new_level_type/" + std::to_string(i - 1) + "_blocks.lvl");
                int number_of_blocks;
                int temp2;
                while(read_level >> number_of_blocks)
                {
                        temp2 = number_of_blocks;
                }
                read_level.close();
                number_blocks.push_back(temp2);
                std::string lvl = "../new_level_type/" + std::to_string(i - 1) + ".lvl";
                //Load_Level(lvl.c_str());
                this->Levels.push_back(temp);
        }

        std::cout << "Normal Levels: " << this->Levels.size() << std::endl;

        Load_Sokoban_Levels();

        std::cout << "Sokoban Levels: " << this->Sokoban_Levels.size() << std::endl;
}
void Game::Load_Sokoban_Levels()
{
        Sokoban_Levels.clear();
        for(int i = 1; i <= level_count; i++)
        {
                GameLevel temp;
                std::string lvl = "../sokoban_levels/" + std::to_string(i) + ".lvl";
                temp.Sokoban_Load(lvl.c_str(), 1000, 1000, i);
                this->Sokoban_Levels.push_back(temp);
        }
}

void Game::Load_Sokoban_Goal()
{
        delete Sokoban_Player;
        glm::vec2 sokoban_player_pos = sokoban_spawn_point[this->Sokoban_Level];
        glm::vec2 sokoban_player_size = glm::vec2(50, 50);

        Sokoban_Player = new GameObject(sokoban_player_pos, sokoban_player_size, ResourceManager::GetTexture("sokoban_player"));

        sokoban_goal.clear();
        for(int i = 0; i < 20; i++)
                for(int a = 0; a < 20; a++)
                        if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[i][a] == 2)
                                sokoban_goal.push_back(glm::vec2(i, a));
}

void Game::setup_blocks()
{
        block_type.clear();
        Texture2D player;
        player = ResourceManager::GetTexture("player");
        block_type.push_back(player);
        Texture2D arcade;
        arcade = ResourceManager::GetTexture("arcade");
        block_type.push_back(arcade);
        Texture2D stone;
        stone = ResourceManager::GetTexture("stone");
        block_type.push_back(stone);
        Texture2D monster;
        monster = ResourceManager::GetTexture("monster");
        block_type.push_back(monster);
        Texture2D grass;
        grass = ResourceManager::GetTexture("grass_block");
        block_type.push_back(grass);
        Texture2D spikes;
        spikes = ResourceManager::GetTexture("spikes");
        block_type.push_back(spikes);
        Texture2D only_dirt;
        only_dirt = ResourceManager::GetTexture("only_dirt");
        block_type.push_back(only_dirt);
        Texture2D metal;
        metal = ResourceManager::GetTexture("metal_block");
        block_type.push_back(metal);
        Texture2D fleip_grass;
        fleip_grass = ResourceManager::GetTexture("fleip_grass");
        block_type.push_back(fleip_grass);
        Texture2D saws;
        saws = ResourceManager::GetTexture("saws");
        block_type.push_back(saws);
}

void Game::draw_map(SpriteRenderer &Renderer)
{
        Texture2D blank;
        blank = ResourceManager::GetTexture("blank");
        Renderer.DrawSprite(blank, glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f, glm::vec3(0.5f, 0.5f, 0.5f));

        int start_x_pos = 75;
        int y_pos = 75;
        int a = 0;
        int square_levels = 1;
next_row:
        for(a = start_x_pos; a < this->Width - 75; a += 75)
        {
                Renderer.DrawSprite(blank, glm::vec2(a, y_pos), glm::vec2(50, 50), 0.0f, glm::vec3(0.8f, 0.0f, 0.0f));
                Text->RenderText(std::to_string(square_levels), a + 5, y_pos + 5, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
                square_levels++;
                if(square_levels > level_count)
                        break;
        }
        if(square_levels < level_count)
        {
                y_pos += 75;
                goto next_row;
        }
}


void Game::draw_map_cursor(SpriteRenderer &Renderer)
{
        int multiplier = 0;
        if(point % 17 == 0)
                multiplier = point / 17;
        if(point < 17)
        {
                map_cursor.x = 75 + 75 * point;
                map_cursor.y = 75;
        }
        else
        {
                map_cursor.x = ((point % 17) * 75) + 75;
                map_cursor.y = 75 + (int(point / 17) * 75);
        }
        Texture2D pointer;
        pointer = ResourceManager::GetTexture("arrow");
        Renderer.DrawSprite(pointer, glm::vec2(map_cursor.x + 10, map_cursor.y - 30), glm::vec2(30, 30), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
}

std::vector<bool> blinked;
std::vector<bool> moved;
std::vector<glm::vec2> move_y_block;
std::vector<glm::vec2> move_x_block;

void Game::Load_Block_Functions()
{
        Break_Block.clear();
        Less_Gravity_Block.clear();
        Blink_Block.clear();
        Moving_Blocks.clear();
        for(GameObject &box : this->Levels[this->Level].Bricks)
        {
                if(box.less_gravity != 0)
                {
                        Less_Gravity_Block.push_back(&box);
                }
                if(box.blink_show != 0.0)
                {
                        Blink_Block.push_back(&box);
                        blink_timer.push_back(box.blink_show);
                        blinked.push_back(false);
                }
                if(box.boost != 0)
                {
                        spawn_point.x = box.Position.x;
                        spawn_point.y = box.Position.y;
                }
                if(box.Movement.x != 0 || box.Movement.y != 0)
                {
                        Moving_Blocks.push_back(&box);
                        move_y_block.push_back(glm::vec2(box.Position.y, box.Movement.y + box.Position.y));
                        move_x_block.push_back(glm::vec2(box.Position.x, box.Movement.x + box.Position.x));
                        moved.push_back(false);
                }
        }
}

void Game::Run_Block_Functions(double dt)
{
        int break_count = 0;
        for(GameObject *box : Break_Block)
        {
                Break_Block_Timer[break_count] += dt;

                if(Break_Block_Timer[break_count] >= Break_Block[break_count]->Break)
                {
                        Break_Block.erase(Break_Block.begin() + break_count);
                        Break_Block_Timer.erase(Break_Block_Timer.begin() + break_count);
                        box->destroyed = true;
                }

                break_count++;
        }

        for(GameObject *box : Less_Gravity_Block)
        {
                box->Rotation += dt * 6.0;
                if(box->Rotation >= 3.1415f)
                        box->Rotation = -3.14f;
        }

        int moving_blocks_count = 0;
        int block_speed = 100;
        for(GameObject *box : Moving_Blocks)
        {
                if(this->Level == 2)
                        block_speed = 50;
                if(move_x_block[moving_blocks_count].y != box->Position.x)
                {
                        if(!moved[moving_blocks_count] && box->Position.x < move_x_block[moving_blocks_count].y)
                                box->Position.x += block_speed * dt;
                        else if(!moved[moving_blocks_count] && box->Position.x >= move_x_block[moving_blocks_count].y)
                                moved[moving_blocks_count] = true;
                        if(moved[moving_blocks_count] && box->Position.x >= move_x_block[moving_blocks_count].x)
                                box->Position.x -= block_speed * dt;
                        else if(moved[moving_blocks_count] && box->Position.x <= move_x_block[moving_blocks_count].x)
                                moved[moving_blocks_count] = false;
                }
                if(move_y_block[moving_blocks_count].y != box->Position.y)
                {
                        if(!moved[moving_blocks_count] && box->Position.y < move_y_block[moving_blocks_count].y)
                                box->Position.y += block_speed * dt;
                        else if(!moved[moving_blocks_count] && box->Position.y >= move_y_block[moving_blocks_count].y)
                                moved[moving_blocks_count] = true;
                        if(moved[moving_blocks_count] && box->Position.y >= move_y_block[moving_blocks_count].x)
                                box->Position.y -= block_speed * dt;
                        else if(moved[moving_blocks_count] && box->Position.y <= move_y_block[moving_blocks_count].x)
                                moved[moving_blocks_count] = false;
                }
                moving_blocks_count++;
        }

        int blink_blocks_count = 0;
        for(GameObject *box : Blink_Block)
        {
                blink_timer[blink_blocks_count] += dt;
                if(blink_timer[blink_blocks_count] >= box->blink_show && !blinked[blink_blocks_count])
                {
                        blinked[blink_blocks_count] = true;
                        box->destroyed = true;
                        blink_timer[blink_blocks_count] = 0.0;
                }
                if(blinked[blink_blocks_count] && blink_timer[blink_blocks_count] >= box->blink_hide)
                {
                        blinked[blink_blocks_count] = false;
                        box->destroyed = false;
                        blink_timer[blink_blocks_count] = 0.0;
                }

                blink_blocks_count++;
        }
}
