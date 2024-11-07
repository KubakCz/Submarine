#pragma once

#include "glm_headers.hpp"

const glm::vec3 up_vector = glm::vec3(0, 1, 0);

// ----------------------------------------------------------------------------
// UNIFORM STRUCTS
// ----------------------------------------------------------------------------
struct CameraUBO {
    glm::mat4 projection;
    glm::mat4 view;
    glm::vec4 position;
};

struct Material {
    glm::vec4 ambient_color = glm::vec4(1);
    glm::vec4 diffuse_color = glm::vec4(1);
    glm::vec4 specular_color = glm::vec4(1);
};

struct LightUBO {
    glm::vec4 position;
    
    // R, G, B
    // ambient, diffuse, and specular have the same base color
    // their ratio is computed in fragment shader based on distance to the light source
    glm::vec4 color;    

    // field for spot light - if the light is not a spot light, set direction.w to 0
    glm::vec4 direction;
    float cut_off;

    float pad1 = 1;
    float pad2 = 2;
    float pad3 = 3;
};

struct alignas(256) ObjectUBO {
    glm::mat4 model_matrix;     // [  0 -  64) bytes
    glm::vec4 ambient_color;    // [ 64 -  80) bytes
    glm::vec4 diffuse_color;    // [ 80 -  96) bytes
    glm::vec4 specular_color;   // [ 96 - 112) bytes // Contains shininess in .w element
    unsigned int has_texture;
};

struct TransformBase {
    glm::vec3 position;
    float pad1;
    glm::vec3 velocity;
    float pad2;
};
