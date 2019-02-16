#ifndef PHYSICS_H
#define PHYSICS_H

#include <iostream>
#include <algorithm>

#include "game_object.h"

class physics
{
public:
    bool bottom_collision(GameObject &one, GameObject &two);
    bool above_collision(GameObject &one, GameObject &two);
    bool right_collision(GameObject &one, GameObject &two);
    bool left_collision(GameObject &one, GameObject &two);

    bool disable_jump = false;
    bool dont_auto_jump = false;
    GLfloat ground = 550.0f;
    bool wall_jump = false;
    GLfloat gravity;
    GLfloat vel;
    bool is_grounded;
    GLfloat velocity;
    GLfloat jump_gravity;
    bool actually_jump;
    GLfloat fake_gravity;
    GLfloat fake_vel;

    bool bottom = false, top = false, left = false, right = false;
    
    physics();
    ~physics();
    void Init();
    void Detect_collision();
    GLfloat Direction_collision();
    void Jump(GameObject &one, GLfloat dt);
    void Main_Char_Init(GameObject &one, GLfloat peak_height, GLfloat time_to_peak_height);
    bool check_floating(GameObject &one, GameObject &two);
    void floating(GameObject &one, GLfloat dt);
    void falseify();
    void narrow_phase_collision();
    void broad_phase_collision();
    void collision(GameObject &one, GameObject &two);
    bool rectangle_circle_collision(GameObject &one, GameObject &two);
    bool circle_collision(GameObject &one, GameObject &two);    
};
#endif
