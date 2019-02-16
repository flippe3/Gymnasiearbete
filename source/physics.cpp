#include <algorithm>
#include <iostream>

#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include "text_renderer.h"
#include "physics.h"

physics::physics()
{
    //Constructor install a bunch of stuff
}

physics::~physics()
{
    //Delete all the stuff
}

void physics::Init()
{
    //Set all the constants here
    gravity = -1400.0f;
    vel = 700.0f;
    is_grounded = false;
    actually_jump = true;
}

bool physics::rectangle_circle_collision(GameObject &rectangle, GameObject &circle)
{
    float circle_x = circle.Position.x + (circle.Size.x / 2);
    float circle_y = circle.Position.y + (circle.Size.y / 2);
    // Find the closest point to the circle within the rectangle
    float closest_x = glm::clamp(circle_x, rectangle.Position.x, rectangle.Position.x + rectangle.Size.x);
    float closest_y = glm::clamp(circle_y, rectangle.Position.y, rectangle.Position.y + rectangle.Size.y);

    // Calculate the distance between the circle's center and this closest point
    float distance_x = circle_x - closest_x;
    float distance_y = circle_y - closest_y;

    // If the distance is less than the circle's radius, an intersection occurs
    float distance_squared = (distance_x * distance_x) + (distance_y * distance_y);
    if(distance_squared < (pow(circle.Size.x /2, 2)))
    {
        //std::cout << "RECTANGLE COLLIDE WITH CIRCLE" << std::endl;
    }
        
    return distance_squared < (pow(circle.Size.x /2, 2));
}
bool physics::circle_collision(GameObject &one, GameObject &two)
{
    GLfloat center_1x = one.Position.x + (one.Size.x / 2);
    GLfloat center_1y = one.Position.y + (two.Size.y / 2);
    GLfloat center_2x = two.Position.x + (two.Size.x / 2);
    GLfloat center_2y = two.Position.y + (two.Size.y / 2);

    GLfloat radius_1 = one.Size.x / 2;
    GLfloat radius_2 = two.Size.x / 2;

    if(pow(center_2x - center_1x, 2) + pow(center_1y - center_2y, 2) <= pow(radius_1 + radius_2, 2))
    {
        //std::cout << "Circle collision" << std::endl;
    }
    return pow(center_2x - center_1x, 2) + pow(center_1y - center_2y, 2) <= pow(radius_1 + radius_2, 2);
}

void physics::collision(GameObject &one, GameObject &two)
{
    double dx = (one.Position.x + (one.Size.x / 2))-(two.Position.x + (two.Size.x / 2));
    double dy = (one.Position.y + (one.Size.y / 2))-(two.Position.y + (two.Size.y / 2));
    double width = (one.Size.x + two.Size.x) / 2;
    double height = (one.Size.y + two.Size.y) / 2;
    double crossWidth = width * dy;
    double crossHeight = height * dx;

    top = false, left = false, right = false, bottom = false;

    if(std::abs(dx) <= width && std::abs(dy) <= height)
    {
        if(crossWidth > crossHeight)
        {
            if(crossWidth > (-crossHeight))
                top = true;
            else
                right = true;
        }
        else
        {
            if(crossWidth > (-crossHeight))
                left = true;
            else
                bottom = true;
        }
    }
}

bool physics::bottom_collision(GameObject &one, GameObject &two)
{
    if(one.Position.x <= two.Position.x + two.Size.x && one.Position.x + one.Size.x >= two.Position.x
       && one.Position.y <= two.Position.y + two.Size.y && one.Position.y + one.Size.y >= two.Position.y)
    {
        collision(one, two);
        if(bottom == true)
            return true;
    }
    return false;
}

bool physics::above_collision(GameObject &one, GameObject &two)
{
    if(one.Position.x <= two.Position.x + two.Size.x && one.Position.x + one.Size.x >= two.Position.x
       && one.Position.y <= two.Position.y + two.Size.y && one.Position.y + one.Size.y >= two.Position.y)
    {
        collision(one, two);
        if(top == true)
        {
            return true;
        }
    }
    return false;
}

bool physics::right_collision(GameObject &one, GameObject &two)
{
    if(one.Position.x <= two.Position.x + two.Size.x && one.Position.x + one.Size.x >= two.Position.x
       && one.Position.y <= two.Position.y + two.Size.y && one.Position.y + one.Size.y >= two.Position.y)
    {
        collision(one, two);
        if(right)
            return true;
    }
    return false;
}

bool physics::left_collision(GameObject &one, GameObject &two)
{
    if(one.Position.x <= two.Position.x + two.Size.x && one.Position.x + one.Size.x >= two.Position.x
       && one.Position.y <= two.Position.y + two.Size.y && one.Position.y + one.Size.y >= two.Position.y)
    {
        collision(one, two);
        if(left)
            return true;
    }
    return false;
}

void physics::Main_Char_Init(GameObject &one, GLfloat peak_height, GLfloat time_to_peak_height)
{
    one.Velocity.y = (peak_height * 2 * time_to_peak_height) / time_to_peak_height;
    jump_gravity = (-2 * peak_height) / time_to_peak_height;
}


void physics::Jump(GameObject &one, GLfloat dt)
{
    if(is_grounded || wall_jump)
    {
        actually_jump = true;
    }
    if(actually_jump)
    {        
        one.Position.y -= (one.Velocity.y * dt) + (0.5f * jump_gravity * dt * dt);
        one.Velocity.y += (jump_gravity * dt);

        if(disable_jump)
        {
            one.Velocity.y = 700.0f;
            falseify();
        }
    }
    //@Fix: Check if the player is floating and do a fix for that.
}

void physics::floating(GameObject &one, GLfloat dt)
{
    one.Position.y += (-fake_vel * dt) + (0.5f * fake_gravity * dt * dt);
    fake_vel += (fake_gravity * dt) * 1.2;
}

void physics::falseify()
{
    disable_jump = false;
    actually_jump = false;
}
