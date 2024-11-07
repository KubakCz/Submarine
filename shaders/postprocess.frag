#version 450

// ----------------------------------------------------------------------------
// Input Variables
// ----------------------------------------------------------------------------

layout(binding = 0) uniform sampler2D input_color;
layout(binding = 1) uniform sampler2D input_depth;
layout(location = 2) uniform float sub_depth;

// ----------------------------------------------------------------------------
// Output Variables
// ----------------------------------------------------------------------------

// The final output color.
layout(location = 0) out vec4 final_color;

// ----------------------------------------------------------------------------
// Local Variables
// ----------------------------------------------------------------------------

const float near = 0.01f;
const float far = 150.0f;

const vec3 fog_color = vec3(.4f, .8f, 1.0f);

const float density = 1.3;
const float gradient = 5.5;

// ----------------------------------------------------------------------------
// Functions
// ----------------------------------------------------------------------------

// https://learnopengl.com/Advanced-OpenGL/Depth-testing
float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main() {
	float depth = LinearizeDepth(texelFetch(input_depth, ivec2(gl_FragCoord.xy), 0).r) / far;
	vec3 color = texelFetch(input_color, ivec2(gl_FragCoord.xy), 0).rgb;

	float fog_coefficient = exp(-pow(depth * density, gradient));

	// attenuation
    vec3 attenuation = vec3(
        pow(1.13, -pow(sub_depth, 1.05)),
        pow(1.08, -sub_depth),
        pow(1.07, -pow(sub_depth, .81))
    );

	final_color = vec4(mix(fog_color * attenuation, color, fog_coefficient), 1.0);
}
