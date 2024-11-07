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

layout(location = 4) uniform float t;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texture_coordinate;

// ----------------------------------------------------------------------------
// Output Variables
// ----------------------------------------------------------------------------
layout(location = 0) out vec3 fs_position;
layout(location = 1) out vec3 fs_normal;
layout(location = 2) out vec2 fs_texture_coordinate;
layout(location = 3) out flat int fs_instance_id;

void main()
{
	Transform fish = flock[gl_InstanceID];

	// compute fish waving
	vec3 pos = position;
	pos.x += .08 * cos(position.z * 2.5 + t * 8);

	// compute model matrix according to forward vector (velocity) and position
	vec3 forward = normalize(fish.velocity);
	vec3 right = normalize(cross(vec3(0, 1, 0), forward));
	vec3 up = cross(forward, right);
	
	mat4 model_matrix = mat4(
		vec4(right, 0.0),
		vec4(up, 0.0),
		vec4(forward, 0.0),
		vec4(fish.position, 1)
	);

	fs_position = vec3(model_matrix * vec4(pos, 1));
	fs_normal = transpose(inverse(mat3(model_matrix))) * normal;
	fs_texture_coordinate = texture_coordinate;
    gl_Position = camera.projection * camera.view * vec4(fs_position, 1.0);
	fs_instance_id = gl_InstanceID;
}
