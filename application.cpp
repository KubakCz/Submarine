#include "application.hpp"
#include "scripts/helper.hpp"

#include <memory>

using std::make_shared;

Application::Application(int initial_width, int initial_height, std::vector<std::string> arguments)
    : PV112Application(initial_width, initial_height, arguments),
        loader(lecture_folder_path / "textures", lecture_folder_path / "objects"),
        sub(loader, light_manager, glm::vec3(0, -5, 0)),
        floor(
            loader.floor_geometry, 
            300,
            -50,
            loader.wavy_texture, 
            Material{ 
                glm::vec4(223.0f / 255, 211.0f / 255, 192.0f / 255, 0.0f),
                glm::vec4(223.0f / 255, 211.0f / 255, 192.0f / 255, 0.0f),
                glm::vec4(.1f, .1f, .1f, 16.0f) }, 
            sub),
        surface(
            loader.surface_geometry,
            300,
            0,
            loader.wavy_texture,
            Material{
                glm::vec4(.01f, .04f, .1f, .0f),
                glm::vec4(.1f, .4f, .9f, .0f),
                glm::vec4(1, 1, 1, 32)},
            sub),
        flock(1000, loader.fish_geometry, loader.fish_texture) {
    this->width = initial_width;
    this->height = initial_height;

    textures_path = lecture_folder_path / "textures";
    objects_path = lecture_folder_path / "objects";

    // --------------------------------------------------------------------------
    // Initialize UBO Data
    // --------------------------------------------------------------------------
    camera_ubo.projection = glm::perspective(glm::radians(50.0f), float(width) / float(height), 0.01f, max_visibility);

    light_manager.add_sun(glm::vec3(1, 1, -2), glm::vec3(2));
    light_manager.create_buffer();


    // --------------------------------------------------------------------------
    // Create Buffers
    // --------------------------------------------------------------------------
    glCreateBuffers(1, &camera_buffer);
    glNamedBufferStorage(camera_buffer, sizeof(CameraUBO), &camera_ubo, GL_DYNAMIC_STORAGE_BIT);

    compile_shaders();

    // create frame buffers
    // Create framebuffers (glCreateFramebuffers)
    glCreateFramebuffers(1, &framebuffer);

    // Initialize color output texture with GL_RGBA32F format (glCreateTextures, glTextureStorage2D)
    glCreateTextures(GL_TEXTURE_2D, 1, &framebuffer_color);
    glTextureStorage2D(framebuffer_color, 1, GL_RGBA32F, initial_width, initial_height);

    // Initialize depth output texture with GL_DEPTH_COMPONENT32F format
    glCreateTextures(GL_TEXTURE_2D, 1, &framebuffer_depth);
    glTextureStorage2D(framebuffer_depth, 1, GL_DEPTH_COMPONENT32F, initial_width, initial_height);
    
    // Set output 0 to GL_COLOR_ATTACHMENT0 (glNamedFramebufferDrawBuffers)
    const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
    glNamedFramebufferDrawBuffers(framebuffer, 1, draw_buffers);

    // Associate color and depth 'attachments'(GL_COLOR_ATTACHMENT0,GL_DEPTH_ATTACHMENT)
    // with (framebuffer_)color and (framebuffer_)depth 'textures' (glNamedFramebufferTexture)
    glNamedFramebufferTexture(framebuffer, GL_COLOR_ATTACHMENT0, framebuffer_color, 0);
    glNamedFramebufferTexture(framebuffer, GL_DEPTH_ATTACHMENT, framebuffer_depth, 0);

    // simulate 5 seconds of run to form flocks
    float length = 5;
    float step = 0.01f;
    for (float t = 0; t < length; t += step)
        flock.update(step, sub);
}

Application::~Application() {
    delete_shaders();

    glDeleteBuffers(1, &camera_buffer);
}

// ----------------------------------------------------------------------------
// Methods
// ----------------------------------------------------------------------------

void Application::delete_shaders() {
}

void Application::compile_shaders() {
    delete_shaders();
    main_program = create_program(lecture_shaders_path / "main.vert", lecture_shaders_path / "main.frag");
    surface_program = create_program(lecture_shaders_path / "main.vert", lecture_shaders_path / "surface.frag");
    fish_program = create_program(lecture_shaders_path / "fish.vert", lecture_shaders_path / "fish.frag");
    fog_program = create_program(lecture_shaders_path / "postprocess.vert", lecture_shaders_path / "postprocess.frag");
}

void Application::update(float delta) {
    float delta_s = delta / 1000;
    time += delta_s;
    sub.update(delta_s);
    flock.update(delta_s, sub);
}

void Application::render() {
    // --------------------------------------------------------------------------
    // Draw scene
    // --------------------------------------------------------------------------
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    // Bind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Configure fixed function pipeline
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_MULTISAMPLE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // update and bind camera
    sub.update_camera_ubo(&camera_ubo);
    glNamedBufferSubData(camera_buffer, 0, sizeof(CameraUBO), &camera_ubo);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, camera_buffer);
    
    // update and bind lights
    sub.update_lights_ubos();
    light_manager.bind_lights();

    // Draw objects
    glUseProgram(main_program);
    sub.draw();
    floor.draw();

    glUseProgram(surface_program);
    surface.draw();

    glUseProgram(fish_program);
    GLuint loc = glGetUniformLocation(fish_program, "t");
    glUniform1f(loc, time);
    flock.draw();

    // Draw fog
    // Bind back the default(0) framebuffer (glBindFramebuffer).
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Clear the color buffer.
    glClear(GL_COLOR_BUFFER_BIT);

    // Disable depth test - we do not need it.
    glDisable(GL_DEPTH_TEST);

    // Use post-process program.
    glUseProgram(fog_program);

    // Bind the output from previous program (framebuffer_color) as input texture.
    // to the post-process program
    glBindTextureUnit(0, framebuffer_color);
    glBindTextureUnit(1, framebuffer_depth);

    // Set depth
    loc = glGetUniformLocation(fog_program, "sub_depth");
    glUniform1f(loc, -sub.position.y);

    // Draw the full-screen triangle.
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

#pragma region UI

void vector_ui(const glm::vec3 vec) {
    std::string x = std::string("x: ") + std::to_string(vec.x);
    ImGui::Text(x.c_str());
    std::string y = std::string("y: ") + std::to_string(vec.y);
    ImGui::Text(y.c_str());
    std::string z = std::string("z: ") + std::to_string(vec.z);
    ImGui::Text(z.c_str());
}

void float_ui(const char* name, float val, int padd = 0) {
    std::string s = std::string(name);
    s.append(": ");
    if (s.length() < padd + 2)
        s.append(padd + 2 - s.length(), ' ');
    s.append(std::to_string(val));
    ImGui::Text(s.c_str());
}

void bool_ui(const char* name, bool val) {
    std::string s = std::string(name) + std::string(val ? ": true" : ": false");
    ImGui::Text(s.c_str());
}

void Application::reder_debug_ui() {
    ImGui::Begin("Debug", nullptr);

    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("Sub")) {
        bool_ui("SAS", sub.sas);

        ImGui::SetNextItemOpen(false, ImGuiCond_Once);
        if (ImGui::TreeNode("Lcl")) {

            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (ImGui::TreeNode("Velocity")) {
                vector_ui(sub.lcl_velocity);
                ImGui::TreePop();
            }

            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (ImGui::TreeNode("Input")) {
                vector_ui(sub.lcl_input);
                ImGui::TreePop();
            }

            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (ImGui::TreeNode("Drag")) {
                vector_ui(sub.lcl_drag);
                ImGui::TreePop();
            }

            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (ImGui::TreeNode("SAS")) {
                vector_ui(sub.lcl_sas);
                ImGui::TreePop();
            }

            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (ImGui::TreeNode("Acceleration")) {
                vector_ui(sub.lcl_acceleration);
                ImGui::TreePop();
            }

            float_ui("Vertical limiter", sub.vertical_limiter);

        ImGui::TreePop();
        }

        ImGui::SetNextItemOpen(false, ImGuiCond_Once);
        if (ImGui::TreeNode("Glb")) {

            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (ImGui::TreeNode("Position")) {
                vector_ui(sub.position);
                float_ui("r", sub.rotation);
                ImGui::TreePop();
            }

            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (ImGui::TreeNode("Velocity")) {
                float_ui("v", Helper::length(sub.velocity));
                vector_ui(sub.velocity);
                float_ui("r", sub.angular_velocity);
                ImGui::TreePop();
            }

            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (ImGui::TreeNode("Acceleration")) {
                float_ui("a", Helper::length(sub.acceleration));
                vector_ui(sub.acceleration);
                float_ui("r", sub.angular_acceleration);
                ImGui::TreePop();
            }

            ImGui::TreePop();
        }

        ImGui::SetNextItemOpen(false, ImGuiCond_Once);
        if (ImGui::TreeNode("Rot")) {
            float_ui("Input", sub.angular_input, 12);
            float_ui("Drag", sub.angular_drag, 12);
            float_ui("SAS", sub.angular_sas, 12);
            float_ui("Acceleration", sub.angular_acceleration, 12);
            float_ui("Velocity", sub.angular_velocity, 12);
            float_ui("Rotation", sub.rotation, 12);

            ImGui::TreePop();
        }

        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("Floor")) {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Grid Position")) {
            vector_ui(floor.grid_position);
            ImGui::TreePop();
        }

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Rounded Grid Position")) {
            vector_ui(floor.rounded_grid_position);
            ImGui::TreePop();
        }

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Floor Position")) {
            vector_ui(floor.position);
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }
    ImGui::End();
}

void Application::render_ui() { 
    const float unit = ImGui::GetFontSize();

    //ImGui::ShowDemoWindow();
    reder_debug_ui();
    
}
#pragma endregion

// ----------------------------------------------------------------------------
// Input Events
// ----------------------------------------------------------------------------

void Application::on_resize(int width, int height) {
    // Calls the default implementation to set the class variables.
    PV112Application::on_resize(width, height);
}

void Application::on_mouse_move(double x, double y) { camera.on_mouse_move(x, y); }
void Application::on_mouse_button(int button, int action, int mods) { camera.on_mouse_button(button, action, mods); }
void Application::on_key_pressed(int key, int scancode, int action, int mods) {
    // Calls default implementation that invokes compile_shaders when 'R key is hit.
    PV112Application::on_key_pressed(key, scancode, action, mods);

    sub.on_key_pressed(key, scancode, action, mods);
}