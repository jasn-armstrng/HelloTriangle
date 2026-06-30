cmake_minimum_required(VERSION 3.29)

project(HelloTriangle)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

option(ENABLE_CPP20_MODULE "Enable C++ 20 module support for Vulkan" OFF)

if(ENABLE_CPP20_MODULE)
  set(CMAKE_CXX_SCAN_FOR_MODULES ON)
endif()

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(Vulkan 1.4.335 REQUIRED)
find_package(tinyobjloader REQUIRED)
find_package(tinygltf REQUIRED)
find_package(KTX REQUIRED)

find_package(stb REQUIRED)
set(STB_INCLUDEDIR ${stb_INCLUDE_DIRS})

# Vulkan C++ module setup
if(ENABLE_CPP20_MODULE)
  add_library(VulkanCppModule)
  add_library(Vulkan::cppm ALIAS VulkanCppModule)

  target_compile_definitions(VulkanCppModule
    PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1 VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
  )
  target_include_directories(VulkanCppModule PUBLIC "${Vulkan_INCLUDE_DIR}")
  target_link_libraries(VulkanCppModule PUBLIC Vulkan::Vulkan)
  set_target_properties(VulkanCppModule PROPERTIES CXX_STANDARD 20)

  if(MSVC)
    target_compile_options(VulkanCppModule PRIVATE
      /std:c++latest
      /permissive-
      /Zc:__cplusplus
      /EHsc
      /Zc:preprocessor
      /translateInclude
    )
  endif()

  target_sources(VulkanCppModule
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS "${Vulkan_INCLUDE_DIR}"
    FILES "${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm"
  )
  target_sources(VulkanCppModule PRIVATE "${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm")
else()
  add_library(VulkanCppModule INTERFACE)
  add_library(Vulkan::cppm ALIAS VulkanCppModule)
  target_link_libraries(VulkanCppModule INTERFACE Vulkan::Vulkan)
  target_compile_definitions(VulkanCppModule
    INTERFACE VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1 VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
  )
endif()

add_executable(glslang::validator IMPORTED)
find_program(GLSLANG_VALIDATOR "glslangValidator" HINTS $ENV{VULKAN_SDK}/bin REQUIRED)
set_property(TARGET glslang::validator PROPERTY IMPORTED_LOCATION "${GLSLANG_VALIDATOR}")

find_program(SLANGC_EXECUTABLE slangc HINTS $ENV{VULKAN_SDK}/bin REQUIRED)

add_executable(HelloTriangle src/main.cpp)

set_target_properties(HelloTriangle PROPERTIES CXX_STANDARD 20)

target_link_libraries(HelloTriangle PRIVATE Vulkan::cppm glfw glm::glm)

target_include_directories(HelloTriangle PRIVATE ${STB_INCLUDEDIR})

target_compile_definitions(HelloTriangle PRIVATE VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS)

if(ENABLE_CPP20_MODULE)
  target_compile_definitions(HelloTriangle PRIVATE USE_CPP20_MODULES=1)
endif()

if(WIN32 AND ${CMAKE_GENERATOR} MATCHES "Visual Studio.*")
  set_target_properties(HelloTriangle PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/HelloTriangle"
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/HelloTriangle"
  )
endif()
