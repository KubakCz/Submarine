// ################################################################################
// Common Framework for Computer Graphics Courses at FI MUNI.
//
// Copyright (c) 2021-2022 Visitlab (https://visitlab.fi.muni.cz)
// All rights reserved.
// ################################################################################

#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include "camera.h"
#include "pv112_application.hpp"
#include "scripts/uniform_structs.hpp"
#include "scripts/sub.hpp"
#include "scripts/flock.hpp"
#include "scripts/loader.hpp"
#include "scripts/floor.hpp"
#include "scripts/light_manager.hpp"

// Constants
const float clear_color[4] = {0.0, 0.0, 0.0, 1.0};
const float clear_depth[1] = {1.0};

class Application : public PV112Application
{
  float time = 0;

  // ----------------------------------------------------------------------------
  // Variables
  // ----------------------------------------------------------------------------
  const float max_visibility = 150;

  std::filesystem::path textures_path;
  std::filesystem::path objects_path;

  Loader loader;

  // Shader programs
  GLuint main_program;
  GLuint surface_program;
  GLuint fish_program;
  GLuint fog_program;

  // Frame buffers
  GLuint framebuffer;
  GLuint framebuffer_color;
  GLuint framebuffer_depth;

  // Camera
  GLuint camera_buffer = 0;
  CameraUBO camera_ubo;

  // Lights
  LightManager light_manager;

  // Objects
  Sub sub;
  Floor floor;
  Floor surface;
  Flock flock;

  // ----------------------------------------------------------------------------
  // Constructors & Destructors
  // ----------------------------------------------------------------------------
public:
  /**
   * Constructs a new @link Application with a custom width and height.
   *
   * @param 	initial_width 	The initial width of the window.
   * @param 	initial_height	The initial height of the window.
   * @param 	arguments	  	The command line arguments used to obtain the application directory.
   */
  Application(int initial_width, int initial_height, std::vector<std::string> arguments = {});

  /** Destroys the {@link Application} and releases the allocated resources. */
  ~Application() override;

  // ----------------------------------------------------------------------------
  // Methods
  // ----------------------------------------------------------------------------

  /** @copydoc PV112Application::compile_shaders */
  void compile_shaders() override;

  /** @copydoc PV112Application::delete_shaders */
  void delete_shaders() override;

  /** @copydoc PV112Application::update */
  void update(float delta) override;

  /** @copydoc PV112Application::render */
  void render() override;

  /** @copydoc PV112Application::render_ui */
  void render_ui() override;

  void reder_debug_ui();

  // ----------------------------------------------------------------------------
  // Input Events
  // ----------------------------------------------------------------------------

  /** @copydoc PV112Application::on_resize */
  void on_resize(int width, int height) override;

  /** @copydoc PV112Application::on_mouse_move */
  void on_mouse_move(double x, double y) override;

  /** @copydoc PV112Application::on_mouse_button */
  void on_mouse_button(int button, int action, int mods) override;

  /** @copydoc PV112Application::on_key_pressed */
  void on_key_pressed(int key, int scancode, int action, int mods) override;
};
