#include <algorithm>
#include <iostream>

#include "game.h"
#include <fstream>
#include <sstream>
#include <utility>
#include <iomanip>

//Creating the objects we need
SpriteRenderer    *Renderer;
TextRenderer      *Text;
TextRenderer      *Label_Text;
GameObject        *Arcade;

int level_count = 0;
int point;
glm::vec2 map_cursor(0, 0);
glm::vec2 choose_pos(0, 0);
glm::vec2 delete_pos(50, 0);
glm::vec2 place_pos(95, 0);
glm::vec2 move_pos(140, 0);
glm::vec2 paint_pos(185, 0);
//std::vector<std::pair<int, Texture2D>> block_type;
std::vector<Texture2D> block_type;
double change_level_timer = 0.0;
double map_d_timer = 0.0;
double map_a_timer = 0.0;
double d_timer = 0.0;
double a_timer = 0.0;
int blocks = 5;
glm::vec2 clicked_on_block(-1, -1);
glm::vec2 clicked_on_block_size(-1, -1);

Texture2D selected_block;
int selected_number;
std::vector<GameObject> temp_lvl;
std::vector<std::string> block_functions_text;
std::string text = "0";

std::vector<int> number_blocks;
std::vector<glm::vec2> textbox_pos;
std::vector<std::string> textbox_text;
int selected_textbox = -1;
int num_selected_block = 0;

int old_x = 0;
int old_y = 0;
int old_sprite = 0;
bool print_text_left = false;

bool hide = false;
double hide_timer = 0.0;
bool hide_grid = false;
double hide_grid_timer = 0.0;

double EDITOR_TIME = 0.0;
double reverse_timer = 0.0;

int width_res[12] = {640, 800, 1024, 1152, 1280, 1281, 1280, 1400 ,1440, 1400, 1600, 1920};
int height_res[12] = {480, 600, 768, 768, 720, 854, 960, 900, 960, 1050, 1050, 1080};

//Initializing the constructor
Game::Game(GLuint width, GLuint height) :
    State(PLACE), Keys(), Width(width), Height(height){}

//Free memory when calling the destructor
Game::~Game()
{
    delete Renderer;
    delete Arcade;
    delete Text;
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

    std::cout << "Level_Count: " << level_count << std::endl;

    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");
    //Setup the "camera"
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
    // Configure shaders
    ResourceManager::GetShader("sprite").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

    Text = new TextRenderer(this->Width, this->Height);
    Text->Load("../fonts/Pixeled.ttf", 24);

    Label_Text = new TextRenderer(this->Width, this->Height);
    Label_Text->Load("../fonts/Pixeled.ttf", 10);

    /*this->Size_Width = width_res[3];
    this->Size_Height = height_res[3];
    this->resize = true;
    */
    Shader load_player;
    load_player = ResourceManager::GetShader("sprite");
    Renderer = new SpriteRenderer(load_player);

    Load_Textures();
    Load_Levels();
    selected_block = ResourceManager::GetTexture("stone");

    this->Load_Blocks();
    this->State = PLACE;
    std::string l = "../new_level_type/0.lvl";
    Load_Level(l.c_str());
    selected_number = 2;

    for(int i = 0; i < 114; i++)
    {
        textbox_text.push_back("0");
    }
}

void Game::ProcessInput(double dt)
{
    if(this->State == MAP)
    {
        map_d_timer += dt;
        map_a_timer += dt;
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
        if(this->Keys[GLFW_KEY_ENTER])
        {
            this->Level = point;
            this->State = PLACE;
            std::string level_file = "../new_level_type/" + std::to_string(this->Level) + ".lvl";
            Load_Level(level_file.c_str());
        }

        if(map_d_timer >= 20)
            map_d_timer = 1.0;
        if(map_a_timer >= 20)
            map_a_timer = 1.0;
    }
    else
    {
        d_timer += dt;
        a_timer += dt;
        hide_timer += dt;
        hide_grid_timer += dt;
        /*if((this->Keys[GLFW_KEY_D] || this->Keys[GLFW_KEY_RIGHT]) && d_timer >= 0.2)
        {
            d_timer = 0.0;
        }
        if((this->Keys[GLFW_KEY_A] || this->Keys[GLFW_KEY_LEFT]) && a_timer >= 0.2)
        {
            if(this->State == PLACE)
                this->State == DELETE;
            else if(this->State == DELETE)
                this->State == CHOOSE;
            else if(this->State == MOVE)
                this->State = PLACE;
            else if(this->State == PAINT)
                this->State = MOVE;
            a_timer = 0.0;
        }
        */
        if(this->Keys[GLFW_KEY_H] && hide_timer >= 0.2)
        {
            if(hide)
                hide = false;
            else
                hide = true;
            hide_timer = 0.0;
        }
        if(this->Keys[GLFW_KEY_G] && hide_grid_timer >= 0.2)
        {
            if(hide_grid)
                hide_grid = false;
            else
                hide_grid = true;
            hide_grid_timer = 0.0;
        }
        if(d_timer >= 20)
            d_timer = 1.0;
        if(a_timer >= 20)
            a_timer = 1.0;
        if(hide_timer >= 20)
            hide_timer = 1.0;
        if(hide_grid_timer >= 20)
            hide_grid_timer = 1.0;
    }

    if(this->State == MOVE)
    {
        EDITOR_TIME += dt;
        
        if(this->Keys[GLFW_KEY_0] && EDITOR_TIME >= 0.17)
        {
            text += "0";
            EDITOR_TIME = 0.0f;
        }
        else if(this->Keys[GLFW_KEY_1] && EDITOR_TIME >= 0.17)
        {
            if(text == "0")
                text = "1";
            else
                text += "1";
            EDITOR_TIME = 0.0f;
        }
        else if(this->Keys[GLFW_KEY_2] && EDITOR_TIME >= 0.17)
        {
            if(text == "0")
                text = "2";
            else
                text += "2";
            EDITOR_TIME = 0.0f;
        }
        else if(this->Keys[GLFW_KEY_3] && EDITOR_TIME >= 0.17)
        {
            if(text == "0")
                text = "3";
            else
                text += "3";
            EDITOR_TIME = 0.0f;
        }
        else if(this->Keys[GLFW_KEY_4] && EDITOR_TIME >= 0.17)
        {
            if(text == "0")
                text = "4";
            else
                text += "4";
            EDITOR_TIME = 0.0f;
        }
        else if(this->Keys[GLFW_KEY_5] && EDITOR_TIME >= 0.17)
        {
            if(text == "0")
                text = "5";
            else
                text += "5";
            EDITOR_TIME = 0.0f;
        }
        else if(this->Keys[GLFW_KEY_6] && EDITOR_TIME >= 0.17)
        {
            if(text == "0")
                text = "6";
            else
                text += "6";
            EDITOR_TIME = 0.0f;
        }
        else if(this->Keys[GLFW_KEY_7] && EDITOR_TIME >= 0.17)
        {
            if(text == "0")
                text = "7";
            else
                text += "7";
            EDITOR_TIME = 0.0f;
        }
        else if(this->Keys[GLFW_KEY_8] && EDITOR_TIME >= 0.17)
        {
            if(text == "0")
                text = "8";
            else
                text += "8";
            EDITOR_TIME = 0.0f;
        }
        else if(this->Keys[GLFW_KEY_9] && EDITOR_TIME >= 0.17)
        {
            if(text == "0")
                text = "9";
            else
                text += "9";
            EDITOR_TIME = 0.0f;
        }
        else if(this->Keys[GLFW_KEY_PERIOD] && EDITOR_TIME >= 0.17)
        {
            text += ".";
            EDITOR_TIME = 0.0f;
        }
        else if(this->Keys[GLFW_KEY_BACKSPACE] && EDITOR_TIME >= 0.17)
        {
            if(text.size() > 0)
                text = text.substr(0, text.size() - 1);
            if(text.size() == 0)
                text = "0";
            EDITOR_TIME = 0.0f;
        }
        else if(this->Keys[GLFW_KEY_ENTER] && EDITOR_TIME >= 0.17)
        {
            std::string level_file = "../new_level_type/" + std::to_string(this->Level) + ".lvl";
            std::string level_2_file = "../new_level_type/" + std::to_string(this->Level) + "_blocks.lvl";

            Write_Change(*Renderer, level_file.c_str(), level_2_file.c_str(), old_x, old_y, selected_number);
            EDITOR_TIME = 0.0;
        }
        if(selected_textbox == 0 && text != "")
        {
            this->Levels[this->Level].Bricks[num_selected_block].Position.x = std::stoi(text);
            textbox_text[0] = text;
            clicked_on_block.x = std::stoi(text);
        }
        else if(selected_textbox == 1 && text != "")
        {
            this->Levels[this->Level].Bricks[num_selected_block].Position.y = std::stoi(text);
            textbox_text[1] = text;
            clicked_on_block.y = std::stoi(text);
        }
        else if(selected_textbox == 2 && text != "")
        {
            this->Levels[this->Level].Bricks[num_selected_block].Size.x = std::stoi(text);
            textbox_text[2] = text;
            clicked_on_block_size.x = std::stoi(text);
        }
        else if(selected_textbox == 3 && text != "")
        {
            this->Levels[this->Level].Bricks[num_selected_block].Size.y = std::stoi(text);
            textbox_text[3] = text;
            clicked_on_block_size.y = std::stoi(text);
        }
        else if(selected_textbox == 4 && text != "")
        {
            this->Levels[this->Level].Bricks[num_selected_block].death = std::stoi(text);
            textbox_text[4] = text;
        }
        else if(selected_textbox == 5 && text != "")
        {
            this->Levels[this->Level].Bricks[num_selected_block].boost = std::stoi(text);
            textbox_text[5] = text;
        }
        else if(selected_textbox == 6 && text != "")
        {
            this->Levels[this->Level].Bricks[num_selected_block].less_gravity = std::stoi(text);
            textbox_text[6] = text;
        }
        else if(selected_textbox == 7 && text != "")
        {
            this->Levels[this->Level].Bricks[num_selected_block].Rotation = std::stod(text);
            textbox_text[7] = text;
        }
        else if(selected_textbox == 8 && text != "")
        {
            this->Levels[this->Level].Bricks[num_selected_block].blink_show = std::stod(text);
            textbox_text[8] = text;
        }
        else if(selected_textbox == 9 && text != "")
        {
            this->Levels[this->Level].Bricks[num_selected_block].blink_hide = std::stod(text);
            textbox_text[9] = text;
        }
        else if(selected_textbox == 10 && text != "")
        {
            this->Levels[this->Level].Bricks[num_selected_block].Color.x = std::stod(text);
            textbox_text[10] = text;
        }
        else if(selected_textbox == 11 && text != "")
        {
            this->Levels[this->Level].Bricks[num_selected_block].Color.y = std::stod(text);
            textbox_text[11] = text;
        }
        else if(selected_textbox == 12 && text != "")
        {
            this->Levels[this->Level].Bricks[num_selected_block].Color.z = std::stod(text);
            textbox_text[12] = text;
        }
        else if(selected_textbox == 13 && text != "")
        {
            this->Levels[this->Level].Bricks[num_selected_block].Movement.x = std::stoi(text);
            textbox_text[13] = text;
        }
        else if(selected_textbox == 14 && text != "")
        {
            this->Levels[this->Level].Bricks[num_selected_block].Movement.y = std::stoi(text);
            textbox_text[14] = text;
        }
        else if(selected_textbox == 15 && text != "")
        {
            this->Levels[this->Level].Bricks[num_selected_block].Break = std::stod(text);
            textbox_text[15] = text;
        }
    }

    change_level_timer += dt;
    reverse_timer += dt;
    
    if(this->Keys[GLFW_KEY_M] && change_level_timer >= 0.2)
    {
        if(this->State != MAP)
            this->State = MAP;
        else
            this->State = PLACE;

        change_level_timer = 0.0;
    }

    if(this->Keys[GLFW_KEY_Z] && (this->Keys[GLFW_KEY_LEFT_CONTROL] || this->Keys[GLFW_KEY_RIGHT_CONTROL]) && reverse_timer >= 0.5) 
    {
        Reverse();
        reverse_timer = 0.0;
    }
    
    if(change_level_timer >= 10.0)
        change_level_timer = 1.0;
    if(reverse_timer >= 20.0)
        reverse_timer = 1.0;
}
double fake_time = 1.0;
void Game::mouse_pos(double mouse_x, double mouse_y, double width_ratio, double height_ratio, double dt)
{
    if(fake_time != 1.0)
        fake_time += dt;
    if(this->State == PLACE && (!((mouse_y < 40) && (mouse_x < 228)) || hide) && fake_time >= 0.1)
    {
        selected_textbox = -1;
        std::string level_file = "../new_level_type/" + std::to_string(this->Level) + ".lvl";
        std::string level_2_file = "../new_level_type/" + std::to_string(this->Level) + "_blocks.lvl";
        this->Write(level_file.c_str(), level_2_file.c_str(), selected_number, (int)mouse_x, (int)mouse_y);
        
        fake_time = 1.0;
    }
    if(this->State == DELETE && (!((mouse_y < 40) && (mouse_x < 228)) || hide))
    {
        int del_block_x = -1;
        int del_block_y = -1;
        for(GameObject &box : this->Levels[this->Level].Bricks)
        {
            if(mouse_x > box.Position.x && mouse_x < box.Position.x + box.Size.x && mouse_y > box.Position.y && mouse_y < box.Position.y + box.Size.y)
            {
                del_block_x = box.Position.x;
                del_block_y = box.Position.y;
            }
        }

        selected_textbox = -1;
        std::string level_file = "../new_level_type/" + std::to_string(this->Level) + ".lvl";
        std::string level_2_file = "../new_level_type/" + std::to_string(this->Level) + "_blocks.lvl";
        this->Delete_Block(level_file.c_str(), level_2_file.c_str(), del_block_x, del_block_y, this->Levels[this->Level].Bricks, selected_number);
        Load_Level(level_file.c_str());
    }
    if(this->State == MOVE && !((mouse_y < 40) && (mouse_x < 228)))
    {
    }
    if(mouse_x >= choose_pos.x && mouse_x <= choose_pos.x + 40 && mouse_y >= 0 && mouse_y <= 38 && !hide)
        this->State = CHOOSE;
    if(mouse_x >= delete_pos.x && mouse_x <= delete_pos.x + 40 && mouse_y >= 0 && mouse_y <= 38 && !hide)
        this->State = DELETE;
    if(mouse_x >= place_pos.x && mouse_x <= place_pos.x + 40 && mouse_y >= 0 && mouse_y <= 38 && !hide)
        this->State = PLACE;
    if(mouse_x >= move_pos.x && mouse_x <= move_pos.x + 40 && mouse_y >= 0 && mouse_y <= 38 && !hide)
    {
        clicked_on_block = glm::vec2(-1, -1);
        clicked_on_block_size = glm::vec2(-1, -1);
        this->State = MOVE;
    }
    if(mouse_x >= paint_pos.x && mouse_x <= paint_pos.x + 40 && mouse_y >= 0 && mouse_y <= 38 && !hide)
        this->State = PAINT;

    if(this->State == CHOOSE)
    {
        int squares = 0;
        int y_pos = 73;
        //Draw Squares
    next_row:
        for(int i = 75; i < this->Width - 75; i += 30)
        {
            if(mouse_x >= i && mouse_x <= i + 30 && mouse_y >= y_pos && mouse_y <= y_pos + 30)
            {
                this->State = PLACE;
                selected_block = block_type[squares];
                selected_number = squares;
                fake_time = 0.0;
            }
            squares++;
            if(squares > block_type.size())
                break;
        }
        if(squares < block_type.size())
        {
            y_pos += 30;
            goto next_row;
        }
    }

    if(this->State == MOVE)
    {
        int count = 0;
        bool clicked_textbox = false; 
        for(int i = 0; i < textbox_pos.size(); i++)
        {
            if(mouse_x >= textbox_pos[i].x && mouse_x <= textbox_pos[i].x + 90 && mouse_y >= textbox_pos[i].y && mouse_y <= textbox_pos[i].y + 18)
            {
                text = "";
                selected_textbox = i;
                clicked_textbox = true; 
                break;
            }
        }
        if(!clicked_textbox)
        {
            for(GameObject &box : this->Levels[this->Level].Bricks)
            {
                if(mouse_x > box.Position.x && mouse_x < box.Position.x + box.Size.x && mouse_y > box.Position.y && mouse_y < box.Position.y + box.Size.y)
                {
                    text = "";
                    clicked_on_block = box.Position;
                    clicked_on_block_size = box.Size;
                    num_selected_block = count;
                    old_x = box.Position.x;
                    old_y = box.Position.y;
                    Fill_Textboxes();
                }
                count++;
            }
        } 
    }
}

void Game::Fill_Textboxes()
{
    textbox_text[0] = Decimals(this->Levels[this->Level].Bricks[num_selected_block].Position.x, 2);
    textbox_text[1] = Decimals(this->Levels[this->Level].Bricks[num_selected_block].Position.y, 2);
    textbox_text[2] = Decimals(this->Levels[this->Level].Bricks[num_selected_block].Size.x, 2);
    textbox_text[3] = Decimals(this->Levels[this->Level].Bricks[num_selected_block].Size.y, 2);
    textbox_text[4] = std::to_string(this->Levels[this->Level].Bricks[num_selected_block].death);
    textbox_text[5] = Decimals(this->Levels[this->Level].Bricks[num_selected_block].boost, 2);
    textbox_text[6] = Decimals(this->Levels[this->Level].Bricks[num_selected_block].less_gravity, 2);
    textbox_text[7] = Decimals(this->Levels[this->Level].Bricks[num_selected_block].Rotation, 2);
    textbox_text[8] = Decimals(this->Levels[this->Level].Bricks[num_selected_block].blink_show, 2);
    textbox_text[9] = Decimals(this->Levels[this->Level].Bricks[num_selected_block].blink_hide, 2);
    textbox_text[10] = Decimals(this->Levels[this->Level].Bricks[num_selected_block].Color.x, 2);
    textbox_text[11] = Decimals(this->Levels[this->Level].Bricks[num_selected_block].Color.y, 2);
    textbox_text[12] = Decimals(this->Levels[this->Level].Bricks[num_selected_block].Color.z, 2);
    textbox_text[13] = Decimals(this->Levels[this->Level].Bricks[num_selected_block].Movement.x, 2);
    textbox_text[14] = Decimals(this->Levels[this->Level].Bricks[num_selected_block].Movement.y, 2);
    textbox_text[15] = Decimals(this->Levels[this->Level].Bricks[num_selected_block].Break, 2);
}

std::string Game::Decimals(double input, int deci)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(deci) << input;
    std::string temp = stream.str();
    return temp;
}
void Game::Update(double dt)
{
    
}

void Game::Reverse()
{
    std::string what_is_this = "";
    std::ifstream clipboard;

    clipboard.open("../clipboard.txt");

    std::string maybe_text = "";

    getline(clipboard, maybe_text);
    clipboard.close();

    std::ifstream block_file;
    std::ofstream temp_block_file;

    temp_block_file.open("temporary.txt");
    block_file.open("../new_level_type/" + std::to_string(this->Level) + ".lvl");

    std::string block_text = "";
    bool deleted = false;

    while(getline(block_file, block_text))
    {
        if(block_text != maybe_text)
            temp_block_file << block_text << std::endl;        
        else if(block_text == maybe_text)
        {
            std::cout << "Deleted" << std::endl;
            deleted = true;
        }
    }
    

    block_file.close();
    temp_block_file.close();

    const GLchar *file;
    file = ("../new_level_type/" + std::to_string(this->Level) + ".lvl").c_str();    
    remove(file);
    rename("temporary.txt", file);

    if(deleted)
    {
        std::ofstream fstream_2(("../new_level_type/" + std::to_string(this->Level) + "_blocks.lvl").c_str());
        fstream_2.clear();
        number_blocks[this->Level]--;
        fstream_2 << number_blocks[this->Level];
        fstream_2.close();
        std::cout << "Done deleting" << std::endl;
    }
    std::cout << "Done reversing" << std::endl;
}

void Game::Load_Levels()
{
    this->Levels.clear();
    for(int i = 1; i <= level_count; i++)
    {
        GameLevel temp;
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
        this->Levels.push_back(temp);
    }

    this->Level = 0;
}


void Game::Render()
{
    if(this->State != MAP)
    {
        Texture2D background_1;
        background_1 = ResourceManager::GetTexture("bakgrund");
        Renderer->DrawSprite(background_1, glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        //this->Draw_Level(*Renderer);
        this->Levels[this->Level].Draw(*Renderer);
        if(!hide_grid)
            this->draw_grid(*Renderer, 0, 0, 20, 20, 70, 45);
        if(!hide)
            this->draw_status_bar(*Renderer);
        if(this->State == CHOOSE)
            this->show_blocks(*Renderer);
        if(this->State == MOVE)
            this->draw_text_editor(*Renderer);        
    }
    else if(this->State == MAP)
    {
        this->draw_map(*Renderer);
        this->draw_map_cursor(*Renderer);
    }
}

void Game::show_blocks(SpriteRenderer &Renderer)
{
    Texture2D blank;
    blank = ResourceManager::GetTexture("blank");
    Renderer.DrawSprite(blank, glm::vec2(50, 50), glm::vec2(this->Width - 100, this->Height - 100), 0.0f, glm::vec3(0.1f, 0.1f, 0.1f));

    this->Load_Blocks();

    int squares = 0;
    int y_pos = 73;
    //Draw Squares
next_row:
    for(int i = 75; i < this->Width - 75; i += 30)
    {
        Renderer.DrawSprite(blank, glm::vec2(i - 2, y_pos), glm::vec2(29, 29), 0.0f, glm::vec3(0.3, 0.0, 0.0));
        squares++;

        if(squares > block_type.size() - 1)
            break;
    }
    if(squares < block_type.size() - 1)
    {
        y_pos += 30;
        goto next_row;
    }

    int place_texture_x = 75;
    int place_texture_y = 75;
    
    for(int i = 0; i < block_type.size(); i++)
    {
        if(i % 42 == 0 && i != 0)
        {
            place_texture_y += 75;
            place_texture_x = 75;
        }
        Renderer.DrawSprite(block_type[i], glm::vec2(place_texture_x, place_texture_y), glm::vec2(25, 25) ,0.0f, glm::vec3(1.0, 1.0, 1.0));
        place_texture_x += 30;
    }
}

void Game::draw_status_bar(SpriteRenderer &Renderer)
{
    Texture2D box = ResourceManager::GetTexture("blank");
    Texture2D eraser = ResourceManager::GetTexture("eraser");
    Texture2D hand = ResourceManager::GetTexture("hand");
    Texture2D pointer = ResourceManager::GetTexture("pointer");
    Texture2D paintbrush = ResourceManager::GetTexture("paintbrush");
    Texture2D curved_arrow = ResourceManager::GetTexture("curved_arrow");
    Texture2D curved_arrow_reverse = ResourceManager::GetTexture("curved_arrow_reverse");

    Renderer.DrawSprite(box, glm::vec2(1, 0), glm::vec2(320, 40), 0.0f, glm::vec3(0.0, 0.0, 0.8));
    Renderer.DrawSprite(box, glm::vec2(5, 0), glm::vec2(40, 38), 0.0f, glm::vec3(0.7, 0.7, 0.7));
    Renderer.DrawSprite(selected_block, glm::vec2(9, 2), glm::vec2(32, 32), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    if(this->State == DELETE)
        Renderer.DrawSprite(box, glm::vec2(50, 0), glm::vec2(40, 38), 0.0f, glm::vec3(0.0, 0.7, 0.4));
    else
        Renderer.DrawSprite(box, glm::vec2(50, 0), glm::vec2(40, 38), 0.0f, glm::vec3(0.7, 0.7, 0.7));
    Renderer.DrawSprite(eraser, glm::vec2(60, 2), glm::vec2(20, 32), 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
    if(this->State == PLACE)
        Renderer.DrawSprite(box, glm::vec2(95, 0), glm::vec2(40, 38), 0.0f, glm::vec3(0.0, 0.7, 0.4));
    else
        Renderer.DrawSprite(box, glm::vec2(95, 0), glm::vec2(40, 38), 0.0f, glm::vec3(0.7, 0.7, 0.7));
    Renderer.DrawSprite(pointer, glm::vec2(105, 2), glm::vec2(20, 32), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    if(this->State == MOVE)
        Renderer.DrawSprite(box, glm::vec2(140, 0), glm::vec2(40, 38), 0.0f, glm::vec3(0.0, 0.7, 0.4));
    else
        Renderer.DrawSprite(box, glm::vec2(140, 0), glm::vec2(40, 38), 0.0f, glm::vec3(0.7, 0.7, 0.7));
    Renderer.DrawSprite(hand, glm::vec2(144, 2), glm::vec2(32, 32), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    if(this->State == PAINT)
        Renderer.DrawSprite(box, glm::vec2(185, 0), glm::vec2(40, 38), 0.0f, glm::vec3(0.0, 0.7, 0.4));
    else
        Renderer.DrawSprite(box, glm::vec2(185, 0), glm::vec2(40, 38), 0.0f, glm::vec3(0.7, 0.7, 0.7));

    Renderer.DrawSprite(paintbrush, glm::vec2(189, 2), glm::vec2(32, 32), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

    Renderer.DrawSprite(box, glm::vec2(230, 0), glm::vec2(40, 38), 0.0f, glm::vec3(0.7, 0.7, 0.7));
    Renderer.DrawSprite(curved_arrow, glm::vec2(234, 3), glm::vec2(32, 32), 0.0f, glm::vec3(1.0, 1.0, 1.0));

    Renderer.DrawSprite(box, glm::vec2(275, 0), glm::vec2(40, 38), 0.0f, glm::vec3(0.7, 0.7, 0.7));
    Renderer.DrawSprite(curved_arrow_reverse, glm::vec2(279, 3), glm::vec2(32, 32), 0.0f, glm::vec3(1.0, 1.0, 1.0));
}

void Game::Load_Textures()
{
    ResourceManager::LoadTexture("../textures/blank.png", GL_TRUE, "blank");
    ResourceManager::LoadTexture("../textures/character.png", GL_TRUE, "player");
    ResourceManager::LoadTexture("../textures/Arcade.png", GL_TRUE, "arcade");
    ResourceManager::LoadTexture("../textures/stone.png", GL_TRUE, "stone");
    ResourceManager::LoadTexture("../textures/Bakgrund.png", GL_TRUE, "bakgrund");
    ResourceManager::LoadTexture("../textures/monster.png", GL_TRUE, "monster");
    ResourceManager::LoadTexture("../textures/eraser.png", GL_TRUE, "eraser");
    ResourceManager::LoadTexture("../textures/arrow.png", GL_TRUE, "arrow");
    ResourceManager::LoadTexture("../textures/hand.png", GL_TRUE, "hand");
    ResourceManager::LoadTexture("../textures/cursor.png", GL_TRUE, "pointer");
    ResourceManager::LoadTexture("../textures/paintbrush.png", GL_TRUE, "paintbrush");
    ResourceManager::LoadTexture("../textures/dirt_block.png", GL_TRUE, "grass_block");
    ResourceManager::LoadTexture("../textures/only_dirt.png", GL_TRUE, "dirt_block");
    ResourceManager::LoadTexture("../textures/spikes.png", GL_TRUE, "spikes");
    ResourceManager::LoadTexture("../textures/only_dirt.png", GL_TRUE, "only_dirt");    
    ResourceManager::LoadTexture("../textures/metal_block.png", GL_TRUE, "metal_block");    
    ResourceManager::LoadTexture("../textures/fleip_grass.png", GL_TRUE, "fleip_grass");    
    ResourceManager::LoadTexture("../textures/curved_arrow.png", GL_TRUE, "curved_arrow");    
    ResourceManager::LoadTexture("../textures/curved_arrow_reverse.png", GL_TRUE, "curved_arrow_reverse");    
    ResourceManager::LoadTexture("../textures/saws.png", GL_TRUE, "saws");    
    std::cout << "Loaded all textures" << std::endl;
}

void Game::draw_grid(SpriteRenderer &Renderer, int start_x, int start_y, int square_width, int square_height, int num_x, int num_y)
{
    Texture2D grid;
    grid = ResourceManager::GetTexture("blank");
    for(int i = 0; i <= square_width * num_x; i += square_width)
        Renderer.DrawSprite(grid, glm::vec2(i, 0), glm::vec2(1, square_width * num_y), 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));
    for(int i = 0; i <= square_height * num_y; i += square_height)
        Renderer.DrawSprite(grid, glm::vec2(0, i), glm::vec2(square_height * num_x, 1), 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));
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

void Game::Write_Change(SpriteRenderer &renderer, const GLchar *file, const GLchar *file_2, int x_pos, int y_pos, int n)
{
    std::string line;
    std::ofstream fstream_temp("temp.txt");
    std::ifstream fstream(file);
    fstream.clear();
    fstream_temp.clear();
    
    while(getline(fstream, line))
    {
        std::string change_line = std::to_string(line[0] - 48) + " " + std::to_string(x_pos) + " " + std::to_string(y_pos);
        std::string temp_line = "";

        for(int i = 0; i < change_line.size(); i++)
        {
            temp_line += line[i];
        }

        if(temp_line != change_line)
            fstream_temp << line << std::endl;
        if(temp_line == change_line && line[temp_line.size() == ' '])
        {
            fstream_temp << n << " "
                         << this->Levels[this->Level].Bricks[num_selected_block].Position.x << " "
                         << this->Levels[this->Level].Bricks[num_selected_block].Position.y << " "
                         << this->Levels[this->Level].Bricks[num_selected_block].Size.x << " "
                         << this->Levels[this->Level].Bricks[num_selected_block].Size.y << " "
                         << this->Levels[this->Level].Bricks[num_selected_block].death << " "
                         << this->Levels[this->Level].Bricks[num_selected_block].boost << " "
                         << this->Levels[this->Level].Bricks[num_selected_block].less_gravity << " "
                         << Decimals(this->Levels[this->Level].Bricks[num_selected_block].Rotation, 1) << " "
                         << Decimals(this->Levels[this->Level].Bricks[num_selected_block].blink_show, 2) << " "
                         << Decimals(this->Levels[this->Level].Bricks[num_selected_block].blink_hide, 2) << " "
                         << Decimals(this->Levels[this->Level].Bricks[num_selected_block].Color.x, 1) << " "
                         << Decimals(this->Levels[this->Level].Bricks[num_selected_block].Color.y, 1) << " "
                         << Decimals(this->Levels[this->Level].Bricks[num_selected_block].Color.z, 1) << " "
                         << this->Levels[this->Level].Bricks[num_selected_block].Movement.x << " "
                         << this->Levels[this->Level].Bricks[num_selected_block].Movement.y << " "
                         << Decimals(this->Levels[this->Level].Bricks[num_selected_block].Break, 1) << "\n";
        }
    }
    fstream_temp.close();
    fstream.close();
    remove(file);
    rename("temp.txt", file);
}

void Game::draw_text_editor(SpriteRenderer &renderer)
{
    Texture2D blank;
    blank = ResourceManager::GetTexture("blank");
    if(clicked_on_block.x != -1 && clicked_on_block.y != -1)
    {
        if(clicked_on_block.x < this->Width / 2)
        {
            Renderer->DrawSprite(blank, glm::vec2((this->Width / 2) + 50, 50), glm::vec2((this->Width / 2) - 100, this->Height - 735), 0.0f, glm::vec3(0.4, 0.4, 0.4));
            print_text_left = false;
            Print_Text();
        }
        if(clicked_on_block.x >= this->Width / 2)
        {
            Renderer->DrawSprite(blank, glm::vec2(50, 50), glm::vec2((this->Width / 2) - 100, this->Height - 735), 0.0f, glm::vec3(0.4, 0.4, 0.4));
            print_text_left = true;
            Print_Text();
        }
        Texture2D line;
        line = ResourceManager::GetTexture("blank");
        Renderer->DrawSprite(line, clicked_on_block, glm::vec2(clicked_on_block_size.x, 2), 0.0f, glm::vec3(1.0, 0.0, 0.0));
        Renderer->DrawSprite(line, clicked_on_block, glm::vec2(2, clicked_on_block_size.y), 0.0f, glm::vec3(1.0, 0.0, 0.0));
        Renderer->DrawSprite(line, glm::vec2(clicked_on_block.x + clicked_on_block_size.x - 2, clicked_on_block.y), glm::vec2(2, clicked_on_block_size.y), 0.0f, glm::vec3(1.0, 0.0, 0.0));
        Renderer->DrawSprite(line, glm::vec2(clicked_on_block.x, clicked_on_block.y + clicked_on_block_size.y - 2), glm::vec2(clicked_on_block_size.x, 2), 0.0f, glm::vec3(1.0, 0.0, 0.0));
    }
}
void Game::Text_Label(std::string temp_string, int x_pos, int y_pos, glm::vec3 color)
{
    Label_Text->RenderText(temp_string, x_pos, y_pos, 1.0f, color);
}

//Add something if you try to put something where there already is a block it should edit it
void Game::Write(const GLchar *file, const GLchar *file_2, int n, int x_pos, int y_pos)
{
    std::ifstream stream_2(file);
    bool dont_add = false;
    for(int i = 0; i < number_blocks[this->Level]; i++)
    {
        double code;
        int in_block;
        int block;
        bool same_x;
        bool same_y;
        std::string line;
        if(stream_2)
        {
            while (std::getline(stream_2, line)) // Read each line from level file
            {
                std::istringstream sstream(line);
                while (sstream >> code)
                {
                    if(in_block == 1)
                    {
                        if(int(x_pos / 20) * 20 == code)
                            same_x = true;
                    }
                    if(in_block == 2 && same_x)
                    {
                        if(code == int(y_pos / 20) * 20)
                            dont_add = true;
                    }
                    in_block++;
                }
                same_x = false;
                in_block = 0;
                block++;
            }
        }
    }
    stream_2.close();
    if(!dont_add)
    {
        std::ofstream fstream(file, std::ios::app);
        std::ofstream clipboard("../clipboard.txt");
        fstream.clear();
        clipboard.clear();
        if(n == 0)
        {
            fstream << n << " " << int(x_pos / 20) * 20 << " " << int(y_pos / 20) * 20 << " 20 20 0 1 0 0.0 0.0 0.0 1.0 1.0 1.0 0 0 0 0 \n";
            clipboard << n << " " << int(x_pos / 20) * 20 << " " << int(y_pos / 20) * 20 << " 20 20 0 1 0 0.0 0.0 0.0 1.0 1.0 1.0 0 0 0 0 \n";            
        }
        else if(n == 1)
        {
            fstream << n << " " << int(x_pos / 20) * 20 << " " << int(y_pos / 20) * 20 << " 50 100 0 0 0 0.0 0.0 0.0 1.0 1.0 1.0 0 0 0 0 \n";
            clipboard << n << " " << int(x_pos / 20) * 20 << " " << int(y_pos / 20) * 20 << " 20 20 0 1 0 0.0 0.0 0.0 1.0 1.0 1.0 0 0 0 0 \n";                        
        }
        else
        {
            fstream << n << " " << int(x_pos / 20) * 20 << " " << int(y_pos / 20) * 20 << " 20 20 0 1 0 0.0 0.0 0.0 1.0 1.0 1.0 0 0 0 0 \n";                        
            clipboard << n << " " << int(x_pos / 20) * 20 << " " << int(y_pos / 20) * 20 << " 20 20 0 0 0 0.0 0.0 0.0 1.0 1.0 1.0 0 0 0 0 \n";                        
        }
        
        fstream.close();
        clipboard.close();
        
        std::ofstream fstream_2(file_2);
        fstream_2.clear();
        number_blocks[this->Level]++;
        fstream_2 << number_blocks[this->Level];
        fstream_2.close();
        Load_Level(file);        
    }
}
void Game::Delete_Block(const GLchar *file, const GLchar *file_2, int x_pos, int y_pos, std::vector<GameObject> temp_bricks, int n)
{
    bool deleted = false;
    std::string line;
    std::ofstream fstream_temp("temp.txt");
    std::ifstream fstream(file);
    fstream.clear();
    fstream_temp.clear();


    while(getline(fstream, line))
    {
        std::string del_line = std::to_string(line[0] - 48) + " " + std::to_string(x_pos) + " " + std::to_string(y_pos);
        std::string temp_line = "";
        for(int i = 0; i < del_line.size(); i++)
        {
            temp_line += line[i];
        }
        
        if(temp_line == del_line && line[temp_line.size()] == ' ')
            deleted = true;
        else
            fstream_temp << line << std::endl;
    }
    fstream_temp.close();
    fstream.close();
    remove(file);
    rename("temp.txt", file);


    if(deleted)
    {
        std::ofstream fstream_2(file_2);
        fstream_2.clear();
        number_blocks[this->Level]--;
        fstream_2 << number_blocks[this->Level];
        fstream_2.close();
    }
}
void Game::Load_Blocks()
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
    Texture2D dirt;
    dirt = ResourceManager::GetTexture("only_dirt");
    block_type.push_back(dirt);    
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
            std::istringstream sstream(line);
            while (sstream >> code)
            {
                
                if(in_block == 0)
                {
                    temp_obj.Sprite = block_type[code];
                    if(code == 1)
                        arcade = true;
                }
                else if(in_block == 1)
                    temp_obj.Position.x = code;
                else if(in_block == 2)
                    temp_obj.Position.y = code;
                else if(in_block == 3)
                    temp_obj.Size.x = code;
                else if(in_block == 4)
                    temp_obj.Size.y = code;
                else if(in_block == 5) 
                    temp_obj.death = code;
                else if(in_block == 6)
                    temp_obj.boost = code;
                else if(in_block == 7)
                    temp_obj.less_gravity = code;
                else if(in_block == 8)
                    temp_obj.Rotation = code;
                else if(in_block == 9)
                    temp_obj.blink_show = (double)code;
                else if(in_block == 10)
                    temp_obj.blink_hide = (double)code;
                else if(in_block == 11)
                    temp_obj.Color.x = (double)code;
                else if(in_block == 12)
                    temp_obj.Color.y = (double)code;
                else if(in_block == 13)
                    temp_obj.Color.z = (double)code;
                else if(in_block == 14)
                    temp_obj.Movement.x = code;
                else if(in_block == 15)
                    temp_obj.Movement.y = code;
                else if(in_block == 16)
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
 
void Game::Draw_Level(SpriteRenderer &renderer)
{
    for (GameObject &tile : temp_lvl)
    {
        if (!tile.destroyed)
            tile.Draw(renderer);
    }
}

void Game::Print_Text()
{
    Texture2D square;
    square = ResourceManager::GetTexture("blank");

    block_functions_text.clear();
    block_functions_text.push_back("X Position:");
    block_functions_text.push_back("Y Position:");
    block_functions_text.push_back("X Size:");
    block_functions_text.push_back("Y Size:");
    block_functions_text.push_back("Death:");
    block_functions_text.push_back("Speed:");
    block_functions_text.push_back("Spin:");
    block_functions_text.push_back("Rotation:");
    block_functions_text.push_back("Blink_Show:");
    block_functions_text.push_back("Blink_Hide:");
    block_functions_text.push_back("R Color:");
    block_functions_text.push_back("G Color:");
    block_functions_text.push_back("B Color:");
    block_functions_text.push_back("X Movement:");
    block_functions_text.push_back("Y Movement:");
    block_functions_text.push_back("Break:");
    block_functions_text.push_back("Speed:");
    block_functions_text.push_back("Break:");

    int num = 0;
    bool done = false;
    textbox_pos.clear();
    int text_square = 0;
    if(!print_text_left)
    {
        for(int i = 75; i <= 815; i += 20)
        {
            if(i < 195)
            {
                Text_Label(block_functions_text[num], this->Width / 2 + 75 , i, glm::vec3(1.0, 1.0, 1.0));
                Text_Label(block_functions_text[num + 1], this->Width / 2 + 265, i, glm::vec3(1.0, 1.0, 1.0));
                Text_Label(block_functions_text[num + 2], this->Width / 2 + 455, i, glm::vec3(1.0, 1.0, 1.0));
            }
            else
            {
                Text_Label("", this->Width / 2 + 75 , i, glm::vec3(1.0, 1.0, 1.0));
                Text_Label("", this->Width / 2 + 265, i, glm::vec3(1.0, 1.0, 1.0));
                Text_Label("", this->Width / 2 + 455, i, glm::vec3(1.0, 1.0, 1.0));
            }
            if(i < 195)
            {
                if(selected_textbox == num)
                    Renderer->DrawSprite(square, glm::vec2(this->Width / 2 + 170, i - 2), glm::vec2(90, 18), 0.0f, glm::vec3(1.0, 0.0, 0.0));
                else
                    Renderer->DrawSprite(square, glm::vec2(this->Width / 2 + 170, i - 2), glm::vec2(90, 18), 0.0f, glm::vec3(1.0, 1.0, 1.0));
                if(selected_textbox == num + 1)
                    Renderer->DrawSprite(square, glm::vec2(this->Width / 2 + 360, i - 2), glm::vec2(90, 18), 0.0f, glm::vec3(1.0, 0.0, 0.0));
                else
                    Renderer->DrawSprite(square, glm::vec2(this->Width / 2 + 360, i - 2), glm::vec2(90, 18), 0.0f, glm::vec3(1.0, 1.0, 1.0));
                if(selected_textbox == num + 2)                    
                    Renderer->DrawSprite(square, glm::vec2(this->Width / 2 + 550, i - 2), glm::vec2(90, 18), 0.0f, glm::vec3(1.0, 0.0, 0.0));
                else
                    Renderer->DrawSprite(square, glm::vec2(this->Width / 2 + 550, i - 2), glm::vec2(90, 18), 0.0f, glm::vec3(1.0, 1.0, 1.0));                

                Text_Label(textbox_text[num], this->Width / 2 + 172, i + 1, glm::vec3(0.0, 0.0, 0.0));
                Text_Label(textbox_text[num + 1], this->Width / 2 + 362, i + 1, glm::vec3(0.0, 0.0, 0.0));
                Text_Label(textbox_text[num + 2], this->Width / 2 + 552, i + 1, glm::vec3(0.0, 0.0, 0.0));

                textbox_pos.push_back(glm::vec2(this->Width / 2 + 170, i - 2));
                textbox_pos.push_back(glm::vec2(this->Width / 2 + 360, i - 2));
                textbox_pos.push_back(glm::vec2(this->Width / 2 + 550, i - 2));
            }
            if(num <= block_functions_text.size() - 6)
                num += 3;
        }
    }
    if(print_text_left)
    {
        for(int i = 75; i <= 815; i += 20)
        {
            if(i < 195)
            {
                Text_Label(block_functions_text[num],75 , i, glm::vec3(1.0, 1.0, 1.0));
                Text_Label(block_functions_text[num + 1], 265, i, glm::vec3(1.0, 1.0, 1.0));
                Text_Label(block_functions_text[num + 2], 455, i, glm::vec3(1.0, 1.0, 1.0));
            }
            else
            {
                Text_Label("", 75 , i, glm::vec3(1.0, 1.0, 1.0));
                Text_Label("", 265, i, glm::vec3(1.0, 1.0, 1.0));
                Text_Label("", 455, i, glm::vec3(1.0, 1.0, 1.0));
            }
            if(i < 195)
            {
                if(selected_textbox == num)
                    Renderer->DrawSprite(square, glm::vec2( 170, i - 2), glm::vec2(90, 18), 0.0f, glm::vec3(1.0, 0.0, 0.0));
                else
                    Renderer->DrawSprite(square, glm::vec2( 170, i - 2), glm::vec2(90, 18), 0.0f, glm::vec3(1.0, 1.0, 1.0));
                if(selected_textbox == num + 1)
                    Renderer->DrawSprite(square, glm::vec2( 360, i - 2), glm::vec2(90, 18), 0.0f, glm::vec3(1.0, 0.0, 0.0));
                else
                    Renderer->DrawSprite(square, glm::vec2( 360, i - 2), glm::vec2(90, 18), 0.0f, glm::vec3(1.0, 1.0, 1.0));
                if(selected_textbox == num + 2)                    
                    Renderer->DrawSprite(square, glm::vec2( 550, i - 2), glm::vec2(90, 18), 0.0f, glm::vec3(1.0, 0.0, 0.0));
                else
                    Renderer->DrawSprite(square, glm::vec2( 550, i - 2), glm::vec2(90, 18), 0.0f, glm::vec3(1.0, 1.0, 1.0));                    

                Text_Label(textbox_text[num],  172, i + 1, glm::vec3(0.0, 0.0, 0.0));
                Text_Label(textbox_text[num + 1],  362, i + 1, glm::vec3(0.0, 0.0, 0.0));
                Text_Label(textbox_text[num + 2],  552, i + 1, glm::vec3(0.0, 0.0, 0.0));

                textbox_pos.push_back(glm::vec2( 170, i - 2));
                textbox_pos.push_back(glm::vec2( 360, i - 2));
                textbox_pos.push_back(glm::vec2( 550, i - 2));
            }
            if(num <= block_functions_text.size() - 6)
                num += 3;
        }

    }

}
