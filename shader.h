#pragma once
#include <glad/glad.h>
#include <string>

class Shader {
public:
    Shader();
    ~Shader();

    bool load(const char* vertexPath, const char* fragmentPath); // Загрузка шейдеров из файлов
    void use() const; // Активация шейдерной программы
    unsigned int getProgram() const { return m_program; }
    int getUniformLocation(const char* name) const;

private:
    unsigned int m_program; // Идентификатор шейдерной программы

    bool checkCompileErrors(unsigned int shader, const char* type);
    bool checkLinkErrors();
};