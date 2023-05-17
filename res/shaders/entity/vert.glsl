#version 410 core
#include "../common/lights.glsl"

// Per vertex data
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texture_coordinate;

out VertexOut {
    vec2 texture_coordinate;
} vertex_out;

out vec3 ws_position;
out vec3 ws_normal;

// Per instance data
uniform mat4 model_matrix;
uniform mat3 normal_matrix;

// Global data
uniform vec3 ws_view_position;
uniform mat4 projection_view_matrix;

void main() {
    // Transform vertices
    ws_position = (model_matrix * vec4(vertex_position, 1.0f)).xyz;
    ws_normal = normalize(normal_matrix * normal);
    vertex_out.texture_coordinate = texture_coordinate;

    gl_Position = projection_view_matrix * vec4(ws_position, 1.0f);
}
