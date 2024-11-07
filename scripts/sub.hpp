#pragma once

#include "uniform_structs.hpp"
#include "loader.hpp"
#include "light_manager.hpp"
#include "sphere.hpp"

class Sub : public TransformBase {
    // ----------------------------------------------------------------------------
    // Variables
    // ----------------------------------------------------------------------------
public:
    // constants
    const glm::vec3 acceleration_const = glm::vec3(4, 4, 8);
    const float angular_acceleration_const = 1;
    const glm::vec3 drag_const = glm::vec3(1, 1, .1f);
    const float angular_drag_const = 4;

    const float max_depth = -49;
    const float min_depth = -5;

    // some of these variables don't have to be here in general
    // but I created them to be abel to show values in debug info window
    // It would be maybe better, to create method for each value,
    // but they use same values, that would be computed several times.

    // lcl
    glm::vec3 lcl_velocity = glm::vec3();
    glm::vec3 lcl_input = glm::vec3();
    glm::vec3 lcl_drag = glm::vec3();
    glm::vec3 lcl_sas = glm::vec3();
    float vertical_limiter = 0;
    glm::vec3 lcl_acceleration = glm::vec3();

    // glb
    glm::vec3 acceleration = glm::vec3();
    // velocity - part of TransformBase
    // position - part of TransformBase


    // rot
    float angular_input = 0;
    float angular_drag = 0;
    float angular_sas = 0;
    float angular_acceleration = 0;
    float angular_velocity = 0;
    float rotation = 0;

    bool sas = false;

private:
    // inputs
    glm::ivec3 input_move = glm::ivec3();
    int input_rotation = 0;    

    // model
    std::shared_ptr<Geometry> geometry;
    GLuint texture;
    GLuint ubo_buffer;
    ObjectUBO ubo = {
        .model_matrix = glm::mat4(1),
        .ambient_color = glm::vec4(1),
        .diffuse_color = glm::vec4(1),
        .specular_color = glm::vec4(1, 1, 1, 32),
        .has_texture = 1
    };

    LightManager& light_manager;
    unsigned int light1, light2;
    const glm::vec3 light_offset1 = glm::vec3(.74732f, .81174f, -.85f);
    const glm::vec3 light_offset2 = light_offset1 * glm::vec3(-1, 1, 1);
    const glm::vec3 light_direction = glm::vec3(0.0f, -0.1f, -1.0f);
    const float light_cut_off = M_PI / 5;

    // camera
    const glm::vec3 camera_offset = glm::vec3(0, 4, 10);
    const glm::vec3 look_at_offset = glm::vec3(0, 2, 0);

    // ----------------------------------------------------------------------------
    // Constructors & Destructors
    // ----------------------------------------------------------------------------
public:
    Sub(const Loader& loader, LightManager& light_manager, glm::vec3 position);
    ~Sub();

    // ----------------------------------------------------------------------------
    // Methods
    // ----------------------------------------------------------------------------
    void update(float delta);
    void draw();
    void update_camera_ubo(CameraUBO *camera);
    void update_lights_ubos();

    // ----------------------------------------------------------------------------
    // Input Events
    // ----------------------------------------------------------------------------
    void on_key_pressed(int key, int scancode, int action, int mods);
};