#version 410 core
#include "../common/lights.glsl"

in VertexOut {
    vec2 texture_coordinate;
} frag_in;

in vec3 ws_position;
in vec3 ws_normal;

layout(location = 0) out vec4 out_colour;

uniform mat4 model_matrix;
uniform mat3 normal_matrix;

// Global Data
uniform float inverse_gamma;

uniform sampler2D diffuse_texture;
uniform sampler2D specular_map_texture;

// Material properties
uniform vec3 diffuse_tint;
uniform vec3 specular_tint;
uniform vec3 ambient_tint;
uniform float shininess;

uniform vec3 ws_view_position;

#if NUM_PL > 0
layout (std140) uniform PointLightArray {
    PointLightData point_lights[NUM_PL];
};
#endif

void main() {
	// per fragment lighting
	vec3 ws_view_dir = normalize(ws_view_position - ws_position);
	LightCalculatioData light_calculation_data =
		LightCalculatioData(ws_position, ws_view_dir, ws_normal);

	Material material =
		Material(diffuse_tint, specular_tint, ambient_tint, shininess);

	LightingResult lighting_result =
		total_light_calculation(
			light_calculation_data,
			material
			#if NUM_PL > 0
				,point_lights
			#endif
		);

    vec3 resolved_lighting =
		resolve_textured_light_calculation(lighting_result,
		                                   diffuse_texture,
		                                   specular_map_texture,
		                                   frag_in.texture_coordinate);

    out_colour = vec4(resolved_lighting, 1.0f);
    out_colour.rgb = pow(out_colour.rgb, vec3(inverse_gamma));
}
