#version 330 core

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D uTexture; // Текстура
uniform vec3 uLightPos; // Позиция источника света
uniform vec3 uViewPos; // Позиция камеры
uniform vec3 uLightColor; // Цвет света
uniform float uTime; // Время для эффекта патины

// МАТЕРИАЛ БРОНЗЫ
const vec3 uMaterialAmbient  = vec3(0.25, 0.15, 0.05); // Фоновое отражение
const vec3 uMaterialDiffuse  = vec3(0.80, 0.50, 0.20); // Диффузное отражение
const vec3 uMaterialSpecular = vec3(0.90, 0.70, 0.40); // Зеркальное отражение
const float uMaterialShininess = 32.0; // Степень блеска

void main() {
    // Получаем цвет из текстуры
    vec4 texColor = texture(uTexture, TexCoord);
    
    // ФОНОВОЕ ОСВЕЩЕНИЕ
    float ambientStrength = 0.25;
    vec3 ambient = ambientStrength * uLightColor * uMaterialAmbient;
    
    // ДИФФУЗНОЕ ОСВЕЩЕНИЕ
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = uLightColor * diff * uMaterialDiffuse;
    
    // ЗЕРКАЛЬНОЕ ОСВЕЩЕНИЕ
    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 halfVec = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfVec), 0.0), uMaterialShininess);
    vec3 specular = uLightColor * spec * uMaterialSpecular;
    
    // ПАТИНИРОВАНИЕ
    vec3 patinaColor = vec3(0.35, 0.55, 0.35);
    
    // Интенсивность патины увеличивается со временем
    float patinaIntensity = sin(uTime * 0.3) * 0.5 + 0.3;
    patinaIntensity = max(0.0, (patinaIntensity - 0.4)) * 0.7;
    
    float patinaMask = (1.0 - spec) * 0.6;
    
    // Суммируем освещение
    vec3 litColor = (ambient + diffuse) * texColor.rgb + specular;
    
    // Добавляем эффект патины
    vec3 result = mix(litColor, patinaColor, patinaIntensity * patinaMask);
    
    FragColor = vec4(result, 1.0);
}