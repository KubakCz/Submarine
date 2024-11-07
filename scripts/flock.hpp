#pragma once

#include "pv112_application.hpp"
#include "uniform_structs.hpp"
#include "cube.hpp"
#include "sub.hpp"


class Flock {
    // ----------------------------------------------------------------------------
    // Variables
    // ----------------------------------------------------------------------------
protected:
    const float max_speed = 6;
    const float coherence = 0.02f;
    const float separation = 0.01f;
    const float alignment = 0.05f;
    const float visual_range = 5;

    const float min_depth = -.5f;
    const float max_depth = -49.5f;
    const float hight_acc_min = 0.01f;
    const float hight_acc_max = 0.1f;

    const float max_sub_dist = 150;
    const float min_sub_dist = 8;
    const float sub_acc = 2;


    float visual_range_squared;

public:
    std::string tag = "";
    std::vector<TransformBase> fish;

protected:
    Material material = {
        .ambient_color = glm::vec4(.8f, 0.0f, 0.0f, 0.0f),
        .diffuse_color = glm::vec4(.8f, 0.0f, 0.0f, 0.0f),
        .specular_color = glm::vec4(1.0f, 1.0f, 1.0f, 64.0f)
    };
    std::shared_ptr<Geometry> geometry;
    GLuint texture;

    GLuint material_buffer;
    GLuint fish_buffer;

    // ----------------------------------------------------------------------------
    // Constructors & Destructors
    // ----------------------------------------------------------------------------
public:
    Flock(int fish_count, std::shared_ptr<Geometry> fish_geom, GLuint fish_texture);
    ~Flock();

    // ----------------------------------------------------------------------------
    // Methods
    // ----------------------------------------------------------------------------
    void update(float delta, const Sub& sub);
    void draw();
};