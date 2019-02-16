#include <iostream>

#include <fstream>
#include <sstream>
#include "game_level.h"

extern int level_count;
extern glm::vec2 spawnpoint;
extern std::vector<glm::vec2> sokoban_spawn_point;
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
                if (!tile.destroyed && !tile.boost)
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

void GameLevel::Sokoban_Load(const GLchar *file, GLuint levelWidth, GLuint levelHeight, int actual_level)
{
        // Clear old data
        this->Sokoban_Bricks.clear();
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
                        Sokoban_tileData.push_back(row);
                }
                if (Sokoban_tileData.size() > 0)
                {
                        this->sokoban_init(Sokoban_tileData, levelWidth, levelHeight, actual_level);
                }
        }
}

void GameLevel::sokoban_init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight, int level)
{
        this->Sokoban_Bricks.clear();
        // Calculate dimensions
        GLuint height = Sokoban_tileData.size();
        GLuint width = Sokoban_tileData[0].size(); // Note we can index vector at [0] since this function is only called if height > 0

        //Base size
        GLfloat unit_width = 50;
        GLfloat unit_height = 50;
        // Initialize level tiles based on tileData

        for (int y = 0; y < 20; ++y)
        {
                for (int x = 0; x < 20; ++x)
                {
                        glm::vec3 color(1.0f, 1.0f, 1.0f);
                        glm::vec3 othercolor(1.0f, 0.0f, 0.0f);
                        glm::vec2 pos(unit_width * x, unit_height * y);
                        glm::vec2 size(unit_width, unit_height);

                        switch(tileData[y][x])
                        {
                        case 1:
                        {
                                GameObject obj(pos, size, ResourceManager::GetTexture("wall"), color);
                                obj.death = true;
                                obj.move = false;
                                this->Sokoban_Bricks.push_back(obj);
                                break;
                        }
                        case 2:
                        {
                                GameObject obj(pos, size, ResourceManager::GetTexture("middle"), color);
                                obj.death = false;
                                obj.move = false;
                                this->Sokoban_Bricks.push_back(obj);
                                break;
                        }
                        case 3:
                        {
                                GameObject obj(pos, size, ResourceManager::GetTexture("box"), color);
                                obj.move = true;
                                obj.death = false;
                                this->Sokoban_Bricks.push_back(obj);
                                break;
                        }
                        case 4:
                        {
                                GameObject obj(pos, size, ResourceManager::GetTexture("box"), color);
                                sokoban_spawn_point.push_back(obj.Position);
                        }
                        }
                }
        }
}

void GameLevel::Sokoban_Draw(SpriteRenderer &renderer)
{
        for(GameObject &tile : this->Sokoban_Bricks)
        {
                if(!tile.destroyed && tile.death && !tile.move)
                        tile.Draw(renderer);
                if(!tile.destroyed && tile.move && !tile.death)
                        tile.Draw(renderer);
        }

        for (GameObject &tile : this->Sokoban_Bricks)
        {
                if(!tile.move && !tile.death)
                        if(!tile.destroyed)
                                tile.Draw(renderer);
        }
}
