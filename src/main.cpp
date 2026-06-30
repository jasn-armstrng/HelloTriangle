// On Learning Vulkan
// -------------------
// - Vulkan is not meant to be easy.
// - Vulkan is targeted at programmers who are enthusiastic about high performance computer graphics and are willing to put some work in.
// - Leave all preconceived notions behind on this journey.The most important thing you can bring within yourself is a willingness to learn.
// - When first approaching learning you should try to avoid thing about any specific problem.Instead, focus on the material being presented.Once you have finished the material, then you can look back and see if you understand how to solve that problem you wanted to solve.
// - Graphics programming can be a daunting task when starting out.The rendering pipeline involves a large number of steps, each dealing with a variety of math operations.It has stages that run actual programs to compute results for the next.Mastering this pipeline, being able to use it as a tool to achieve a visual effect, is the essence of being a graphics programmer.
// - Vulkan is designed from scratch for modern graphics architectures.It reduces the driver overhead by allowing programmers to clearly specify their intent.Note the weight and responsibility the word intent carries.
// -------------------

// Vulkan Tutorial
// ---------------
// Reference: https://docs.vulkan.org/tutorial/latest/00_Introduction.html
// Drawing a triangle
// [X] Base code
// [ ] Create the first Vulkan object ... Creating an instance

// Tools
// 1. To show function/method details, parameters tooltip - place the caret right after the first parenthesis `glClearColor(|  <-- caret` then Ctrl+Shift+Space

#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp> // Provides the functions, structures and enumerations
#else
import vulkan_hpp;
#endif
#include <GLFW/glfw3.h> // Provides a simple API for creating windows, contexts and surfaces, receiving input and events

#include <cstdlib>   // Provides functions for general-purpose tasks like memory management, random number generation, type conversions, and process control.
#include <iostream>  // Used for reporting and propogating errors
#include <stdexcept> // Provides pre-defined exception classes to report common errors. Automatically includes the <exception> header as it contains the base classes for these exception classes.

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

// Private class members
private:
    GLFWwindow* window; // Struct: glfw window object

    void initWindow() {
        // GLFW integration
        // Initialize glfw and create window
        glfwInit(); // Initializes the glfw library

        // Note the patterb in the arguments for the two functions below, (option, flag)
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Tells glfw to not create an OpenGL context
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Tells glfw to disable resizing window 

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr); // Window width, height, which monitor to open window on, OpenGL parameter
    }

    // Vulkan objects iniitialization functions
    void initVulkan() {

    }

    // Our event loop, where we render frames
    void mainLoop() {
        // Loop till window closes
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents(); // Waits until events are queued and processes them
        }
    }

    void cleanup() {
        // Deallocation of resources
        glfwDestroyWindow(window);
        glfwTerminate(); // Terminates the glfw library we initialized in initWindow()
    }
}; // HelloTriangleApplication

int main()
{
    try
    {
        HelloTriangleApplication app;
        app.run();
    }
    catch (const std::exception& e) // If any fatal error occurs during execution, throw error with desciptive mesaage `e.what()`
    {
        std::cerr << e.what() << std::endl; // std::cerr comes from <iostream>
        return EXIT_FAILURE; // From <cstdlib> 
    }

    return EXIT_SUCCESS; // From <cstdlib> 
}



// Vulkan objects are either created directly with functions like vkCreateXXX, or allocated through another object with functions like vkAllocateXXX.
//  - After making sure that an object is no longer used anywhere, you need to destroy it with the counterparts vkDestroyXXX and vkFreeXXX.
//  - The parameters for these functions generally vary for different types of objects, but there is one parameter that they all share: pAllocator.
//    This is an optional parameter that allows you to specify callbacks for a custom memory allocator. 


// Misc:
// ----------------------------------
// 1. For the below, tell Claude that you will write the notes in the main.cpp file.
// One thing I noticed in your main.cpp before you go — you haven't written your chapter 01 notes yet. notes/01_instance_creation.md doesn't exist.
// Before you read the validation layers material, write those notes.What you put in your code comments is a start but the notes file is where you articulate understanding in prose, not just label what things do.
//
// 2. Get really good at Visual Studio. Make it your ultimate development environment.