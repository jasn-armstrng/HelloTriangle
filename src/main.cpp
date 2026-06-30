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
// [X] Create the first Vulkan object ... Creating an instance

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
#include <algorithm>

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

// Private class/data members
private:
    GLFWwindow* window; // Struct: glfw window object
    vk::raii::Context context; // Create a new instance of the Context class.
    vk::raii::Instance instance = nullptr;

    // 1. Setup window
    void initWindow() {
        // GLFW INTEGRATION
        // ----------------
        // Initialize glfw and create window
        glfwInit(); // Initializes the glfw library

        // Note the patterb in the arguments for the two functions below, (option, flag)
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Tells glfw to not create an OpenGL context
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Tells glfw to disable resizing window 

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr); // Window width, height, which monitor to open window on, OpenGL parameter
    }

    // 2. Setup instance
    // - The instance ist the connection between the application and the Vulkan library. 
    // - Creating it involves specifying some details about your application to the Vulkan driver ...
    void createInstance() {
        // APPLICATIONINFO STRUCT
        // ---------------------
        // ... filling in the struct with some information about our application. This struct is called vk::ApplicationInfo. More on that struct here: https://docs.vulkan.org/refpages/latest/refpages/source/VkApplicationInfo.html
        // Filling this is optional but it may provide some useful information to the Vulkan driver to optimize our specific application.
        //
        // *Note: A lot of information in Vulkan is passed thru structs instead of function parameters
        constexpr vk::ApplicationInfo appinfo{
            .pApplicationName = "Hello Triangle",
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "No Engine",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = vk::ApiVersion14 // Our baseline version for the functionality we need.
        };

        // DEBUG: Retrieve a list of supported extensions before creating an instance
        // --------------------------------------------------------------------------
        auto extensions = context.enumerateInstanceExtensionProperties();
        std::cout << "available extensions:\n"; // See console output
        // Each vk::ExtensionProperties struct contains the name and version of an extension. Listing them,
        for (const auto& extension : extensions) {
            std::cout << '\t' << extension.extensionName << '\n';
        }
        /*
        available extensions :
            VK_KHR_surface
            VK_KHR_win32_surface
            VK_KHR_external_memory_capabilities
            VK_KHR_external_semaphore_capabilities
            VK_KHR_external_fence_capabilities
            VK_KHR_get_physical_device_properties2
            VK_KHR_get_surface_capabilities2
            VK_KHR_device_group_creation
            VK_EXT_swapchain_colorspace
            VK_EXT_debug_utils
            VK_EXT_debug_report
            VK_KHR_portability_enumeration
            VK_LUNARG_direct_driver_loading
         */

        // GET EXTENSION FOR VULKAN TO INTERFACE WITH GLFW
        // -----------------------------------------------
        // As we know Vulkan is a platform - agnostic API, which means that you need an extension to interface with the window system.
        // GLFW has a handy built - in function that returns the extension(s) it needs to do that which we can pass to the `createInfo` struct that follows

        // Get the required instance extensions from GLFW
        uint32_t glfwExtensionCount = 0;
        auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount); // Returns an array of names - the Vulkan instance extensions required by GLFW for creating Vulkan surfaces for GLFW windows.

        // Get the Vulkan supported instance extensions
        auto extensionProperties = context.enumerateInstanceExtensionProperties(); // Returns a vector of supported instance extensions in Vulkan. See more at https://registry.khronos.org/vulkan/specs/latest/man/html/vkEnumerateInstanceExtensionProperties.html
        
        // Check if the required GLFW extensions are supported by the Vulkan implementation.
        // for each required extension R in glfwExtensions:
        //    if none of the available extensions in extensionProperties match R :
        //        throw ("R is missing")
        for (uint32_t i = 0; i < glfwExtensionCount; ++i) {
            if (std::ranges::none_of( // A C++20 constrained algorithm that checks if no elements in a given range satisfy a specific condition. It returns true if the condition (predicate) evaluates to false for every single element, or if the range is completely empty. Syntax: none_of(range, pred)
                extensionProperties,
                [glfwExtension = glfwExtensions[i]](auto const& extensionProperty) {
                    return strcmp(extensionProperty.extensionName, glfwExtension) == 0;
                })) // Boolean condition for if ends here
            {
                throw std::runtime_error("Required GLFW extension not supported: " + std::string(glfwExtensions[i]));
            }
        }

        // CREATEINFO STRUCT
        // --------------------------------------------------------------------------------------------------------------------
        // This struct is non-optional. It tells the Vulkan driver which global extensions and validation layers we want to use. 
        // Global here means that they apply to the entire program and not a specific device
        vk::InstanceCreateInfo createInfo{
            .pApplicationInfo = &appinfo, // The appinfo struct we completed just above this
            .enabledExtensionCount = glfwExtensionCount,
            .ppEnabledExtensionNames = glfwExtensions,
            // TODO: Add validation layers
        };

        // CREATE THE VK::RAII::INSTANCE
        // -----------------------------
        // Check if instance was successful
        try {
            instance = vk::raii::Instance(context, createInfo); // Returns the pointer to the raii constructed object.
            std::cout << "Instance created successfully\n";
        }
        catch (vk::SystemError const& err) {
            std::cerr << "Failed to create instance: " << err.what() << std::endl;
        }
        // Instance(Context const& context, VkInstance instance, Optional<AllocationCallbacks const> allocator = nullptr) // Instance constructor
        // 
        // Note (Above): The general pattern that object creation function parameters in Vulkan follow is:
        //  - Pointer to the device, instance, or context on which the constructor depends
        //  - Pointer to struct with creation info
        //  - Pointer to custom allocator callbacks, *always ignored in this tutorial as it is optional if you’re using the default.
        //  - Returns the pointer to the raii constructed object.
    }

    // Vulkan objects iniitialization functions
    void initVulkan() {
        createInstance();
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