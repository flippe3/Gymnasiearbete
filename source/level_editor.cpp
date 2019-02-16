#include "level_editor.h"

Level_Editor::Level_Editor(GLuint Width, GLuint Height)
{
}

Level_Editor::~Level_Editor()
{
}

void Level_Editor::Render(SpriteRenderer *Renderer)
{
    Texture2D background_1;
    background_1 = ResourceManager::GetTexture("bakgrund");
    Renderer->DrawSprite(background_1, glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
}
void Level_Editor::Input(double dt)
{
}
