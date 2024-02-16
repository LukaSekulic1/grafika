#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
unsigned int loadCubemap(vector<std::string> faces);
unsigned int loadTexture(char const* path);
int random(int min,int max);



// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int br=0;
int scale = 30;
bool lightOn = false;

struct PointLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

Camera camera;
bool CameraMouseMovementUpdateEnabled = true;

glm::vec3 UFOPosition = glm::vec3(0.0f,0.0f,-15.0f);
float UFOScale = 1.0f;

int main() {
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
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(false);


    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // build and compile shaders
    // -------------------------
    Shader ourShader("resources/shaders/advance_lighting.vs", "resources/shaders/advance_lighting.fs");
    Shader skyboxShader("resources/shaders/skybox.vs","resources/shaders/skybox.fs");
    Shader blendingShader("resources/shaders/blending.vs","resources/shaders/blending.fs");

    // load models
    // -----------
    Model ourModel("resources/objects/UFO/13884_UFO_Saucer_v1_l2.obj");
    ourModel.SetShaderTextureNamePrefix("material.");

    // skybox
    //---------------------------------------------------------

    float skyboxVertices[] = {
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };

    float transparentVertices[] = {
            0.0f,0.5f,0.0f,0.0f,0.0f,
            0.0f,-0.5f,0.0f,0.0f,1.0f,
            1.0f,-0.5f,0.0f,1.0f,1.0f,

            0.0f,0.5f,0.0f,0.0f,0.0f,
            1.0f,-0.5f,0.0f,1.0f,1.0f,
            1.0f,0.5f,0.0f,1.0f,0.0f
    };

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1,&transparentVAO);
    glGenBuffers(1,&transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER,transparentVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(transparentVertices),transparentVertices,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int transparentTexture = loadTexture(FileSystem::getPath("resources/textures/lightning.png").c_str());

    vector<std::string> faces =
            {
            FileSystem::getPath("resources/textures/skybox/right.jpg"),
            FileSystem::getPath("resources/textures/skybox/left.jpg"),
            FileSystem::getPath("resources/textures/skybox/top.jpg"),
            FileSystem::getPath("resources/textures/skybox/bottom.jpg"),
            FileSystem::getPath("resources/textures/skybox/front.jpg"),
            FileSystem::getPath("resources/textures/skybox/back.jpg"),
            };

    unsigned int cubemapTexture = loadCubemap(faces);

    vector<glm::vec3> coord{
        glm::vec3(-10.0f,2.0f,5.0f),
        glm::vec3(10.0f,20.0f,10.0f),
        glm::vec3(-50.0f,5.0f,-26.0f),
        glm::vec3(-34.0f,30.0f,-72.0f),
        glm::vec3(-10.0f,20.0f,-90.0f),
        glm::vec3(0.0f,10.0f,-90.0f),
        glm::vec3(0.0f,-10.0f,-90.0f),
        glm::vec3(0.0f,0.0f,-75.0f),
        glm::vec3(0.0f,0.0f,0.0f)

    };

    //---------------------------------------------------



    PointLight pointLight;
    pointLight.position = glm::vec3(4.0f, 4.0, 0.0);
    pointLight.ambient = glm::vec3(0.1, 0.1, 0.1);
    pointLight.diffuse = glm::vec3(0.6, 0.6, 0.6);
    pointLight.specular = glm::vec3(1.0, 1.0, 1.0);

    pointLight.constant = 1.0f;
    pointLight.linear = 0.08f;
    pointLight.quadratic = 0.032f;

    SpotLight spotLight;
    spotLight.position = glm::vec3(UFOPosition.x,UFOPosition.y-2,UFOPosition.z);
    spotLight.direction = glm::vec3(0,-1,0);
    spotLight.ambient = glm::vec3(1.0f,1.0f,1.0f);
    spotLight.diffuse = glm::vec3(0.7f,0.7f,0.7f);
    spotLight.specular = glm::vec3(1.0f,1.0f,1.0f);
    spotLight.constant = 0.1f;
    spotLight.linear = 0.05f;
    spotLight.quadratic = 0.012f;
    spotLight.cutOff = glm::cos(glm::radians(5.5f));
    spotLight.outerCutOff = glm::cos(glm::radians(35.0f));


    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    skyboxShader.use();
    skyboxShader.setInt("skybox",0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);


        // render
        // ------
        glClearColor(0.0f,0.0f,0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // don't forget to enable shader before setting uniforms
        ourShader.use();
        //pointLight.position = glm::vec3(4.0 * cos(currentFrame), 4.0f, 4.0 * sin(currentFrame));
        pointLight.position = glm::vec3(UFOPosition.x,UFOPosition.y + 3,UFOPosition.z);
        ourShader.setVec3("pointLight.position", pointLight.position);
        ourShader.setVec3("pointLight.ambient", pointLight.ambient);
        ourShader.setVec3("pointLight.diffuse", pointLight.diffuse);
        ourShader.setVec3("pointLight.specular", pointLight.specular);
        ourShader.setFloat("pointLight.constant", pointLight.constant);
        ourShader.setFloat("pointLight.linear", pointLight.linear);
        ourShader.setFloat("pointLight.quadratic", pointLight.quadratic);

        ourShader.setVec3("viewPosition", camera.Position);
        ourShader.setFloat("material.shininess", 32.0f);

        spotLight.position = glm::vec3(UFOPosition.x,UFOPosition.y-2,UFOPosition.z);
        ourShader.setVec3("spotLight.position", spotLight.position);
        ourShader.setVec3("spotLight.direction", spotLight.direction);
        ourShader.setVec3("spotLight.ambient", spotLight.ambient);
        ourShader.setVec3("spotLight.diffuse", spotLight.diffuse);
        ourShader.setVec3("spotLight.specular", spotLight.specular);
        ourShader.setFloat("spotLight.constant", spotLight.constant);
        ourShader.setFloat("spotLight.linear", spotLight.linear);
        ourShader.setFloat("spotLight.quadratic", spotLight.quadratic);
        ourShader.setFloat("spotLight.cutOff", spotLight.cutOff);
        ourShader.setFloat("spotLight.outerCutOff", spotLight.outerCutOff);
        ourShader.setBool("lightOn",lightOn);

        // view/projection transformations
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model,
                               UFOPosition); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(UFOScale/30));    // it's a bit too big for our scene, so scale it down
        model = glm::rotate(model,glm::radians(90.0f),glm::vec3(1,0,0));
        model = glm::rotate(model,(float)glfwGetTime()*2,glm::vec3(0,0,-1));
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        skyboxShader.setMat4("view",glm::mat4(glm::mat3(view)));
        skyboxShader.setMat4("projection",projection);
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP,cubemapTexture);
        glDrawArrays(GL_TRIANGLES,0,36);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);


        glDisable(GL_CULL_FACE);
        blendingShader.use();
        blendingShader.setMat4("view",view);
        blendingShader.setMat4("projection",projection);
        glBindVertexArray(transparentVAO);
        glBindTexture(GL_TEXTURE_2D,transparentTexture);
        //if(lightning) {
            //for (unsigned int i = 0; i < coord.size(); i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, coord[br]);
            model = glm::scale(model, glm::vec3(scale));
            blendingShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            br = random(0,coord.size()-1);
            scale = random(10,80);
            //}
        //}

        glEnable(GL_CULL_FACE);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if(glfwGetKey(window,GLFW_KEY_UP)==GLFW_PRESS){
        UFOPosition.z -= 0.3;
    }
    if(glfwGetKey(window,GLFW_KEY_DOWN)==GLFW_PRESS){
        UFOPosition.z += 0.3;
    }
    if(glfwGetKey(window,GLFW_KEY_LEFT)==GLFW_PRESS){
        UFOPosition.x -= 0.2;
    }
    if(glfwGetKey(window,GLFW_KEY_RIGHT)==GLFW_PRESS){
        UFOPosition.x += 0.2;
    }
    if(glfwGetKey(window,GLFW_KEY_N)==GLFW_PRESS){
        UFOPosition.y += 0.3;
    }
    if(glfwGetKey(window,GLFW_KEY_M)==GLFW_PRESS){
        UFOPosition.y -= 0.3;
    }
    if(glfwGetKey(window,GLFW_KEY_O)==GLFW_PRESS){
        lightOn = true;
    }
    if(glfwGetKey(window,GLFW_KEY_P)==GLFW_PRESS){
        lightOn = false;
    }



}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (CameraMouseMovementUpdateEnabled)
        camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {

    }
}

unsigned int loadCubemap(vector<std::string> faces){
    unsigned int textureID;
    glGenTextures(1,&textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP,textureID);
    int width, height, nrChannels;
    unsigned char* data;

    for(int i=0;i<faces.size();i++){
        data = stbi_load(faces[i].c_str(),&width,&height,&nrChannels,0);
        if(data){
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,data);

        }
        else{
            std::cerr << "Failed to load cube map texture face\n";
            return -1;
        }
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);

    return textureID;
}

unsigned int loadTexture(char const* path){
    unsigned int textureID;
    glGenTextures(1,&textureID);
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        if(nullptr != strstr(path,"lightning.png")){
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        }
        else{
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

int random(int min,int max){
    std::srand(glfwGetTime());
    return min + std::rand()%(max-min+1);
}