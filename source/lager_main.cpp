#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
//#include <GL/glxew.h>
//#include <GL/glx.h>
#include <iostream>
#include "lager_game.h"
// GLFW function declerations
void lager_key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
GLuint LAGER_SCREEN_WIDTH = 1400;
GLuint LAGER_SCREEN_HEIGHT = 900;

//GLdouble width_ratio = 1.0f;
//GLdouble height_ratio = 1.0f;

const GLuint default_screen_width = 1000;
const GLuint default_screen_height = 1000;

Lager_Game Lager_Project(LAGER_SCREEN_WIDTH, LAGER_SCREEN_HEIGHT);

int lager_main(int argc, char *argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(LAGER_SCREEN_WIDTH, LAGER_SCREEN_HEIGHT, "Gymnasiearbete", nullptr, nullptr);

    glfwMakeContextCurrent(window);
    //glewExperimental = GL_TRUE;
    glewInit();
    glGetError(); // Call it once to catch glewInit() bug, all other errors are now from our application.

    glfwSetKeyCallback(window, lager_key_callback);

    //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // OpenGL configuration
    glViewport(0, 0, LAGER_SCREEN_WIDTH, LAGER_SCREEN_HEIGHT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Initialize game
    Lager_Project.Init();
    std::cout << "Init done" << std::endl;
    // DeltaTime variables
    double deltaTime = 0.0f;
    double lastFrame = 0.0f;
    // Start Game in GAME_ACTIVE STATE
    //Lager_Project.State = GAME_ACTIVE;

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
        Lager_Project.Input(deltaTime);
        if (Lager_Project.Exit)
            glfwDestroyWindow(window);

        int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        // Update Game state
        Lager_Project.Update(deltaTime);

        // Render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Lager_Project.Render();

        glfwSwapBuffers(window);

        //0 is for vsync off, 1 is for vsync on
        glfwSwapInterval(0);

    }
    // Delete all resources as loaded using the resource manager
    ResourceManager::Clear();

    glfwTerminate();
    return 0;
}

void lager_key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    // When a user presses the escape key, we set the WindowShouldClose property to true, closing the application
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            Lager_Project.Keys[key] = GL_TRUE;
        else if (action == GLFW_RELEASE)
            Lager_Project.Keys[key] = GL_FALSE;
    }
}
