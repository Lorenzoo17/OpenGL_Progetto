#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "Camera.h"
#include "texture.h"
#include "resource_manager.h"
#include "game_object.h"

#include <iostream>
#include "game.h"
#include "time.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const float CAMERASPEED = 2.5f;


float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX = SCR_WIDTH/2, lastY = SCR_HEIGHT/2;

Game MyGame(SCR_WIDTH, SCR_HEIGHT);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback); // imposto il call back dei tasti

    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------

    // blend per rimuovere background nero dalle texture
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    MyGame.Init();

    // render loop
    // -----------

    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----

        double currentTime = glfwGetTime();
        Time::setTime(currentTime);
        
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;
        Time::setDeltatime(deltaTime);
        
        // processInput(window);

        //wMyGame.ProcessInput();
        MyGame.Update();

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!
        
        MyGame.Render();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ResourceManager::Clear();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    // if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    //    glfwSetWindowShouldClose(window, true);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true); // se premo esc chiudo la finestra

    if (key >= 0 && key <= 1024) { // Se è stato premuto/rilasciato un pulsante nel range corretto
        if (action == GLFW_PRESS) { // Se il pulsante è stato premuto
            MyGame.Keys[key] = true; // metto a true la map presente in Game.h in corrispondenza del tasto premuto(permette di capire quali tasti stanno venendo premuti, anche in hold ad esempio)
        }
        else if (action == GLFW_RELEASE) {
            MyGame.Keys[key] = false; // lo tolgo dalla map (setto a false)
        }
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
