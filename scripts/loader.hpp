#pragma once

#include "geometry.hpp"

class Loader {
public:
    std::shared_ptr<Geometry> sub_geometry;
    GLuint sub_texture;

    std::shared_ptr<Geometry> fish_geometry;
    GLuint fish_texture;

    std::shared_ptr<Geometry> floor_geometry;
    std::shared_ptr<Geometry> surface_geometry;
    GLuint wavy_texture;

    Loader(const std::filesystem::path textures_path, const std::filesystem::path objects_path);
    ~Loader();

    static GLuint load_texture_2d(const std::filesystem::path filename);
    static std::shared_ptr<Geometry> load_geometry(const std::filesystem::path filename);
};