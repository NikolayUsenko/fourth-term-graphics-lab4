#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <random>
#include <chrono>
#include "cube_data.h"
#include "texture.h"
#include "shader.h"

// Размеры окна
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Обработка ввода с клавиатуры
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

// Функция для хаотичного вращения
glm::vec3 getRandomAxis(float time) {
    static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    int segment = (int)(time / 3.0f); // Меняем ось каждые 3 секунды
    static int lastSegment = -1;
    static glm::vec3 currentAxis(1.0f, 1.0f, 0.0f);

    if (segment != lastSegment) {
        lastSegment = segment;
        currentAxis = glm::normalize(glm::vec3(
            dist(rng) * 2.0f - 1.0f,
            dist(rng) * 2.0f - 1.0f,
            dist(rng) * 2.0f - 1.0f
        ));
    }
    return currentAxis;
}

int main() {
    // ИНИЦИАЛИЗАЦИЯ GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // СОЗДАНИЕ ОКНА
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Bronze Cube - Phong Lighting & Patina", NULL, NULL);
    if (window == NULL) {
        std::cout << "(!): Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // ЗАГРУЗКА GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "(!): Couldn't initialize GLAD" << std::endl;
        return -1;
    }

    // НАСТРОЙКА ГЛУБИНЫ
    glEnable(GL_DEPTH_TEST);

    // ПОДГОТОВКА ДАННЫХ КУБА
    std::vector<Vertex> vertices = Cube::getVertices();
    std::vector<unsigned int> indices = Cube::getIndices();

    // СОЗДАНИЕ VAO, VBO, EBO
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Загрузка данных вершин в VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Загрузка индексов в EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Позиция
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // Нормаль
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // Текстурные координаты
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(2);

    // ЗАГРУЗКА ТЕКСТУРЫ
    Texture texture;
    if (!texture.loadFromFile("bronze.jpg")) {
        std::cout << "(!): The texture is not loaded. Make sure that the file is bronze.jpg exists" << std::endl;
        std::cout << "You can use any other texture by renaming it to bronze.jpg" << std::endl;
    }

    // ЗАГРУЗКА ШЕЙДЕРОВ
    Shader shader;
    if (!shader.load("vertex.glsl", "fragment.glsl")) {
        std::cout << "(!): Couldn't load shaders" << std::endl;
        return -1;
    }
    shader.use();

    // ПОЛУЧЕНИЕ UNIFORM LOCATION
    int modelLoc = shader.getUniformLocation("uModel");
    int viewLoc = shader.getUniformLocation("uView");
    int projLoc = shader.getUniformLocation("uProjection");
    int lightPosLoc = shader.getUniformLocation("uLightPos");
    int viewPosLoc = shader.getUniformLocation("uViewPos");
    int lightColorLoc = shader.getUniformLocation("uLightColor");
    int timeLoc = shader.getUniformLocation("uTime");

    // НАСТРОЙКА КАМЕРЫ
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f); // Позиция камеры
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f); // На что смотрим
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // Направление "вверх"

    // НАСТРОЙКА ИСТОЧНИКА СВЕТА
    glm::vec3 lightColor = glm::vec3(1.0f, 0.95f, 0.85f); // Тёплый свет для бронзы

    // Устанавливаем текстуру на юнит 0
    glUniform1i(shader.getUniformLocation("uTexture"), 0);

    // Засекаем время для анимации
    auto startTime = std::chrono::steady_clock::now();

    // СООБЩЕНИЕ ПОЛЬЗОВАТЕЛЮ ДЛЯ ВЫХОДА
    std::cout << "- To exit, press ESC" << std::endl;

    while (!glfwWindowShouldClose(window)) {
        // Получаем текущее время
        auto currentTime = std::chrono::steady_clock::now();
        float time = std::chrono::duration<float>(currentTime - startTime).count();

        // Обработка ввода
        processInput(window);

        // Очистка экрана
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ХАОТИЧНОЕ ВРАЩЕНИЕ КУБА
        glm::vec3 randomAxis = getRandomAxis(time);
        float angle = time * 1.0f; // Скорость вращения
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, angle, randomAxis);

        // МАТРИЦЫ КАМЕРЫ И ПРОЕКЦИИ
        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // ДВИЖЕНИЕ ИСТОЧНИКА СВЕТА
        glm::vec3 lightPos = glm::vec3(
            2.0f * cos(time * 0.7f),
            1.5f + sin(time * 0.8f),
            2.0f * sin(time * 0.5f)
        );
        //glm::vec3 lightPos = glm::vec3( 2.0f, 1.5f, 2.0f ); // (!) Для демонстрации патинирования

        // ПЕРЕДАЧА UNIFORM В ШЕЙДЕР
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
        glUniform3fv(viewPosLoc, 1, glm::value_ptr(cameraPos));
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
        glUniform1f(timeLoc, time);

        // ОТРИСОВКА КУБА
        texture.bind(0);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        // Обмен буферов и обработка событий
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ОЧИСТКА РЕСУРСОВ
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}