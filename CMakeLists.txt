set(course "PV112")
set(lecture "project")

# Processes the parameters.
set(options SOLUTION)
set(oneValueArgs "")
set(multiValueArgs "")
cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

message("----------------------------------------------------------------------")
message("Setting up '${lecture}' in '${course}' course (SOLUTION: ${ARG_SOLUTION}).")
message("----------------------------------------------------------------------")

# Resolves the application name.
visitlab_to_course_dir_name(course ${course})
# Resolves the course absolute path.
visitlab_get_course_path(${course} course_path)
# Resolves the lecture absolute path.
set(lecture_path "${course_path}/${lecture}")


set(executable_target "${course}_${lecture}")
add_executable(${executable_target} 
    "main.cpp" 
    "application.hpp" 
    "application.cpp" 
    "scripts/sub.hpp" 
    "scripts/sub.cpp" 
    "scripts/flock.hpp"
    "scripts/flock.cpp" 
    "scripts/helper.hpp" 
    "scripts/helper.cpp" 
    "scripts/uniform_structs.hpp" 
     
    "scripts/loader.hpp" 
    "scripts/loader.cpp" 
    "scripts/floor.hpp"
    "scripts/floor.cpp"  "scripts/light_manager.hpp" "scripts/light_manager.cpp")


message(STATUS "Generating ${executable_target} target.")
    
# Sets the build properties.
# CXX_STANDARD - the C++ standard whose features are requested.
# CXX_EXTENSIONS - no compiler specific extensions are requested.
set_target_properties(${executable_target}
    PROPERTIES CXX_STANDARD 20
                CXX_EXTENSIONS OFF
)

# Solves the lecture dependencies.
visitlab_resolve_dependencies(${executable_target} ${course_path})
