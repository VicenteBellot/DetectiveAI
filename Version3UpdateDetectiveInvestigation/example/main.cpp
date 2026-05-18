#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"   
#include "imgui/imgui_impl_opengl3.h" 


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Information.hpp"
#include "BasePawn.hpp"
#include "Suspect.hpp"
#include "Detective.hpp"
#include "GameManager.hpp"
#include "StoryGenerator.hpp"
#include "Jobsystem.hpp"

#include <stdio.h>
#include <chrono>

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Thorne: Detective AI", NULL, NULL);
    glfwSetWindowSizeLimits(window, 600, 400, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwMakeContextCurrent(window);  
    glewExperimental = GL_TRUE;
    glewInit();

    static C_GameManager* game = new C_GameManager();
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    game->LoadFonts();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    game->m_LastWindowW = 1280;
    game->m_LastWindowH = 720;

    glfwSetWindowSizeCallback(window, [](GLFWwindow* w, int width, int height) {
        if (game) game->CheckAndHandleResize(width,height);
        });

    auto start = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();

	game->InitializeGame(); //
    while (!glfwWindowShouldClose(window) && !game->GeneratedVoice) {

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count(); //DeltaTime
        currentTime = newTime;

        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 2. The "Functional" Game Logic & UI
        game->MainLoop(frameTime);
        

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    printf("------------------------\nINGAME TABLE\n");
    for (int i = 0; i < game->quantitySustec; i++) {
        printf("ID: %d ", i + 101);
        game->suspects[i]->guilty ? printf("Was Guilty\n") : printf("Was NOT Guilty\n");
    }
    printf("------------------------\nEND INGAME TABLE\n\n");

    printf("------------------------\nQUESTION TABLE\n");
    for (int i = 0; i < game->quantitySustec; i++) {
        printf("Questions asked to ID: %d \n", i + 101);
        auto p = game->Thorne->memory[i+101].timeline.begin();
        while (p != game->Thorne->memory[i+101].timeline.end()) {
            printf("Question: %d\n", p->first);
            p++;
        }
        printf("\n");
    }
    printf("------------------------\nEND QUESTION TABLE\n\n");

    //JB.WaitAll();


    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return 0;
}