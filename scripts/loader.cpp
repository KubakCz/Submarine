#include "loader.hpp"
#include <tiny_obj_loader.h>
#include "glm/vec3.hpp"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Loader::Loader(const std::filesystem::path textures_path, const std::filesystem::path objects_path) {
    sub_geometry = load_geometry(objects_path / "submersible.obj");
    sub_texture = load_texture_2d(textures_path / "sub.png");

    fish_geometry = load_geometry(objects_path / "fish.obj");
    fish_texture = load_texture_2d(textures_path / "fish.png");

    floor_geometry = load_geometry(objects_path / "floor.obj");
    surface_geometry = load_geometry(objects_path / "surface.obj");
    wavy_texture = load_texture_2d(textures_path / "sand.png");
}

Loader::~Loader() {

}

// load_texture_2d copied from application.cpp
GLuint Loader::load_texture_2d(const std::filesystem::path filename) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename.generic_string().data(), &width, &height, &channels, 4);

    GLuint texture;
    glCreateTextures(GL_TEXTURE_2D, 1, &texture);

    glTextureStorage2D(texture, std::log2(width), GL_RGBA8, width, height);

    glTextureSubImage2D(texture,
        0,                         //
        0, 0,                      //
        width, height,             //
        GL_RGBA, GL_UNSIGNED_BYTE, //
        data);

    stbi_image_free(data);

    glGenerateTextureMipmap(texture);

    glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture;
}


// load geometry from file with size normalization removed and added texture y flip
std::shared_ptr<Geometry> Loader::load_geometry(const std::filesystem::path filename) {
    //return std::make_shared<Geometry>(Geometry::from_file(filename));

    const std::string extension = filename.extension().generic_string();

    if (extension == ".obj") {
        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(filename.generic_string())) {
            if (!reader.Error().empty()) {
                std::cerr << "TinyObjReader: " << reader.Error();
            }
        }

        if (!reader.Warning().empty()) {
            std::cout << "TinyObjReader: " << reader.Warning();
        }

        auto& attrib = reader.GetAttrib();
        auto& shapes = reader.GetShapes();
        auto& materials = reader.GetMaterials();

        // Take only the first shape found
        const tinyobj::shape_t& shape = shapes[0];

        std::vector<float> positions;
        std::vector<float> normals;
        std::vector<float> tex_coords;

        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            // Loop over vertices in the face.
            for (size_t v = 0; v < 3; v++) {
                // Access to vertex
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
                tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
                tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];

                tinyobj::real_t nx;
                tinyobj::real_t ny;
                tinyobj::real_t nz;
                if (!attrib.normals.empty()) {
                    nx = attrib.normals[3 * idx.normal_index + 0];
                    ny = attrib.normals[3 * idx.normal_index + 1];
                    nz = attrib.normals[3 * idx.normal_index + 2];
                }
                else {
                    nx = 0.0;
                    ny = 0.0;
                    nz = 0.0;
                }

                tinyobj::real_t tx;
                tinyobj::real_t ty;
                if (!attrib.texcoords.empty()) {
                    tx = attrib.texcoords[2 * idx.texcoord_index + 0];
                    ty = -attrib.texcoords[2 * idx.texcoord_index + 1];
                }
                else {
                    tx = 0.0;
                    ty = 0.0;
                }

                positions.insert(positions.end(), { vx, vy, vz });
                normals.insert(normals.end(), { nx, ny, nz });
                tex_coords.insert(tex_coords.end(), { tx, ty });
            }
            index_offset += 3;
        }

        const int elements_per_vertex = 3 + (!normals.empty() ? 3 : 0) + (!tex_coords.empty() ? 2 : 0);

        return std::make_shared<Geometry>(Geometry{GL_TRIANGLES, positions, {/*indices*/}, normals, {/*colors*/}, tex_coords});
    }
    std::cerr << "Extension " << extension << " not supported" << std::endl;

    return nullptr;
}