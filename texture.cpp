#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "texture.h"
#include <iostream>

Texture::Texture() : m_id(0), m_width(0), m_height(0), m_channels(0) {}

Texture::~Texture() {
    if (m_id) {
        glDeleteTextures(1, &m_id);
    }
}

bool Texture::loadFromFile(const std::string& path) {
    // Переворачиваем изображение по вертикали
    stbi_set_flip_vertically_on_load(true);

    // Загружаем изображение через stb_image
    unsigned char* data = stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 0);

    if (!data) {
        std::cerr << "(!): Couldn't load texture " << path << std::endl;
        return false;
    }

    // Определяем формат пикселей
    GLenum format;
    if (m_channels == 1)      format = GL_RED;
    else if (m_channels == 3) format = GL_RGB;
    else if (m_channels == 4) format = GL_RGBA;
    else                      format = GL_RGB;

    // Создаём текстуру OpenGL
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    // Настройка параметров текстуры
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Повтор по U
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Повтор по V
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Фильтр для уменьшения
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Фильтр для увеличения

    // Загружаем данные в текстуру
    glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);

    // Генерируем mipmap уровни
    glGenerateMipmap(GL_TEXTURE_2D);

    // Освобождаем память изображения
    stbi_image_free(data);

    std::cout << "The texture is loaded: " << path << " (" << m_width << "x" << m_height << ")" << std::endl;
    return true;
}

void Texture::bind(unsigned int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}