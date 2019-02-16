#include <iostream>

#include <fstream>
#include <sstream>
#include "game_level.h"

extern int level_count;
extern glm::vec2 spawnpoint;
void GameLevel::Load(const GLchar *file, GLuint levelWidth, GLuint levelHeight)
{
    // Clear old data
    this->Bricks.clear();
    // Load from file
    GLuint tileCode;
    GameLevel level;
    std::string line;
    std::ifstream fstream(file);
    std::vector<std::vector<GLuint>> level_color;
    if (fstream)
    {
        while (std::getline(fstream, line)) // Read each line from level file
        {
            std::istringstream sstream(line);
            std::vector<GLuint> row;
            while (sstream >> tileCode)
            { // Read each word seperated by spaces
                row.push_back(tileCode);
            }
            tileData.push_back(row);
        }
        if (tileData.size() > 0)
        {
            this->init(tileData, levelWidth, levelHeight);
        }
    }

}
 
void GameLevel::Draw(SpriteRenderer &renderer)
{
    for (GameObject &tile : this->Bricks)
        if (!tile.destroyed)
            tile.Draw(renderer);
}

void GameLevel::Clear(const GLchar *file, int level)
{
    std::ofstream fstream(file);
    for(int y = 0; y < 45; y++)
    {
        for(int x = 0; x < 70; x++)
        {
            fstream << "0 ";
        }
        fstream << "\n";
    }

}
void GameLevel::new_level(int levels)
{
    std::string a = "levels/" + std::to_string(levels) + ".lvl";
    std::ofstream a_file(a.c_str());

    std::ofstream fstream(a.c_str());
    for(int y = 0; y < 45; y++)
    {
        for(int x = 0; x < 70; x++)
        {
            fstream << "0 ";
        }
        fstream << "\n";
    }
}

void GameLevel::init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight)
{
    this->Bricks.clear();
    // Calculate dimensions
    GLuint height = tileData.size();
    GLuint width = tileData[0].size(); // Note we can index vector at [0] since this function is only called if height > 0

    //Base size
    GLfloat unit_width = 20;
    GLfloat unit_height = 20;
    // Initialize level tiles based on tileData

    for (int y = 0; y < 45; ++y)
    {
        for (int x = 0; x < 70; ++x)
        {
            glm::vec3 othercolor(1.0f, 0.0f, 0.0f);
            glm::vec2 pos(unit_width * x, unit_height * y);
            glm::vec2 size(unit_width, unit_height);

            switch(tileData[y][x])
            { 
            case 1:
            {
                glm::vec3 color(1.0f, 1.0f, 1.0f);
                GameObject obj(pos, size, ResourceManager::GetTexture("stone"), color);
                obj.death = false;
                this->Bricks.push_back(obj);
                break;
            }
            case 2:
            {
                glm::vec3 color(0.0f, 1.0f, 0.0f);
                GameObject obj(pos, size, ResourceManager::GetTexture("blank"), color);
                obj.death = false;
                obj.Color.x = 1.0f;
                this->Bricks.push_back(obj);
                break;
            }
            case 3:
            {
                glm::vec3 color(1.0f, 255.0 / 127.0, 255.0 / 80.0);
                GameObject obj(pos, size, ResourceManager::GetTexture("blank"), color);
                obj.death = true;
                this->Bricks.push_back(obj);
                break;
            }
            }
        }
    }
}
