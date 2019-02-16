#include "game.h"

extern SpriteRenderer *Renderer;
extern TextRenderer   *Text;
extern GameObject *Sokoban_Player;
extern std::vector<glm::vec2> sokoban_spawn_point;
extern std::vector<glm::vec2> sokoban_goal;
extern bool on_arcade;
extern int steps;
extern int pushes;
extern int time_round;
extern double level_time;
/*
  glm::vec2 start_pos(0, 0);
  glm::vec2 end_pos(0, 0);
  bool start_moving = true;
  std::vector<glm::vec2> spawn_point;
  double new_level = 0.0;
  std::vector<glm::vec2> sokoban_goal;
*/

int move_x = 0;
int move_y = 0;
bool done_color = false;
bool undone_color = false;
bool box_move_down = false;
bool box_move_up = false;
bool box_move_left = false;
bool box_move_right = false;
double aa_timer = 0.0;
double ww_timer = 0.0;
double dd_timer = 0.0;
double rr_timer = 0.0;
double ss_timer = 0.0;
double new_level = 0.0;
int should_go = 0;
int should_go_box = 0;
int saved_box_pos = 0;
bool first_left = false;
bool first_right = false;
bool first_up = false;
bool first_down = false;
int pushed_block = 0;
GameObject fake;
bool box_first_move = false;

int frequency = 0;
void Game::sokoban_update(double dt)
{
    /*
    if(frequency == 500)
    {
        for(int i = 0; i < 20; i++)
        {
            for(int a = 0; a < 20; a++)
            {
                std::cout << this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[i][a] << " ";
            }
            std::cout << "\n";
        }
        std::cout << "**************************************" << std::endl;
        frequency = 0;
        std::cout << "TileData: " << this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData.size() << std::endl;
        std::cout << "Bricks: " << this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks.size() << std::endl;
    }
    frequency++;
    */
    //std::cout << Sokoban_Player->Position.x << "\t" << Sokoban_Player->Position.y << std::endl;
    //Is the error in the actual tiledata????
    if(sokoban_check_win())
    {
        this->Sokoban_State = SOKOBAN_WIN;
    }
    if(this->Sokoban_State == SOKOBAN_ACTIVE)
    {
        level_time += dt;
        time_round = (int)level_time;
        //Character Collision
        if(this->mov == up)
        {
            if(first_up)
            {
                this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[Sokoban_Player->Position.y / 50][Sokoban_Player->Position.x / 50] = 0;
                should_go = Sokoban_Player->Position.y - 50;
                should_go_box = should_go - 50;
                first_up = false;
                if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[(Sokoban_Player->Position.y / 50) - 1][Sokoban_Player->Position.x / 50] == 3)
                {
                    pushed_block = 1;
                    pushes++;
                    move_y = Sokoban_Player->Position.y - 100;
                    move_x = Sokoban_Player->Position.x;
                    saved_box_pos = move_y + 50;

                    for(int i = 0; i < sokoban_goal.size(); i++)
                    {
                        if((move_y / 50) == sokoban_goal[i].x && (move_x / 50) == sokoban_goal[i].y)
                            done_color = true;
                    }
                    if(!done_color)
                        undone_color = true;
                    box_first_move = true;
                    box_move_up = true;
                }
            }
            if(Sokoban_Player->Position.y > should_go)
            {
                Sokoban_Player->Position.y -= 300 * dt;
            }
            else
            {
                //pushed_block = 2;
                Sokoban_Player->Position.y = should_go;
                should_go_box = Sokoban_Player->Position.y - 50;

                this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[Sokoban_Player->Position.y / 50][Sokoban_Player->Position.x / 50] = 4;
                this->mov = still;
            }
        }
        if(this->mov == down)
        {
            if(first_down)
            {
                this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[Sokoban_Player->Position.y / 50][Sokoban_Player->Position.x / 50] = 0;
                should_go = Sokoban_Player->Position.y + 50;
                should_go_box = should_go + 50;
                first_down = false;
                if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[(Sokoban_Player->Position.y / 50) + 1][Sokoban_Player->Position.x / 50] == 3)
                {
                    pushed_block = 1;
                    pushes++;
                    move_y = Sokoban_Player->Position.y + 100;
                    move_x = Sokoban_Player->Position.x;
                    saved_box_pos = move_y - 50;

                    for(int i = 0; i < sokoban_goal.size(); i++)
                    {
                        if((move_y / 50) == sokoban_goal[i].x && (move_x / 50) == sokoban_goal[i].y)
                            done_color = true;
                    }
                    if(!done_color)
                        undone_color = true;

                    box_move_down = true;
                    box_first_move = true;
                }
            }
            if(Sokoban_Player->Position.y < should_go)
                Sokoban_Player->Position.y += 300 * dt;
            else
            {
                Sokoban_Player->Position.y = should_go;
                should_go_box = Sokoban_Player->Position.y + 50;
                this->mov = still;

                this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[Sokoban_Player->Position.y / 50][Sokoban_Player->Position.x / 50] = 4;
            }
        }
        if(this->mov == right)
        {
            if(first_right)
            {
                this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[Sokoban_Player->Position.y / 50][Sokoban_Player->Position.x / 50] = 0;

                should_go = Sokoban_Player->Position.x + 50;
                should_go_box = should_go + 50;
                first_right = false;
                if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[Sokoban_Player->Position.y / 50][(Sokoban_Player->Position.x / 50) + 1] == 3)
                {
                    pushed_block = 1;
                    pushes++;
                    move_y = Sokoban_Player->Position.y;
                    move_x = Sokoban_Player->Position.x + 100;
                    saved_box_pos = move_x - 50;

                    for(int i = 0; i < sokoban_goal.size(); i++)
                    {
                        if((move_y / 50) == sokoban_goal[i].x && (move_x / 50) == sokoban_goal[i].y)
                            done_color = true;
                    }
                    if(!done_color)
                        undone_color = true;

                    box_first_move = true;
                    box_move_right = true;
                }
            }
            if(Sokoban_Player->Position.x < should_go)
                Sokoban_Player->Position.x += 300 * dt;
            else
            {
                Sokoban_Player->Position.x = should_go;
                should_go_box = Sokoban_Player->Position.x + 50;
                this->mov = still;

                this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[Sokoban_Player->Position.y / 50][Sokoban_Player->Position.x / 50] = 4;
            }
        }
        if(this->mov == left)
        {
            if(first_left)
            {
                this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[Sokoban_Player->Position.y / 50][Sokoban_Player->Position.x / 50] = 0;

                should_go = Sokoban_Player->Position.x - 50;
                should_go_box = should_go - 50;
                first_left = false;
                if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[Sokoban_Player->Position.y / 50][(Sokoban_Player->Position.x / 50) - 1] == 3)
                {
                    pushed_block = 1;
                    pushes++;
                    move_y = Sokoban_Player->Position.y;
                    move_x = Sokoban_Player->Position.x - 100;
                    saved_box_pos = move_x + 50;

                    for(int i = 0; i < sokoban_goal.size(); i++)
                    {
                        if((move_y / 50) == sokoban_goal[i].x && (move_x / 50) == sokoban_goal[i].y)
                            done_color = true;
                    }
                    if(!done_color)
                        undone_color = true;

                    box_move_left = true;
                    box_first_move = true;
                }

            }
            if(Sokoban_Player->Position.x > should_go)
                Sokoban_Player->Position.x -= 300 * dt;
            else
            {
                //pushed_block = 2;
                Sokoban_Player->Position.x = should_go;
                should_go_box = Sokoban_Player->Position.x - 50;
                this->mov = still;

                this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[Sokoban_Player->Position.y / 50][Sokoban_Player->Position.x / 50] = 4;
            }

        }
        for(GameObject &box : this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks)
        {
            if(box.move)
            {
                if(box_move_up)
                {
                    if(box_first_move && box.Position.y == move_y + 50 && box.Position.x == move_x)
                    {
                        fake = box;
                        fake.destroyed = false;
                        box.destroyed = true;
                        this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks.push_back(fake);
                        box_first_move = false;
                    }

                    if((this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks[this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks.size() - 1].Position.y == move_y + 50 || move_y + 50 == saved_box_pos) && this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks[this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks.size() - 1].Position.x == move_x && this->mov != still && !box_first_move)
                    {
                        this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks[this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks.size() - 1].Position.y = Sokoban_Player->Position.y - 50;
                    }

                    if(move_x == box.Position.x && box.Position.y == move_y + 50 && this->mov == still)
                    {
                        box.Position.y = move_y;
                        box.destroyed = false;
                        this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks.pop_back();
                        box_move_up = false;
                        if(done_color)
                            box.Color.x = 0.0;
                        if(undone_color)
                            box.Color.x = 1.0;

                        undone_color = false;
                        done_color = false;

                        this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[(Sokoban_Player->Position.y / 50)][Sokoban_Player->Position.x / 50] = 0;
                        this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[(Sokoban_Player->Position.y / 50) - 1][Sokoban_Player->Position.x / 50] = 3;
                    }
                }
                if(box_move_down)
                {
                    if(box_first_move && box.Position.y == move_y - 50 && box.Position.x == move_x)
                    {
                        fake = box;
                        fake.destroyed = false;
                        box.destroyed = true;
                        this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks.push_back(fake);
                        box_first_move = false;
                    }

                    if((this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks[this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks.size() - 1].Position.y == move_y - 50 || move_y - 50 == saved_box_pos) && this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks[this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks.size() - 1].Position.x == move_x && this->mov != still && !box_first_move)
                    {
                        this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks[this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks.size() - 1].Position.y = Sokoban_Player->Position.y + 50;
                    }

                    if(box.Position.y == move_y - 50 && box.Position.x == move_x  && this->mov == still)
                    {
                        std::cout << "Delete fake" << std::endl;
                        box.Position.y = move_y;
                        box.destroyed = false;
                        this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks.pop_back();
                        box_move_down = false;
                        if(done_color)
                            box.Color.x = 0.0;
                        if(undone_color)
                            box.Color.x = 1.0;

                        undone_color = false;
                        done_color = false;
                        this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[(Sokoban_Player->Position.y / 50)][Sokoban_Player->Position.x / 50] = 0;
                        this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[(Sokoban_Player->Position.y / 50) + 1][Sokoban_Player->Position.x / 50] = 3;
                    }
                }
                if(box_move_right)
                {
                    if(box_first_move && box.Position.x == move_x - 50 && box.Position.y == move_y)
                    {
                        fake = box;
                        fake.destroyed = false;
                        box.destroyed = true;
                        this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks.push_back(fake);
                        box_first_move = false;
                    }

                    if((this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks[this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks.size() - 1].Position.x == move_x - 50 || move_x - 50 == saved_box_pos) && this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks[this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks.size() - 1].Position.y == move_y && this->mov != still  && !box_first_move)
                    {
                        this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks[this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks.size() - 1].Position.x = Sokoban_Player->Position.x + 50;
                    }

                    if(box.Position.x == move_x - 50 && box.Position.y == move_y  && this->mov == still)
                    {
                        box.Position.x = move_x;
                        box.destroyed = false;
                        this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks.pop_back();
                        box.Position.x = move_x;
                        box_move_right = false;
                        if(done_color)
                            box.Color.x = 0.0;
                        if(undone_color)
                            box.Color.x = 1.0;

                        undone_color = false;
                        done_color = false;

                        this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[Sokoban_Player->Position.y / 50][(Sokoban_Player->Position.x / 50)] = 0;
                        this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[Sokoban_Player->Position.y / 50][(Sokoban_Player->Position.x / 50) + 1] = 3;
                    }
                }
                if(box_move_left)
                {
                    if(box_first_move && box.Position.x == move_x + 50 && box.Position.y == move_y)
                    {
                        fake = box;
                        fake.destroyed = false;
                        box.destroyed = true;
                        this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks.push_back(fake);
                        box_first_move = false;
                    }
                    if((this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks[this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks.size() - 1].Position.x == move_x + 50 || move_x + 50 == saved_box_pos) && this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks[this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks.size() - 1].Position.y == move_y && this->mov != still && !box_first_move)
                    {
                        this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks[this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks.size() - 1].Position.x = Sokoban_Player->Position.x - 50;
                    }

                    if(box.Position.x == move_x + 50 && box.Position.y == move_y && this->mov == still)
                    {
                        box.Position.x = move_x;
                        box.destroyed = false;
                        this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks.pop_back();
                        box_move_left = false;
                        if(done_color)
                            box.Color.x = 0.0;
                        if(undone_color)
                            box.Color.x = 1.0;

                        undone_color = false;
                        done_color = false;
                        this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[Sokoban_Player->Position.y / 50][(Sokoban_Player->Position.x / 50)] = 0;
                        this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[Sokoban_Player->Position.y / 50][(Sokoban_Player->Position.x / 50) - 1] = 3;
                    }
                }
            }
/*
  for(GameObject &box : this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Bricks)
  {
  if(box.move)
  {
  if(box_move_up)
  {
  if((box.Position.y == move_y + 50 || move_y + 50 == saved_box_pos) && box.Position.x == move_x)
  {
  //box.Position.y = Sokoban_Player->Position.y - 50;
  }
  if(move_x == box.Position.x && box.Position.y == move_y + 50 && pushed_block == 2)
  {
  if(done_color)
  box.Color.x = 0.0;
  if(undone_color)
  box.Color.x = 1.0;

  box.Position.y = should_go_box;
  box.Position.x = move_x;
  undone_color = false;
  done_color = false;
  box_move_up = false;
  pushed_block = 0;
  std::cout << "Done pushing up" << std::endl;
  }
  }
  if(box_move_down)
  {
  if((box.Position.y == move_y - 50 || move_y - 50 == saved_box_pos) && box.Position.x == move_x)
  {
  //box.Position.y = Sokoban_Player->Position.y + 50;
  }
  if(move_x == box.Position.x && box.Position.y == move_y - 50 && pushed_block == 2)
  {
  if(done_color)
  box.Color.x = 0.0;
  if(undone_color)
  box.Color.x = 1.0;

  box.Position.y = should_go_box;
  box.Position.x = move_x;

  undone_color = false;
  done_color = false;
  box_move_down = false;
  pushed_block = 0;
  std::cout << "Done pushing down" << std::endl;
  }
  }
  if(box_move_right)
  {
  if((box.Position.x == move_x - 50 || move_x - 50 == saved_box_pos) && box.Position.y == move_y)
  {
  //box.Position.x = Sokoban_Player->Position.x + 50;
  }
  if(move_x - 50 == box.Position.x && box.Position.y == move_y && pushed_block == 2)
  {
  if(done_color)
  box.Color.x = 0.0;
  if(undone_color)
  box.Color.x = 1.0;

  box.Position.x = should_go_box;
  box.Position.y = move_y;
  undone_color = false;
  done_color = false;
  box_move_right = false;
  pushed_block = 0;
  std::cout << "Done pushing right" << std::endl;
  }
  }
  if(box_move_left)
  {
  if((box.Position.x == move_x + 50 || move_x + 50 == saved_box_pos) && box.Position.y == move_y)
  //box.Position.x = Sokoban_Player->Position.x - 50;
  if(move_x + 50 == box.Position.x && box.Position.y == move_y && pushed_block == 2)
  {
  if(done_color)
  box.Color.x = 0.0;
  if(undone_color)
  box.Color.x = 1.0;

  box.Position.x = should_go_box;
  box.Position.y = move_y;
  std::cout << "Should_go: " << should_go_box;
  std::cout << "Move_Y" << move_y << std::endl;
  undone_color = false;
  done_color = false;
  box_move_left = false;
  pushed_block = 0;
  std::cout << "Done pushing left" << std::endl;
  }
  }
  }
*/
        }
    }
}
void Game::sokoban_input(double dt)
{
    if(this->Sokoban_State == SOKOBAN_ACTIVE)
    {
        if(this->mov == still)
        {
            aa_timer += dt;
            dd_timer += dt;
            ww_timer += dt;
            ss_timer += dt;
            rr_timer += dt;
            if((this->Keys[GLFW_KEY_A] || this->Keys[GLFW_KEY_LEFT]) && aa_timer >= 0.2)
            {
                if(sokoban_check_left(*Sokoban_Player))
                {
                    this->mov = left;
                    first_left = true;
                    Sokoban_Player->Rotation = -3.14159265359 / 2;
                    aa_timer = 0.0;
                    steps++;
                }
            }
            else if((this->Keys[GLFW_KEY_D] || this->Keys[GLFW_KEY_RIGHT]) && dd_timer >= 0.2)
            {
                if(sokoban_check_right(*Sokoban_Player))
                {
                    this->mov = right;
                    first_right = true;
                    Sokoban_Player->Rotation = 3.14159265359 / 2;
                    dd_timer = 0.0;
                    steps++;
                }
            }
            else if((this->Keys[GLFW_KEY_W] || this->Keys[GLFW_KEY_UP]) && ww_timer >= 0.2)
            {
                if(sokoban_check_up(*Sokoban_Player))
                {
                    this->mov = up;
                    first_up = true;
                    Sokoban_Player->Rotation = 0.0;
                    ww_timer = 0.0;
                    steps++;
                }
            }
            else if((this->Keys[GLFW_KEY_S] || this->Keys[GLFW_KEY_DOWN]) && ss_timer >= 0.2)
            {
                if(sokoban_check_down(*Sokoban_Player))
                {
                    this->mov = down;
                    first_down = true;
                    Sokoban_Player->Rotation = 3.14159265359;
                    ss_timer = 0.0;
                    steps++;
                }
            }
            else if(this->Keys[GLFW_KEY_R] && rr_timer >= 0.2)
            {
                steps = 0;
                pushes = 0;
                time_round = 0.0;
                level_time = 0.0;
                this->Sokoban_Levels.clear();
                this->Load_Sokoban_Levels();
                this->Load_Sokoban_Goal();
            }
            if(aa_timer >= 10.0)
                aa_timer = 1.0;
            if(ww_timer >= 10.0)
                ww_timer = 1.0;
            if(dd_timer >= 10.0)
                dd_timer = 1.0;
            if(ss_timer >= 10.0)
                ss_timer = 1.0;
            if(rr_timer >= 10.0)
                rr_timer = 1.0;
        }
    }
    if(this->Sokoban_State == SOKOBAN_WIN)
    {
        new_level += dt;
        if(this->Keys[GLFW_KEY_R])
        {
            this->Sokoban_Levels.clear();
            this->Load_Sokoban_Goal();
            this->Init();
        }
        if(this->Keys[GLFW_KEY_A] && new_level >= 0.2)
        {
            this->Sokoban_Levels.clear();
            this->Sokoban_Level++;
            if(this->Levels.size() - 1 > this->Level)
            {
                this->Level++;
                std::string new_level = "../new_level_type/" + std::to_string(this->Level) + ".lvl";
                Load_Level(new_level.c_str());
                new_level = 0.0;
                Reset_Player();
                on_arcade = false;
                this->State = GAME_ACTIVE;
                this->Sokoban_State = SOKOBAN_ACTIVE;
                Load_Sokoban_Goal();
            } 
            else
            {
                this->State = GAME_WIN;
            }
            if(new_level >= 10.0)
                new_level = 2.0;
        }
        if(this->Keys[GLFW_KEY_ESCAPE])
            this->Sokoban_State = SOKOBAN_ACTIVE;
    }
}

void Game::sokoban_render()
{
    if(this->Sokoban_State == SOKOBAN_ACTIVE)
    {
        Texture2D myTexture;
        myTexture = ResourceManager::GetTexture("sokoban_wallpaper");
        Renderer->DrawSprite(myTexture, glm::vec2(0, 0), glm::vec2(this->Width, this->Height + 45), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

        //Draw Sokoban_Player
        Sokoban_Player->Draw(*Renderer);
        //Draw Sokoban_Level
        this->Sokoban_Levels[this->Sokoban_Level].Sokoban_Draw(*Renderer);
        Text->RenderText("steps: " + std::to_string(steps) + "     pushes: " + std::to_string(pushes) + "     time: " + std::to_string(time_round), 0, this->Height - 45, 1.0f, glm::vec3(1.0, 1.0, 1.0));
    }
    else if(this->Sokoban_State == SOKOBAN_WIN)
    {
        Text->RenderText("That was easy... Press A to continue", this->Width / 2 - 400, this->Height / 2 - 15, 1.0f, glm::vec3(1.0, 1.0, 1.0));
    }

}

bool Game::sokoban_check_win()
{
    for(int i = 0; i < sokoban_goal.size(); i++)
        if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[sokoban_goal[i].x][sokoban_goal[i].y] != 3)
        {
            return false;
        }
    return true;

}

bool Game::sokoban_check_left(GameObject &player)
{
    if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[Sokoban_Player->Position.y / 50][(Sokoban_Player->Position.x / 50) - 1] == 1)
        return false;
    if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[Sokoban_Player->Position.y / 50][(Sokoban_Player->Position.x / 50) - 1] == 3)
        if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[Sokoban_Player->Position.y / 50][(Sokoban_Player->Position.x / 50) - 2] == 3)
            return false;
    if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[Sokoban_Player->Position.y / 50][(Sokoban_Player->Position.x / 50) - 1] == 3)
        if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[Sokoban_Player->Position.y / 50][(Sokoban_Player->Position.x / 50) - 2] == 1)
            return false;
    return true;

}

bool Game::sokoban_check_right(GameObject &player)
{
    if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[Sokoban_Player->Position.y / 50][(Sokoban_Player->Position.x / 50) + 1] == 1)
        return false;
    if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[Sokoban_Player->Position.y / 50][(Sokoban_Player->Position.x / 50) + 1] == 3)
        if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[Sokoban_Player->Position.y / 50][(Sokoban_Player->Position.x / 50) + 2] == 3)
            return false;
    if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[Sokoban_Player->Position.y / 50][(Sokoban_Player->Position.x / 50) + 1] == 3)
        if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[Sokoban_Player->Position.y / 50][(Sokoban_Player->Position.x / 50) + 2] == 1)
            return false;
    return true;

}

bool Game::sokoban_check_up(GameObject &player)
{
    if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[(Sokoban_Player->Position.y / 50) - 1][Sokoban_Player->Position.x / 50] == 1)
        return false;
    if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[(Sokoban_Player->Position.y / 50) - 1][Sokoban_Player->Position.x / 50] == 3)
        if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[(Sokoban_Player->Position.y / 50) - 2][Sokoban_Player->Position.x / 50] == 3)
            return false;
    if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[(Sokoban_Player->Position.y / 50) - 1][Sokoban_Player->Position.x / 50] == 3)
        if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[(Sokoban_Player->Position.y / 50) - 2][Sokoban_Player->Position.x / 50] == 1)
            return false;
    return true;
}

bool Game::sokoban_check_down(GameObject &player)
{
    if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[(Sokoban_Player->Position.y / 50) + 1][Sokoban_Player->Position.x / 50] == 1)
        return false;
    if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[(Sokoban_Player->Position.y / 50) + 1][Sokoban_Player->Position.x / 50] == 3)
        if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[(Sokoban_Player->Position.y / 50) + 2][Sokoban_Player->Position.x / 50] == 3)
            return false;
    if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[(Sokoban_Player->Position.y / 50) + 1][Sokoban_Player->Position.x / 50] == 3)
        if(this->Sokoban_Levels[this->Sokoban_Level].Sokoban_tileData[(Sokoban_Player->Position.y / 50) + 2][Sokoban_Player->Position.x / 50] == 1)
            return false;
    return true;
}
