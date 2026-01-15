#include "Information.hpp"
#include "BasePawn.hpp"
#include "Suspect.hpp"
#include "Detective.hpp"
#include "GameManager.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"   // If using GLFW
#include "imgui/imgui_impl_opengl3.h" // If using OpenGL 3

#include <GLFW/glfw3.h>

#include <stdio.h>

int main() {
    if (!glfwInit()) return 1;
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Thorne: Detective AI", NULL, NULL);
    glfwMakeContextCurrent(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");


	C_GameManager game;
	game.InitializeGame(); //
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        // 1. Input/New Frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 2. The "Functional" Game Logic & UI
        game.DrawInterface();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return 0;
}