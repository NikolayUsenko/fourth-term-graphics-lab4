#version 330 core

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D uTexture; // Текстура бетона
uniform vec3 uLightPos; // Позиция источника света
uniform vec3 uViewPos; // Позиция камеры
uniform vec3 uLightColor; // Цвет света

// МАТЕРИАЛ МОКРОГО БЕТОНА
const vec3 uMaterialAmbient  = vec3(0.12, 0.12, 0.12); // Фоновое отражение
const vec3 uMaterialDiffuse  = vec3(0.45, 0.45, 0.45); // Диффузное отражение
const vec3 uMaterialSpecular = vec3(0.35, 0.35, 0.35); // Зеркальное отражение
const float uMaterialShininess = 42.0; // Степень блеска

void main() {
    // Получаем цвет из текстуры бетона
    vec4 texColor = texture(uTexture, TexCoord);
    
    // ФОНОВОЕ ОСВЕЩЕНИЕ
    float ambientStrength = 0.35;
    vec3 ambient = ambientStrength * uLightColor * uMaterialAmbient;
    
    // ДИФФУЗНОЕ ОСВЕЩЕНИЕ
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = uLightColor * diff * uMaterialDiffuse;
    
    // ЗЕРКАЛЬНОЕ ОСВЕЩЕНИЕ (Blinn-Phong)
    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 halfVec = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfVec), 0.0), uMaterialShininess);
    vec3 specular = uLightColor * spec * uMaterialSpecular;
    
    // Мокрый бетон
    vec3 wetColor = texColor.rgb * 0.5;
    wetColor = mix(wetColor, vec3(0.25, 0.28, 0.32), 0.5);
    
    // Суммируем освещение
    vec3 result = (ambient + diffuse) * wetColor + specular;
    
    FragColor = vec4(result, 1.0);
}