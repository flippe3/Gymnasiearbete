#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
//#include <GL/glxew.h>
//#include <GL/glx.h>
#include <iostream>
#include "game.h"

// GLFW function declerations
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void window_resize_handler(GLFWwindow* window, int window_width, int window_height);

GLuint SCREEN_WIDTH = 1400;
GLuint SCREEN_HEIGHT = 900;

double width_ratio = 1.0f;
double height_ratio = 1.0f;

const GLuint default_screen_width = 1400;
const GLuint default_screen_height = 900;
Game level_editor(SCREEN_WIDTH, SCREEN_HEIGHT);
int main(int argc, char *argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Gymnasiearbete - Level Editor", nullptr, nullptr);

    glfwMakeContextCurrent(window);
    //glewExperimental = GL_TRUE;
    glewInit();
    glGetError(); // Call it once to catch glewInit() bug, all other errors are now from our application.

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, window_resize_handler);

    //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // OpenGL configuration
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Initialize game
    level_editor.Init();
    std::cout << "Init done" << std::endl;
    // DeltaTime variables
    double deltaTime = 0.0f;
    double lastFrame = 0.0f;
    // Start Game in GAME_ACTIVE STATE
    //Project.State = GAME_ACTIVE;

    double mouse_x = 0, mouse_y = 0;
    while (!glfwWindowShouldClose(window))
    {
        // Calculate delta time
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();
        glfwGetCursorPos(window, &mouse_x, &mouse_y);

        //deltaTime = 0.001f;
        // Manage user input
        level_editor.ProcessInput(deltaTime);

        int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if(state == GLFW_PRESS)
            level_editor.mouse_pos(mouse_x, mouse_y, width_ratio, height_ratio, deltaTime);

        level_editor.Update(deltaTime);
        // Render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        level_editor.Render();

        glfwSwapBuffers(window);

        //0 is for vsync off, 1 is for vsync on
        glfwSwapInterval(0);

    }
    // Delete all resources as loaded using the resource manager
    ResourceManager::Clear();
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    // When a user presses the escape key, we set the WindowShouldClose property to true, closing the application
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            level_editor.Keys[key] = GL_TRUE;
        else if (action == GLFW_RELEASE)
            level_editor.Keys[key] = GL_FALSE;
    }
}

void window_resize_handler(GLFWwindow* window, int windowWidth, int windowHeight)
{
    int virtual_width = 1400;
    int virtual_height = 900;
    
    int screen_width = windowWidth;
    int screen_height = windowHeight;

    
    double targetAspectRatio = (double)virtual_width/virtual_height;
    
    int width = screen_width;
    int height = (int)(width / targetAspectRatio + 0.5);

    if (height > screen_height)
    {
        height = screen_height;
        width = (int)(height * targetAspectRatio + 0.5);
    }

    int vp_x = (screen_width  / 2) - (width / 2);
    int vp_y = (screen_height / 2) - (height/ 2);

    glViewport(vp_x,vp_y,width,height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, screen_width, screen_height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    double scale_x = (double)(((double)(screen_width)) / ((double)virtual_width));
    double scale_y = (double)(((double)(screen_height)) / ((double)virtual_height));
    glScalef(scale_x, scale_y, 1.0f);

    glBegin(GL_TRIANGLES);
    glColor3f(0.1, 0.2, 0.3);
    glVertex3f(0,  0, 0);
    glVertex3f(50, 0, 0);
    glVertex3f(0, 50, 0);
    glEnd();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();   
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
