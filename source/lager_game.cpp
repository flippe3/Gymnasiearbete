#include <iostream>
#include "lager_game.h"
 
SpriteRenderer *Lager_Renderer;
TextRenderer   *Lager_Text;
Lager_GameObject     *Lager_Player;

glm::vec2 start_pos(0, 0);
glm::vec2 end_pos(0, 0);
bool start_moving = true;
glm::vec2 spawn_point(0, 0);
double a_timer = 0.0;
double w_timer = 0.0;
double d_timer = 0.0;
double s_timer = 0.0;
double r_timer = 0.0;
bool box_move_down = false;
bool box_move_up = false;
bool box_move_left = false;
bool box_move_right = false;
bool done_color = false;
bool undone_color = false;
int move_x = 0;
int move_y = 0;
std::vector<glm::vec2> goal;
int steps = 0;
int pushes = 0;
double level_time = 0.0;
int time_round = 0;
Lager_Game::Lager_Game(GLuint width, GLuint height) :
    Lager_State(LAGER_GAME_ACTIVE), Keys(), Width(width), Height(height){}

Lager_Game::~Lager_Game()
{
    delete Lager_Renderer;
    delete Lager_Text;
    delete Lager_Player;
}

void Lager_Game::Init()
{
    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");
    ResourceManager::LoadShader("shaders/grid.vs", "shaders/grid.frag", nullptr, "grid");

    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

    Shader mainShader;
    mainShader = ResourceManager::GetShader("sprite");
    Lager_Renderer = new SpriteRenderer(mainShader);

    ResourceManager::LoadTexture("textures/character.png", GL_TRUE, "player");
    ResourceManager::LoadTexture("textures/grey.jpg", GL_FALSE, "background");
    ResourceManager::LoadTexture("textures/wall.png", GL_TRUE, "wall");
    ResourceManager::LoadTexture("textures/box.png", GL_TRUE, "box");
    ResourceManager::LoadTexture("textures/middle.png", GL_TRUE, "middle");
 
    Lager_Text = new TextRenderer(this->Width, this->Height);
    Lager_Text->Load("fonts/Pixeled.ttf", 20);

    this->Level = 0;
    Lager_GameLevel temp;
    temp.Load("levels/1.lvl", 1000, 1000);
    this->Levels.push_back(temp);
    this->mov = still;

    glm::vec2 playerPos = glm::vec2(spawn_point.x, spawn_point.y);
    glm::vec2 playerSize = glm::vec2(50, 50);

    Lager_Player = new Lager_GameObject(playerPos, playerSize, ResourceManager::GetTexture("player"));

    steps = 0;
    level_time = 0;
    pushes = 0;
    time_round = 0;

    for(int i = 0; i < 20; i++)
        for(int a = 0; a < 20; a++)
            if(this->Levels[this->Level].tileData[i][a] == 2)
                goal.push_back(glm::vec2(i, a));
}

void Lager_Game::Update(double dt)
{
    level_time += dt;
    time_round = (int)level_time;
    //Character Collision
    if(this->mov == up)
    {
        if(this->Levels[this->Level].tileData[(Lager_Player->Position.y / 50) - 1][Lager_Player->Position.x / 50] == 3)
        {
            pushes++;
            move_y = Lager_Player->Position.y - 100;
            move_x = Lager_Player->Position.x;
            for(int i = 0; i < goal.size(); i++)
            {
                if((move_y / 50) == goal[i].x && (move_x / 50) == goal[i].y)
                    done_color = true;
            }
            if(!done_color)
                undone_color = true;
            box_move_up = true;
            this->Levels[this->Level].tileData[(Lager_Player->Position.y / 50) - 1][Lager_Player->Position.x / 50] = 0;
            this->Levels[this->Level].tileData[(Lager_Player->Position.y / 50) - 2][Lager_Player->Position.x / 50] = 3;

        }
        this->Levels[this->Level].tileData[Lager_Player->Position.y / 50][Lager_Player->Position.x / 50] = 0;
        Lager_Player->Position.y -= 50;
        this->Levels[this->Level].tileData[Lager_Player->Position.y / 50][Lager_Player->Position.x / 50] = 4;

        this->mov = still;
    }
    if(this->mov == down)
    {
        if(this->Levels[this->Level].tileData[(Lager_Player->Position.y / 50) + 1][Lager_Player->Position.x / 50] == 3)
        {
            pushes++;
            move_y = Lager_Player->Position.y + 100;
            move_x = Lager_Player->Position.x;
            for(int i = 0; i < goal.size(); i++)
            {
                if((move_y / 50) == goal[i].x && (move_x / 50) == goal[i].y)
                    done_color = true;
            }
            if(!done_color)
                undone_color = true;
            
            box_move_down = true;
            this->Levels[this->Level].tileData[(Lager_Player->Position.y / 50) + 1][Lager_Player->Position.x / 50] = 0;
            this->Levels[this->Level].tileData[(Lager_Player->Position.y / 50) + 2][Lager_Player->Position.x / 50] = 3;
        }
        this->Levels[this->Level].tileData[Lager_Player->Position.y / 50][Lager_Player->Position.x / 50] = 0;
        Lager_Player->Position.y += 50;
        this->Levels[this->Level].tileData[Lager_Player->Position.y / 50][Lager_Player->Position.x / 50] = 4;

        this->mov = still;
    }
    if(this->mov == right)
    {
        if(this->Levels[this->Level].tileData[Lager_Player->Position.y / 50][(Lager_Player->Position.x / 50) + 1] == 3)
        {
            pushes++;
            move_y = Lager_Player->Position.y;
            move_x = Lager_Player->Position.x + 100;
            for(int i = 0; i < goal.size(); i++)
            {
                if((move_y / 50) == goal[i].x && (move_x / 50) == goal[i].y)
                    done_color = true;
            }
            if(!done_color)
                undone_color = true;

            box_move_right = true;
            this->Levels[this->Level].tileData[Lager_Player->Position.y / 50][(Lager_Player->Position.x / 50) + 1] = 0;
            this->Levels[this->Level].tileData[Lager_Player->Position.y / 50][(Lager_Player->Position.x / 50) + 2] = 3;
        }

        this->Levels[this->Level].tileData[Lager_Player->Position.y / 50][Lager_Player->Position.x / 50] = 0;
        Lager_Player->Position.x += 50;
        this->Levels[this->Level].tileData[Lager_Player->Position.y / 50][Lager_Player->Position.x / 50] = 4;

        this->mov = still;
    }
    if(this->mov == left)
    {
        if(this->Levels[this->Level].tileData[Lager_Player->Position.y / 50][(Lager_Player->Position.x / 50) - 1] == 3)
        {
            pushes++;
            move_y = Lager_Player->Position.y;
            move_x = Lager_Player->Position.x - 100;
            for(int i = 0; i < goal.size(); i++)
            {
                if((move_y / 50) == goal[i].x && (move_x / 50) == goal[i].y)
                    done_color = true;
            }
            if(!done_color)
                undone_color = true;
            
            box_move_left = true;
            this->Levels[this->Level].tileData[Lager_Player->Position.y / 50][(Lager_Player->Position.x / 50) - 1] = 0;
            this->Levels[this->Level].tileData[Lager_Player->Position.y / 50][(Lager_Player->Position.x / 50) - 2] = 3;
        }
        this->Levels[this->Level].tileData[Lager_Player->Position.y / 50][Lager_Player->Position.x / 50] = 0;
        Lager_Player->Position.x -= 50;
        this->Levels[this->Level].tileData[Lager_Player->Position.y / 50][Lager_Player->Position.x / 50] = 4;

        this->mov = still;
    }

    for(Lager_GameObject &box : this->Levels[this->Level].Bricks)
    {
        if(box.moving_block)
        {
            if(box_move_up)
            {
                if(box.Position.y == move_y + 50 && box.Position.x == move_x)
                {
                    box.Position.y = move_y;
                    box_move_up = false;
                    if(done_color)
                        box.Color.x = 0.0;
                    if(undone_color)
                        box.Color.x = 1.0;

                    undone_color = false;
                    done_color = false;
                }
            }
            if(box_move_down)
            {
                if(box.Position.y == move_y - 50 && box.Position.x == move_x)
                {
                    box.Position.y = move_y;
                    box_move_down = false;
                    if(done_color)
                        box.Color.x = 0.0;
                    if(undone_color)
                        box.Color.x = 1.0;

                    undone_color = false;
                    done_color = false;

                }
            }
            if(box_move_right)
            {
                if(box.Position.x == move_x - 50 && box.Position.y == move_y)
                {
                    box.Position.x = move_x;
                    box_move_right = false;
                    if(done_color) 
                        box.Color.x = 0.0;
                    if(undone_color)
                        box.Color.x = 1.0;

                    undone_color = false;
                    done_color = false;
                }
            }
            if(box_move_left)
            {
                if(box.Position.x == move_x + 50 && box.Position.y == move_y)
                {
                    box.Position.x = move_x;
                    box_move_left = false;
                    if(done_color)
                        box.Color.x = 0.0;
                    if(undone_color)
                        box.Color.x = 1.0;
                    undone_color = false;
                    done_color = false;
                }
            }
        }
    }
}


void Lager_Game::Render()
{
    Texture2D myTexture;
    myTexture = ResourceManager::GetTexture("background");
    Lager_Renderer->DrawSprite(myTexture, glm::vec2(0, 0), glm::vec2(this->Width, this->Height + 45), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

    //Draw Lager_Player
    Lager_Player->Draw(*Lager_Renderer);
    //Draw Level
    this->Levels[this->Level].Draw(*Lager_Renderer);
    Lager_Text->RenderText("steps: " + std::to_string(steps) + "     pushes: " + std::to_string(pushes) + "     time: " + std::to_string(time_round), 0, this->Height - 25, 1.0f, glm::vec3(1.0, 1.0, 1.0));
    //Draw Grid
    //Draw_Grid(*Lager_Renderer, 0, 0, 50, 50, 25, 25);
}

void Lager_Game::Input(double dt)
{
    if(this->mov == still)
    {
        a_timer += dt;
        d_timer += dt;
        w_timer += dt;
        s_timer += dt;
        r_timer += dt;
        if(this->Keys[GLFW_KEY_A] && a_timer >= 0.2)
        {
            if(check_left(*Lager_Player))
            {
                this->mov = left;
                Lager_Player->Rotation = -3.14159265359 / 2;
                a_timer = 0.0;
                steps++;
            }
        }
        else if(this->Keys[GLFW_KEY_D] && d_timer >= 0.2)
        {
            if(check_right(*Lager_Player))
            {
                this->mov = right;
                Lager_Player->Rotation = 3.14159265359 / 2;
                d_timer = 0.0;
                steps++;
            }
        }
        else if(this->Keys[GLFW_KEY_W] && w_timer >= 0.2)
        {
            if(check_up(*Lager_Player))
            {
                this->mov = up;
                Lager_Player->Rotation = 0.0;
                w_timer = 0.0;
                steps++;
            }
        }
        else if(this->Keys[GLFW_KEY_S] && s_timer >= 0.2)
        {
            if(check_down(*Lager_Player))
            {
                this->mov = down;
                Lager_Player->Rotation = 3.14159265359;
                s_timer = 0.0;
                steps++;
            }
        }
        else if(this->Keys[GLFW_KEY_R] && r_timer >= 0.2)
        {
            this->Levels.clear();
            this->Init();
        }
        if(a_timer >= 10.0)
            a_timer = 1.0;
        if(w_timer >= 10.0)
            w_timer = 1.0;
        if(d_timer >= 10.0)
            d_timer = 1.0;
        if(s_timer >= 10.0)
            s_timer = 1.0;
        if(r_timer >= 10.0)
            r_timer = 1.0;
    }
    if(this->Keys[GLFW_KEY_ESCAPE])
        this->game_exit = true;
}

void Lager_Game::Draw_Grid(SpriteRenderer &Renderer, int start_x, int start_y, int square_width, int square_height, int num_x, int num_y)
{
    Texture2D grid;
    grid = ResourceManager::GetTexture("grey");
    for(int i = 0; i <= square_width * num_x; i += square_width)
        Renderer.DrawSprite(grid, glm::vec2(i, 0), glm::vec2(1, square_width * num_y), 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));
    for(int i = 0; i <= square_height * num_y; i += square_height)
        Renderer.DrawSprite(grid, glm::vec2(0, i), glm::vec2(square_height * num_x, 1), 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));
}
bool Lager_Game::check_left(Lager_GameObject &player)
{
    if(this->Levels[this->Level].tileData[Lager_Player->Position.y / 50][(Lager_Player->Position.x / 50) - 1] == 1)
        return false;
    if(this->Levels[this->Level].tileData[Lager_Player->Position.y / 50][(Lager_Player->Position.x / 50) - 1] == 3)
        if(this->Levels[this->Level].tileData[Lager_Player->Position.y / 50][(Lager_Player->Position.x / 50) - 2] == 3)
            return false;
    if(this->Levels[this->Level].tileData[Lager_Player->Position.y / 50][(Lager_Player->Position.x / 50) - 1] == 3)
        if(this->Levels[this->Level].tileData[Lager_Player->Position.y / 50][(Lager_Player->Position.x / 50) - 2] == 1)
            return false;
    return true;
}
bool Lager_Game::check_right(Lager_GameObject &player)
{
    if(this->Levels[this->Level].tileData[Lager_Player->Position.y / 50][(Lager_Player->Position.x / 50) + 1] == 1)
        return false;
    if(this->Levels[this->Level].tileData[Lager_Player->Position.y / 50][(Lager_Player->Position.x / 50) + 1] == 3)
        if(this->Levels[this->Level].tileData[Lager_Player->Position.y / 50][(Lager_Player->Position.x / 50) + 2] == 3)
            return false;
    if(this->Levels[this->Level].tileData[Lager_Player->Position.y / 50][(Lager_Player->Position.x / 50) + 1] == 3)
        if(this->Levels[this->Level].tileData[Lager_Player->Position.y / 50][(Lager_Player->Position.x / 50) + 2] == 1)
            return false;
    return true;
}
bool Lager_Game::check_down(Lager_GameObject &player)
{
    if(this->Levels[this->Level].tileData[(Lager_Player->Position.y / 50) + 1][Lager_Player->Position.x / 50] == 1)
        return false;
    if(this->Levels[this->Level].tileData[(Lager_Player->Position.y / 50) + 1][Lager_Player->Position.x / 50] == 3)
        if(this->Levels[this->Level].tileData[(Lager_Player->Position.y / 50) + 2][Lager_Player->Position.x / 50] == 3)
            return false;
    if(this->Levels[this->Level].tileData[(Lager_Player->Position.y / 50) + 1][Lager_Player->Position.x / 50] == 3)
        if(this->Levels[this->Level].tileData[(Lager_Player->Position.y / 50) + 2][Lager_Player->Position.x / 50] == 1)
            return false;
    return true;
}
bool Lager_Game::check_up(Lager_GameObject &player)
{
    if(this->Levels[this->Level].tileData[(Lager_Player->Position.y / 50) - 1][Lager_Player->Position.x / 50] == 1)
        return false;
    if(this->Levels[this->Level].tileData[(Lager_Player->Position.y / 50) - 1][Lager_Player->Position.x / 50] == 3)
        if(this->Levels[this->Level].tileData[(Lager_Player->Position.y / 50) - 2][Lager_Player->Position.x / 50] == 3)
            return false;
    if(this->Levels[this->Level].tileData[(Lager_Player->Position.y / 50) - 1][Lager_Player->Position.x / 50] == 3)
        if(this->Levels[this->Level].tileData[(Lager_Player->Position.y / 50) - 2][Lager_Player->Position.x / 50] == 1)
            return false;
    return true;
}
