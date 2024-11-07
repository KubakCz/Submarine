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

struct Transform {
    vec3 position;
    vec3 velocity;
};

layout(binding = 2, std430) buffer Flock {
	Transform flock[];
};

layout(binding = 3, std140) uniform Material {
	vec4 ambient_color;
    vec4 diffuse_color;
    vec4 specular_color;
} material;

layout(binding = 3) uniform sampler2D albedo_texture;

layout(location = 0) in vec3 fs_position;
layout(location = 1) in vec3 fs_normal;
layout(location = 2) in vec2 fs_texture_coordinate;
layout(location = 3) in flat int fs_instance_id;

// ----------------------------------------------------------------------------
// Output Variables
// ----------------------------------------------------------------------------

// The final output color.
layout(location = 0) out vec4 final_color;

vec3 blinn_phong(Light light) {
    vec3 light_vector = light.position.xyz - fs_position * light.position.w;
    vec3 L = normalize(light_vector);
    
    if (light.direction.w != 0 && light.cut_off > dot(L, normalize(-light.direction.xyz)))
        return vec3(0);

    vec3 N = normalize(fs_normal);
    vec3 E = normalize(camera.position - fs_position);
    vec3 H = normalize(L + E);

    float NdotL = max(dot(N, L), 0.0);
    float NdotH = max(dot(N, H), 0.0);

    vec3 ambient = material.ambient_color.rgb * texture(albedo_texture, fs_texture_coordinate).rgb * light.color.rgb;
    vec3 diffuse = material.diffuse_color.rgb * texture(albedo_texture, fs_texture_coordinate).rgb * light.color.rgb * NdotL;
    vec3 specular = material.specular_color.rgb * light.color.rgb * pow(NdotH, material.specular_color.w);

    // distance to the light source (depth for "sun")
    float dist = light.position.w == 0 ? -fs_position.y : length(light_vector);
    
    // ratio of components
    float diffuse_percentage = max(pow(1.03, -dist) - .2, .01);
    float ambient_percentage = 1 - diffuse_percentage;
    float specular_percentage = pow(1.2, -dist);

    vec3 color = ambient * ambient_percentage + diffuse * diffuse_percentage + specular * specular_percentage;

    // attenuation
    vec3 attenuation = vec3(
        pow(1.13, -pow(dist, 1.05)),
        pow(1.08, -dist),
        pow(1.07, -pow(dist, .81))
    );

    return color * attenuation;
}

void main() {
    vec3 color_sum = vec3(0);

    for (int i = 0; i < lights.length(); ++i) {
        color_sum += blinn_phong(lights[i]);
    }

    color_sum /= color_sum + 1.0;                // tone mapping
    color_sum = pow(color_sum, vec3(1.0 / 2.2)); // gamma correction

    final_color = vec4(color_sum, 1.0);
    // final_color = vec4(flock[fs_instance_id].velocity, 0);
}
