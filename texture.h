#pragma once
#include <glad/glad.h>
#include <string>

class Texture {
public:
    Texture();
    ~Texture();

    bool loadFromFile(const std::string& path); // Загрузка текстуры из файла
    void bind(unsigned int unit = 0) const; // Привязка текстурного юнита
    void unbind() const; // Отвязка текстуры

    unsigned int getId() const { return m_id; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    unsigned int m_id; // Идентификатор текстуры OpenGL
    int m_width; // Ширина текстуры
    int m_height; // Высота текстуры
    int m_channels; // Количество каналов (RGB=3, RGBA=4)
};