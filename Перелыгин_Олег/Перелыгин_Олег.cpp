#define GLEW_DLL
#define GLFW_DLL

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include "ShaderLoader.h"
#include "Model.h"
#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 256.0f;
float lastY = 256.0f;
float fov = 45.0f;

struct ObjectTransform {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 pivotPoint = glm::vec3(0.0f);

    struct {
        float min = -5.0f;
        float max = 5.0f;
    } zLimit;

    struct {
        float min = -90.0f;
        float max = 90.0f;
    } rotationLimit;
};

std::vector<ObjectTransform> objectTransforms;

void processInput(GLFWwindow* window) {
    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    float moveSpeed = 1.5f * deltaTime;
    float rotateSpeed = 50.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        objectTransforms[1].rotation.z += rotateSpeed;

        if (objectTransforms[1].rotation.z > objectTransforms[1].rotationLimit.max)
            objectTransforms[1].rotation.z = objectTransforms[1].rotationLimit.max;
    }
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        objectTransforms[1].rotation.z -= rotateSpeed;

        if (objectTransforms[1].rotation.z < objectTransforms[1].rotationLimit.min)
            objectTransforms[1].rotation.z = objectTransforms[1].rotationLimit.min;
    }

    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        objectTransforms[2].rotation.z += rotateSpeed;

        if (objectTransforms[2].rotation.z > objectTransforms[2].rotationLimit.max)
            objectTransforms[2].rotation.z = objectTransforms[2].rotationLimit.max;
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        objectTransforms[2].rotation.z -= rotateSpeed;

        if (objectTransforms[2].rotation.z < objectTransforms[2].rotationLimit.min)
            objectTransforms[2].rotation.z = objectTransforms[2].rotationLimit.min;
    }

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        objectTransforms[3].position.x += moveSpeed;

        if (objectTransforms[3].position.x > objectTransforms[3].zLimit.max)
            objectTransforms[3].position.x = objectTransforms[3].zLimit.max;
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        objectTransforms[3].position.x -= moveSpeed;

        if (objectTransforms[3].position.x < objectTransforms[3].zLimit.min)
            objectTransforms[3].position.x = objectTransforms[3].zLimit.min;
    }
}

glm::mat4 calculateModelMatrix(int index) {
    glm::mat4 model = glm::mat4(1.0f);

    if (index == 0) {
        return model;
    }

    if (index == 1) {
        model = glm::translate(model, objectTransforms[1].pivotPoint);
        model = glm::rotate(model, glm::radians(objectTransforms[1].rotation.z), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, -objectTransforms[1].pivotPoint);
        return model;
    }

    if (index == 2) {
        model = glm::translate(model, objectTransforms[1].pivotPoint);
        model = glm::rotate(model, glm::radians(objectTransforms[1].rotation.z), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, -objectTransforms[1].pivotPoint);

        model = glm::translate(model, objectTransforms[2].pivotPoint);
        model = glm::rotate(model, glm::radians(objectTransforms[2].rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, -objectTransforms[2].pivotPoint);
        return model;
    }

    if (index == 3) {
        model = glm::translate(model, objectTransforms[1].pivotPoint);
        model = glm::rotate(model, glm::radians(objectTransforms[1].rotation.z), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, -objectTransforms[1].pivotPoint);

        model = glm::translate(model, objectTransforms[2].pivotPoint);
        model = glm::rotate(model, glm::radians(objectTransforms[2].rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, -objectTransforms[2].pivotPoint);

        model = glm::translate(model, objectTransforms[3].position);
        return model;
    }

    return model;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    fov -= (float)yoffset;
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 45.0f) fov = 45.0f;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

void settingMat4(int ID, const std::string& name, const glm::mat4& mat) {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void settingMat3(int ID, const std::string& name, const glm::mat3& mat) {

    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void setVec3(Shader shader, const char* name, glm::vec3& value) {
    glUniform3fv(glGetUniformLocation(shader.ID, name), 1, &value[0]);
}

int main() {

    setlocale(LC_ALL, "Russian");
    cout << "Перелыгин Олег лаба 6!" << endl;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Lighting Model", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    objectTransforms.resize(4);

    objectTransforms[1].pivotPoint = glm::vec3(0.0f, 0.0f, 1.58f);
    objectTransforms[2].pivotPoint = glm::vec3(0.0f, 1.45f, 0.0f);

    objectTransforms[1].rotationLimit = { -90.0f, 90.0f };
    objectTransforms[2].rotationLimit = { -20.0f, 50.0f };
    objectTransforms[3].zLimit = { 0.0f, 1.0f };

    glewExperimental = true;
    glewInit();

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);

    Shader shader("vertex_shader.glsl", "fragment_shader.glsl");
    Model model("oleg.obj");

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.4f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 modelMat = glm::mat4(1.0f);

        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMat));

        //свет
        glUniform3fv(glGetUniformLocation(shader.ID, "viewPos"), 1, glm::value_ptr(cameraPos));

        glUniform3f(glGetUniformLocation(shader.ID, "light.position"), 3.0f, 5.0f, 3.0f); 
        glUniform3f(glGetUniformLocation(shader.ID, "light.ambient"), 0.5f, 0.5f, 0.5f);   
        glUniform3f(glGetUniformLocation(shader.ID, "light.diffuse"), 1.0f, 1.0f, 1.0f);   
        glUniform3f(glGetUniformLocation(shader.ID, "light.specular"), 1.0f, 1.0f, 1.0f);  

        //материал
        glUniform3f(glGetUniformLocation(shader.ID, "material.ambient"), 0.6f, 0.8f, 0.7f);  
        glUniform3f(glGetUniformLocation(shader.ID, "material.diffuse"), 0.3f, 0.8f, 0.7f);   
        glUniform3f(glGetUniformLocation(shader.ID, "material.specular"), 0.9f, 0.9f, 0.8f);  
        glUniform1f(glGetUniformLocation(shader.ID, "material.shininess"), 32.0f);

        for (size_t i = 0; i < model.meshTransforms.size(); ++i) {
            model.meshTransforms[i] = calculateModelMatrix(i);
        }

        model.Draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}