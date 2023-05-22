#include "PanningCamera.h"

#include <cmath>

#include <glm/gtx/transform.hpp>

#include "utility/Math.h"
#include "rendering/imgui/ImGuiManager.h"

const float PAN_AROUND_SPEED = 1.0f; //constant for speed of camera pan

PanningCamera::PanningCamera() : distance(init_distance), focus_point(init_focus_point), pitch(init_pitch), yaw(init_yaw), near(init_near), fov(init_fov) {}

PanningCamera::PanningCamera(float distance, glm::vec3 focus_point, float pitch, float yaw, float near, float fov)
    : init_distance(distance), init_focus_point(focus_point), init_pitch(pitch), init_yaw(yaw), init_near(near), init_fov(fov), distance(distance), focus_point(focus_point), pitch(pitch), yaw(yaw), near(near), fov(fov) {}

void PanningCamera::update(const Window& window, float dt, bool controls_enabled) {
    if (controls_enabled) {
        bool ctrl_is_pressed = window.is_key_pressed(GLFW_KEY_LEFT_CONTROL) || window.is_key_pressed(GLFW_KEY_RIGHT_CONTROL);
       //implementation for Part I
       //If key 'S' is pressed
        bool pan_around_is_pressed = window.is_key_pressed(GLFW_KEY_S);

        bool resetSeq = false;
        if (window.was_key_pressed(GLFW_KEY_R) && !ctrl_is_pressed) {
            reset();
            resetSeq = true;
        }

        if (!resetSeq) {
            // Extract basis vectors from inverse view matrix to find world space directions of view space basis
            auto x_basis = glm::vec3{inverse_view_matrix[0]};
            auto y_basis = glm::vec3{inverse_view_matrix[1]};
            

            auto pan = window.get_mouse_delta(GLFW_MOUSE_BUTTON_MIDDLE);
            focus_point += (x_basis * (float) -pan.x + y_basis * (float) pan.y) * PAN_SPEED * dt * distance / (float) window.get_window_height();

            pitch -= PITCH_SPEED * (float) window.get_mouse_delta(GLFW_MOUSE_BUTTON_RIGHT).y;
            yaw -= YAW_SPEED * (float) window.get_mouse_delta(GLFW_MOUSE_BUTTON_RIGHT).x;
            distance -= ZOOM_SCROLL_MULTIPLIER * ZOOM_SPEED * window.get_scroll_delta();

            // Pan around the scene if 'S' is held down
            if (pan_around_is_pressed) {
                yaw += PAN_AROUND_SPEED * dt;
            }

            auto is_dragging = window.is_mouse_pressed(GLFW_MOUSE_BUTTON_RIGHT) || window.is_mouse_pressed(GLFW_MOUSE_BUTTON_MIDDLE);
            if (is_dragging) {
                window.set_cursor_disabled(true);

            }
        }

    }
    
yaw = std::fmod(yaw + YAW_PERIOD, YAW_PERIOD);
pitch = clamp(pitch, PITCH_MIN, PITCH_MAX);
distance = clamp(distance, MIN_DISTANCE, MAX_DISTANCE);

// Creating a rotation matrix around the x-axis using the pitch value
glm::mat4 x_rotation = glm::rotate(-pitch, glm::vec3{1.0f, 0.0f, 0.0f});
// Creating a rotation matrix around the y-axis using the yaw value
glm::mat4 y_rotation = glm::rotate(-yaw, glm::vec3{0.0f, 1.0f, 0.0f});

// This matrix transforms world coordinates to camera coordinates.
// containing several transformations: 
// 1) A translation by the focus point
// 2) Rotations around the x and y axes
// 3) A translation along the z-axis by the negative distance
view_matrix = glm::translate(glm::vec3{0.0f, 0.0f, -distance}) * x_rotation * y_rotation * glm::translate(focus_point);

//This matrix transforms camera coordinates back to world coordinates.
inverse_view_matrix = glm::inverse(view_matrix);

//using near instead of 1.0f
projection_matrix = glm::infinitePerspective(fov, window.get_framebuffer_aspect_ratio(), near);
inverse_projection_matrix = glm::inverse(projection_matrix);
}


void PanningCamera::reset() {
    distance = init_distance;
    focus_point = init_focus_point;
    pitch = init_pitch;
    yaw = init_yaw;
    fov = init_fov;
    near = init_near;
    gamma = init_gamma;
}

void PanningCamera::add_imgui_options_section(const SceneContext& scene_context) {
    if (!ImGui::CollapsingHeader("Camera Options")) {
        return;
    }

    ImGui::DragFloat3("Focus Point (x,y,z)", &focus_point[0], 0.01f);
    ImGui::DragDisableCursor(scene_context.window);

    ImGui::DragFloat("Distance", &distance, 0.01f, MIN_DISTANCE, MAX_DISTANCE);
    ImGui::DragDisableCursor(scene_context.window);

    float pitch_degrees = glm::degrees(pitch);
    ImGui::SliderFloat("Pitch", &pitch_degrees, -89.99f, 89.99f);
    pitch = glm::radians(pitch_degrees);

    float yaw_degrees = glm::degrees(yaw);
    ImGui::DragFloat("Yaw", &yaw_degrees);
    ImGui::DragDisableCursor(scene_context.window);
    yaw = glm::radians(glm::mod(yaw_degrees, 360.0f));

    ImGui::SliderFloat("Near Plane", &near, 0.001f, 1.0f, "%.3f", ImGuiSliderFlags_Logarithmic);

    float fov_degrees = glm::degrees(fov);
    ImGui::SliderFloat("FOV", &fov_degrees, 40.0f, 170.0f);
    fov = glm::radians(fov_degrees);

    ImGui::Spacing();
    ImGui::SliderFloat("Gamma", &gamma, 1.0f, 5.0f, "%.2f");

    if (ImGui::Button("Reset (R)")) {
        reset();
    }
}

CameraProperties PanningCamera::save_properties() const {
    return CameraProperties{
        get_position(),
        yaw,
        pitch,
        fov,
        gamma
    };
}

void PanningCamera::load_properties(const CameraProperties& camera_properties) {
    yaw = camera_properties.yaw;
    pitch = camera_properties.pitch;
    focus_point = camera_properties.position;
    fov = camera_properties.fov;
    gamma = camera_properties.gamma;
    distance = 1.0f;

    auto inverse_rot_matrix = glm::rotate(yaw, glm::vec3{0.0f, 1.0f, 0.0f}) * glm::rotate(pitch, glm::vec3{1.0f, 0.0f, 0.0f});
    auto forward = glm::vec3{inverse_rot_matrix[2]};

    focus_point -= distance * forward;
}


glm::mat4 PanningCamera::get_view_matrix() const {
    return view_matrix;
}

glm::mat4 PanningCamera::get_inverse_view_matrix() const {
    return inverse_view_matrix;
}

glm::mat4 PanningCamera::get_projection_matrix() const {
    return projection_matrix;
}

glm::mat4 PanningCamera::get_inverse_projection_matrix() const {
    return inverse_projection_matrix;
}

float PanningCamera::get_gamma() const {
    return gamma;
}