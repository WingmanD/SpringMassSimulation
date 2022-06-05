#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <filesystem>
#include <cstdlib>
#include <future>

#include "Shader.h"
#include "Globals.h"
#include "Renderer.h"
#include "Util.h"
#include "forces/GravityForce.h"
#include "forces/RigidBodyCollisionForce.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"


int width = 1280;
int height = 720;

bool bWireframeMode = false;
bool bShowParticles = true;
bool bShowBoundingBoxes = false;
bool bPaused = false;

Shader* defaultShader = nullptr;
Shader* particleShader = nullptr;

float particleMass = 1.0f;
float springConstant = 200.0f;
float internalSpringConstant = 300.f;
float damping = 5.0f;
float internalPressureForceConstant = 0.01f;

double deltaTime = 0.0;
double lastFrame = 0.0;

bool cameraControls = true;
bool cursorCaptured = true;

Renderer* renderer;
Scene* scene;
Camera* activeCamera;

int currentObjectIndex = 0;

double lastX = static_cast<double>(width) / 2.0;
double lastY = static_cast<double>(height) / 2.0;
bool firstMouse = true;


void framebuffer_size_callback(GLFWwindow* window, int Width, int Height) {
    width = Width;
    height = Height;

    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_C && action == GLFW_PRESS) cameraControls = !cameraControls;
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        bWireframeMode = !bWireframeMode;
        glPolygonMode(GL_FRONT_AND_BACK, bWireframeMode ? GL_LINE : GL_FILL);
    }
    if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        scene->loadSoft(Util::chooseOBJ());
        lastFrame = glfwGetTime();
    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS) bPaused = !bPaused;
    if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
        cursorCaptured = !cursorCaptured;
        glfwSetInputMode(window, GLFW_CURSOR, cursorCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
}

void processInput(GLFWwindow* window) {

    auto speed = static_cast<float>(2.5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) speed *= 5.0f;

    if (glfwGetKey(window, GLFW_KEY_W)) activeCamera->move(activeCamera->getFront() * speed);
    if (glfwGetKey(window, GLFW_KEY_S)) activeCamera->move(-activeCamera->getFront() * speed);
    if (glfwGetKey(window, GLFW_KEY_A)) activeCamera->move(-activeCamera->getRight() * speed);
    if (glfwGetKey(window, GLFW_KEY_D)) activeCamera->move(activeCamera->getRight() * speed);
    if (glfwGetKey(window, GLFW_KEY_Q)) activeCamera->move(activeCamera->getViewUp() * speed);
    if (glfwGetKey(window, GLFW_KEY_E)) activeCamera->move(-activeCamera->getViewUp() * speed);
}

void mouse_callback(GLFWwindow* window, double x, double y) {
    if (firstMouse) {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }

    

    double xoffset = x - lastX;
    double yoffset = lastY - y;

    lastX = x;
    lastY = y;

if (!cursorCaptured) return;
    
    constexpr double sensitivity = 0.005;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    //todo when cursor is not capured, camera can flip after we capture it
    activeCamera->rotate(glm::vec3(0, yoffset, xoffset));
}

int main(int argc, char* argv[]) {
    GLFWwindow* window;

    glfwInit();
    gladLoadGL();

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

    window = glfwCreateWindow(width, height, "Soft Body Simulation", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to Create OpenGL Context" << std::endl;
        return 1;
    }
    glfwMakeContextCurrent(window);


    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return 1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, cursorCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    const ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glClearColor(0.15f, 0.1f, 0.1f, 1);

    glPolygonMode(GL_FRONT_AND_BACK, bWireframeMode ? GL_LINE : GL_FILL);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_POINT_SMOOTH);

    glfwSwapInterval(0);

    std::filesystem::path p = R"(..\Debug\resources\plane\plane.obj)";
    std::string path = std::filesystem::absolute(p).string();

    defaultShader = Util::loadShader(std::string(argv[0]), "flatShading");
    particleShader = Util::loadShader(std::string(argv[0]), "particleShader");

    const auto camera = new Camera(glm::vec3(3, 4, 1), glm::vec3(0, 0, 0));
    activeCamera = camera;

    scene = new Scene(activeCamera);
    scene->load(path, defaultShader, false);

    p = R"(..\Debug\resources\cube\cube_up.obj)";
    //path = std::filesystem::absolute(p).string();
    //const auto colliderCube = scene->load(path, defaultShader, false);
    //colliderCube->material->setDiffuse({0.5f, 0.0f, 0.0f});

    scene->objects[0]->bHasCollision = false;

    scene->addLight(new DirectionalLight({-1, -1, -1}, {1, 1, 1}, 1));

    const auto gravityForce = new GravityForce(glm::vec3(0, -9.8, 0));
    scene->addForce(gravityForce);
    scene->addForce(new RigidBodyCollisionForce({0, 0, 0}, {0, 1, 0}));

    renderer = new Renderer(scene, camera);

    constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground;

    while (glfwWindowShouldClose(window) == false) {
        const double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!bPaused) scene->tick(deltaTime);
        renderer->render();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Settings", nullptr, flags);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0 / static_cast<double>(ImGui::GetIO().Framerate),
                    static_cast<double>(ImGui::GetIO().Framerate));
        ImGui::Text("CTRL to release cursor");
        ImGui::Text("O to import .obj mesh");
        ImGui::SliderFloat("Spring constant", &springConstant, 0.0f, 1000.0f);
        ImGui::SliderFloat("Internal spring constant", &internalSpringConstant, 0.0f, 1000.0f);
        ImGui::SliderFloat("Damping", &damping, 0.0f, 10.0f);
        ImGui::SliderFloat("Internal pressure force constant", &internalPressureForceConstant, 0.0f, 1.0f);

        ImGui::SliderFloat3("Gravity", &gravityForce->gforce[0], -10.0f, 10.0f);

        if (ImGui::Button("Toggle Wireframe")) {
            bWireframeMode = !bWireframeMode;
            glPolygonMode(GL_FRONT_AND_BACK, bWireframeMode ? GL_LINE : GL_FILL);
        }

        if (ImGui::Button("Toggle Show Particles"))
            bShowParticles = !bShowParticles;

        if (ImGui::Button("Play/Pause"))
            bPaused = !bPaused;

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwSwapBuffers(window);
        glfwPollEvents();
        processInput(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    delete renderer;

    glfwTerminate();

    return EXIT_SUCCESS;
}
