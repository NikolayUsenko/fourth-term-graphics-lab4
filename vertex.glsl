#version 330 core

layout(location = 0) in vec3 aPos; // Позиция вершины
layout(location = 1) in vec3 aNormal; // Нормаль
layout(location = 2) in vec2 aTexCoord; // Текстурные координаты

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 uModel; // Модельная матрица
uniform mat4 uView; // Матрица камеры
uniform mat4 uProjection; // Проекционная матрица

void main() {
    // Позиция вершины в мировых координатах
    FragPos = vec3(uModel * vec4(aPos, 1.0));
    
    // Преобразование нормали
    Normal = mat3(transpose(inverse(uModel))) * aNormal;
    
    // Передаём текстурные координаты
    TexCoord = aTexCoord;
    
    // Итоговая позиция в clip space
    gl_Position = uProjection * uView * vec4(FragPos, 1.0);
}