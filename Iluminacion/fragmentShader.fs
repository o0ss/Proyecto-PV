#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec3 Normal;  
in vec3 FragPos; 
in vec2 Texture;

uniform Material material; 
uniform Light light_directional;
uniform Light light_p1;
uniform Light light_p2;
uniform Light light_p3;
uniform Light light_p4;

//uniform vec3 objectColor;
uniform vec3 viewPos;

void main()
{
    // point light 1
    // ambiental
    vec3 ambient_p1  = light_p1.ambient  * vec3(texture(material.diffuse, Texture));

    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light_p1.position - FragPos);
    float distance = length(light_p1.position - FragPos);
    float attenuation = 1.0 / (light_p1.constant + light_p1.linear * distance + light_p1.quadratic * (distance * distance));
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse_p1  = light_p1.diffuse  * diff * vec3(texture(material.diffuse, Texture)) * attenuation;
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular_p1 = light_p1.specular * spec * vec3(texture(material.specular, Texture)) * attenuation;
    

    // point light 2
    // ambiental
    vec3 ambient_p2  = light_p2.ambient  * vec3(texture(material.diffuse, Texture));

    // diffuse
    vec3 lightDir2 = normalize(light_p2.position - FragPos);
    float distance2 = length(light_p2.position - FragPos);
    float attenuation2 = 1.0 / (light_p2.constant + light_p2.linear * distance2 + light_p2.quadratic * (distance2 * distance2));
    float diff2 = max(dot(norm, lightDir2), 0.0);
    vec3 diffuse_p2  = light_p2.diffuse  * diff2 * vec3(texture(material.diffuse, Texture)) * attenuation2;

    // specular
    vec3 reflectDir2 = reflect(-lightDir2, norm);
    float spec2 = pow(max(dot(viewDir, reflectDir2), 0.0), material.shininess);
    vec3 specular_p2 = light_p2.specular * spec2 * vec3(texture(material.specular, Texture)) * attenuation2;


    // point light 3
    // ambiental
    vec3 ambient_p3  = light_p3.ambient  * vec3(texture(material.diffuse, Texture));

    // diffuse
    vec3 lightDir3 = normalize(light_p3.position - FragPos);
    float distance3 = length(light_p3.position - FragPos);
    float attenuation3 = 1.0 / (light_p3.constant + light_p3.linear * distance3 + light_p3.quadratic * (distance3 * distance3));
    float diff3 = max(dot(norm, lightDir3), 0.0);
    vec3 diffuse_p3  = light_p3.diffuse  * diff3 * vec3(texture(material.diffuse, Texture)) * attenuation3;

    // specular
    vec3 reflectDir3 = reflect(-lightDir3, norm);
    float spec3 = pow(max(dot(viewDir, reflectDir3), 0.0), material.shininess);
    vec3 specular_p3 = light_p3.specular * spec3 * vec3(texture(material.specular, Texture)) * attenuation3;


    // point light 4
    // ambiental
    vec3 ambient_p4  = light_p4.ambient  * vec3(texture(material.diffuse, Texture));

    // diffuse
    vec3 lightDir4 = normalize(light_p4.position - FragPos);
    float distance4 = length(light_p4.position - FragPos);
    float attenuation4 = 1.0 / (light_p4.constant + light_p4.linear * distance4 + light_p4.quadratic * (distance4 * distance4));
    float diff4 = max(dot(norm, lightDir4), 0.0);
    vec3 diffuse_p4  = light_p4.diffuse  * diff4 * vec3(texture(material.diffuse, Texture)) * attenuation4;

    // specular
    vec3 reflectDir4 = reflect(-lightDir4, norm);
    float spec4 = pow(max(dot(viewDir, reflectDir4), 0.0), material.shininess);
    vec3 specular_p4 = light_p4.specular * spec4 * vec3(texture(material.specular, Texture)) * attenuation4;


    // directional light
    // ambiental
    vec3 ambient_d = light_directional.ambient * vec3(texture(material.diffuse, Texture));

    // diffuse
    vec3 lightDir_d = normalize(-light_directional.direction);
    float diff_d = max(dot(norm, lightDir_d), 0.0);
    vec3 diffuse_d = light_directional.diffuse * diff_d * vec3(texture(material.diffuse, Texture));

    // specular
    vec3 reflectDir_d = reflect(-lightDir_d, norm);
    float spec_d = pow(max(dot(viewDir, reflectDir_d), 0.0), material.shininess);
    vec3 specular_d = light_directional.specular * spec_d * vec3(texture(material.specular, Texture));

    
    // Sumar los componentes de la luz
    vec3 ambient = ambient_p1 + ambient_p2 + ambient_p3 + ambient_p4 + ambient_d;
    vec3 diffuse = diffuse_p1 + diffuse_p2 + diffuse_p3 + diffuse_p4 + diffuse_d;
    vec3 specular = specular_p1 + specular_p2 + specular_p3 + specular_p4 + specular_d;

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}