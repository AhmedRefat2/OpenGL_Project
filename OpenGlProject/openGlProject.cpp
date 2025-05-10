#include <glad/glad.h> // Loads OpenGL function pointers
#include <GLFW/glfw3.h> // Handles window creation, input, and OpenGL context
#include <iostream> // For outputting errors and messages
#include <glm/glm.hpp> // Core GLM types and functions
#include <glm/gtc/matrix_transform.hpp> // Matrix transformation functions like translate/rotate
#include <glm/gtc/type_ptr.hpp> // For converting glm types to OpenGL-friendly pointers

// Window size settings
const unsigned int SCR_WIDTH = 800; // Width of the window
const unsigned int SCR_HEIGHT = 600; // Height of the window

// Camera vectors
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f); // Initial camera position
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // Direction the camera is looking at
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // The upward direction relative to the camera

// Yaw/pitch for mouse movement
float yaw = -90.0f; // Initial yaw to look along -Z
float pitch = 0.0f; // Initial pitch
float lastX = SCR_WIDTH / 2.0f; // Last X position of mouse
float lastY = SCR_HEIGHT / 2.0f; // Last Y position of mouse
bool firstMouse = true; // Track if it's the first mouse input
bool leftMousePressed = false; // Flag to track mouse click status

// Time tracking
float deltaTime = 0.0f; // Time between frames
float lastFrame = 0.0f; // Time of last frame

// Transformation flags
bool applyTranslation = false; // Enable/disable translation
bool applyRotation = false; // Enable/disable rotation
bool applyScaling = false; // Enable/disable scaling
bool applyShearing = false; // Enable/disable shearing
bool applyReflection = false; // Enable/disable reflection
bool keyStates[6] = { false }; // Track pressed state to avoid multiple toggles
bool toggleStates[6] = { false }; // Track on/off states for each transformation

// Vertex Shader source code
const char* vertexShaderSource = R"(
#version 330 core // Use GLSL version 3.30
layout (location = 0) in vec3 aPos; // Input vertex position
layout (location = 1) in vec3 aColor; // Input vertex color

out vec3 ourColor; // Pass color to fragment shader

uniform mat4 model; // Model matrix
uniform mat4 view; // View (camera) matrix
uniform mat4 projection; // Projection matrix

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f); // Calculate transformed position
    ourColor = aColor; // Forward vertex color to fragment shader
})";

// Fragment Shader source code
const char* fragmentShaderSource = R"(
#version 330 core // Use GLSL version 3.30
in vec3 ourColor; // Color from vertex shader
out vec4 FragColor; // Final color output

void main()
{
    FragColor = vec4(ourColor, 1.0f); // Set the pixel color
})";

// Handles all input processing
void processInput(GLFWwindow* window)
{
    float cameraSpeed = 2.5f * deltaTime; // Adjust camera speed per frame

    // Move forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    // Move left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    // Move right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    // Map transformation keys
    int keys[5] = { GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3, GLFW_KEY_4, GLFW_KEY_5 };
    bool* flags[5] = { &applyTranslation, &applyRotation, &applyScaling, &applyShearing, &applyReflection };

    for (int i = 0; i < 5; i++) {
        if (glfwGetKey(window, keys[i]) == GLFW_PRESS) {
            if (!keyStates[i]) { // Only toggle once per key press
                toggleStates[i] = !toggleStates[i]; // Toggle state
                *flags[i] = toggleStates[i]; // Update flag
                keyStates[i] = true; // Mark key as down
            }
        }
        else {
            keyStates[i] = false; // Reset when key is released
        }
    }

    // Reset all toggles with '0'
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
        for (int i = 0; i < 5; i++) {
            toggleStates[i] = false;
            *flags[i] = false;
        }
    }
}

// Resize viewport when window changes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height); // Set OpenGL viewport
}

// Handle mouse button events
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Lock cursor
            leftMousePressed = true; // Enable mouse movement control
        }
        else if (action == GLFW_RELEASE)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // Release cursor
            firstMouse = true; // Reset mouse tracking
            leftMousePressed = false; // Disable control
        }
    }
}

// Mouse movement callback
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (!leftMousePressed) return; // Do nothing if mouse isn't pressed

    if (firstMouse) {
        lastX = xpos; lastY = ypos; // Initialize position
        firstMouse = false;
    }

    // Calculate offset since last frame
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos; lastY = ypos;

    float sensitivity = 0.1f; // Mouse sensitivity
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset; // Update yaw
    pitch += yoffset; // Update pitch

    // Clamp pitch to prevent flipping
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Update camera front vector
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
    // add some changes
}

int main()
{
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL version 3.x
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use core profile

    // Create GLFW window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Cube", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate(); // Exit if window creation fails
        return -1;
    }

    glfwMakeContextCurrent(window); // Set current context
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Window resize callback
    glfwSetCursorPosCallback(window, mouse_callback); // Mouse movement callback
    glfwSetMouseButtonCallback(window, mouse_button_callback); // Mouse click callback

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1; // Exit if GLAD fails
    }

    // Define cube vertices (position + color)
    float vertices[] = {
        // back face
        -0.5f,-0.5f,-0.5f, 1,0,0, 0.5f,-0.5f,-0.5f, 0,1,0, 0.5f,0.5f,-0.5f, 0,0,1,
         0.5f,0.5f,-0.5f, 0,0,1, -0.5f,0.5f,-0.5f, 1,1,0, -0.5f,-0.5f,-0.5f, 1,1,0,
         // front face
         -0.5f,-0.5f,0.5f, 1,0,0, 0.5f,-0.5f,0.5f, 0,1,0, 0.5f,0.5f,0.5f, 0,0,1,
          0.5f,0.5f,0.5f, 0,0,1, -0.5f,0.5f,0.5f, 1,1,0, -0.5f,-0.5f,0.5f, 1,1,0,
          // left face
          -0.5f,0.5f,0.5f, 1,0,0, -0.5f,0.5f,-0.5f, 0,1,0, -0.5f,-0.5f,-0.5f, 0,0,1,
          -0.5f,-0.5f,-0.5f, 0,0,1, -0.5f,-0.5f,0.5f, 1,1,0, -0.5f,0.5f,0.5f, 1,1,0,
          // right face
           0.5f,0.5f,0.5f, 1,0,0, 0.5f,0.5f,-0.5f, 0,1,0, 0.5f,-0.5f,-0.5f, 0,0,1,
           0.5f,-0.5f,-0.5f, 0,0,1, 0.5f,-0.5f,0.5f, 1,1,0, 0.5f,0.5f,0.5f, 1,1,0,
           // bottom face
           -0.5f,-0.5f,-0.5f, 1,0,0, 0.5f,-0.5f,-0.5f, 0,1,0, 0.5f,-0.5f,0.5f, 0,0,1,
            0.5f,-0.5f,0.5f, 0,0,1, -0.5f,-0.5f,0.5f, 1,1,0, -0.5f,-0.5f,-0.5f, 1,1,0
    };

    // Create and bind VAO/VBO
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO); // Create Vertex Array
    glGenBuffers(1, &VBO); // Create Vertex Buffer
    glBindVertexArray(VAO); // Bind VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Bind buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Upload vertex data

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // Set layout
    glEnableVertexAttribArray(0); // Enable position

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // Set layout
    glEnableVertexAttribArray(1); // Enable color

    // Compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Link shaders into a program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime(); // Get current time
        deltaTime = currentFrame - lastFrame; // Time between frames
        lastFrame = currentFrame;

        processInput(window); // Handle input

        // Clear screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Set background color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear buffers

        glUseProgram(shaderProgram); // Use the shader

        // Create transformation matrices
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 model = glm::mat4(1.0f); // Identity matrix

        // Apply transformations based on toggles
        if (applyTranslation)
            model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
        if (applyRotation)
            model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
        if (applyScaling)
            model = glm::scale(model, glm::vec3(sin(glfwGetTime()) + 1.0f));
        if (applyShearing)
        {
            glm::mat4 shear = glm::mat4(1.0f);
            shear[1][0] = 0.5f * sin(glfwGetTime()); // Shear on X axis
            model *= shear;
        }
        if (applyReflection)
        {
            glm::mat4 reflect = glm::scale(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, 1.0f)); // Reflect X axis
            model *= reflect;
        }

        // Pass matrices to shader
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO); // Bind VAO
        glDrawArrays(GL_TRIANGLES, 0, 36); // Draw cube

        glfwSwapBuffers(window); // Swap front and back buffers
        glfwPollEvents(); // Handle window/input events
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate(); // Close application

    return 0;
}
