#version 450

// ----------------------------------------------------------------------------
// Input Variables
// ----------------------------------------------------------------------------

layout(binding = 0, std140) uniform Camera {
    mat4 projection;
    mat4 view;
    vec3 position;
} camera;

struct Light {
	vec4 position;
	vec4 color;
	vec4 direction;
    float cut_off;
};

layout(binding = 1, std430) buffer Lights {
	Light[] lights;
};

layout(binding = 2, std140) uniform Object {
    mat4 model_matrix;
    vec4 ambient_color;
    vec4 diffuse_color;
    vec4 specular_color;
    uint has_texture;
} object;

layout(binding = 3) uniform sampler2D albedo_texture;

layout(location = 0) in vec3 fs_position;
layout(location = 1) in vec3 fs_normal;
layout(location = 2) in vec2 fs_texture_coordinate;

layout(location = 0) out vec4 final_color;


vec3 blinn_phong(Light light) {
    vec3 light_vector = light.position.xyz - fs_position * light.position.w;
    vec3 L = normalize(light_vector);
    vec3 N = -normalize(fs_normal);  // use -normal, becauese surface is lit from top
    vec3 E = normalize(camera.position - fs_position);
    vec3 H = normalize(L + E);

    float NdotL = max(dot(N, L), 0.0);
    float NdotH = max(dot(N, H), 0.0);

    vec3 ambient = object.ambient_color.rgb * (object.has_texture != 0 ? texture(albedo_texture, fs_texture_coordinate).rgb : vec3(1.0)) * light.color.rgb;
    vec3 diffuse = object.diffuse_color.rgb * (object.has_texture != 0 ? texture(albedo_texture, fs_texture_coordinate).rgb : vec3(1.0)) * light.color.rgb * NdotL;
    vec3 specular = object.specular_color.rgb * light.color.rgb * pow(NdotH, object.specular_color.w);

    return ambient + diffuse + specular;
}

void main() {
    vec3 color_sum = vec3(0);

    for (int i = 0; i < lights.length(); ++i) {
        if (lights[i].position.w == 0)  // compute lighting only for "sun like" light sources
            color_sum += blinn_phong(lights[i]);
    }

    color_sum /= color_sum + 1.0;                // tone mapping
    color_sum = pow(color_sum, vec3(1.0 / 2.2)); // gamma correction

    final_color = vec4(color_sum, 1.0);
}


