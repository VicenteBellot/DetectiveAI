#include "GameManager.hpp"

#include "BasePawn.hpp"
#include "Suspect.hpp"

#include <fstream>
#include <sstream>

#include <filesystem>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "imgui/stb_image.h"
#include <glm/glm.hpp>


C_GameManager::~C_GameManager()
{
}

void C_GameManager::GenerateStoryFuture()
{
    if (GeneratingStory) return;
    storyFuture = JB.EnqueueTask([&]() -> std::string {
        printf("\nstarted");
        std::string returnval = Thorne->Gen->GenerateStory();
        printf("\nended");
        GeneratedStory = true;
        return returnval;
        });
    GeneratingStory = true;
}

void C_GameManager::GenerateAudioFuture()
{
    if (GeneratingVoice || !SavedStory)return;

    JB.Enqueue([this]() {  
        std::cerr << "Starting narration...\n";
        int result = system(
            "C:\\piper\\piper\\piper.exe "
            "--model C:\\piper\\models\\en_GB-alan-medium.onnx "
            "--output_file ./data/narration.wav "
            "< ./data/Story.txt"
        );
        if (result != 0) {
            std::cerr << "Piper failed, is it installed at C:\\piper?\n";
        } else {
            std::cerr << "Narration saved to ./data/narration.wav\n";
            system("start ./data/narration.wav");
        }
        GeneratedVoice = true;  // signal completion
    });
    GeneratingVoice = true;
}

void C_GameManager::SaveStory()
{
    if (!GeneratedStory) return;
    if (SavedStory) return;
    std::string story = storyFuture.get();
    FILE* OutputStory = fopen("./data/Story.txt", "w");
    if (OutputStory) {
        fputs(story.c_str(), OutputStory);
        fclose(OutputStory);
    }
    SavedStory = true;
}

void C_GameManager::MainLoop(float deltaTime)
{
    //---------------------------------------------
    ImGuiIO& io = ImGui::GetIO();
    const int FONT_SMALL = 0;
    const int FONT_MEDIUM = 1;
    const int FONT_LARGE = 2;
    const int FONT_XLARGE = 3;

    ImFont* activeFont;
    int displacement = Thorne->IQ > Thorne->MinDumbIQ ? 0 : 8;
    if (m_LastWindowW < 800 || m_LastWindowH < 800)  activeFont = Fonts[FONT_SMALL + displacement];
    else if (m_LastWindowW < 1280 || m_LastWindowH < 1200) activeFont = Fonts[FONT_MEDIUM + displacement];
    else if (m_LastWindowW < 1600 || m_LastWindowH < 1600) activeFont = Fonts[FONT_LARGE + displacement];
    else                               activeFont = Fonts[FONT_XLARGE + displacement];

    ImGui::PushFont(activeFont);

    if (Thorne->dead && !SavedStory) {
        ImGui::PopFont();

        displacement = Thorne->IQ > Thorne->MinDumbIQ ? 4 : 12;
        if (m_LastWindowW < 800 || m_LastWindowH < 800)  activeFont = Fonts[FONT_SMALL + displacement];
        else if (m_LastWindowW < 1280 || m_LastWindowH < 1200) activeFont = Fonts[FONT_MEDIUM + displacement];
        else if (m_LastWindowW < 1600 || m_LastWindowH < 1600) activeFont = Fonts[FONT_LARGE + displacement];
        else                               activeFont = Fonts[FONT_XLARGE + displacement];

        ImGui::PushFont(activeFont);

        DeathImage();

        ImGui::PopFont();
        GenerateStoryFuture();
        SaveStory();
        return;
	}
	else if (Thorne->FoundCulprit && !SavedStory) {
		ImGui::PopFont();
        GenerateStoryFuture();
		SaveStory();
		return;
	}
    if (SavedStory) {
        ImGui::PopFont();
        GenerateAudioFuture();
        return;
    }


    if (rounds >= maxroundsPerDay) {
        EndOfDayImage();
        BeerPos.clear();
    }
    else {
        DrawSpriteWindow();
    }
    if (Thorne->GramoPorLitro > MaxComaEtilico) {
        Thorne->dead = true;
        Thorne->Talk(11);
    }
    
    //---------------------------------------------
    DrawInterface(deltaTime);
    ImGui::PopFont();
}

void C_GameManager::CheckAndHandleResize(int newWidth, int newHeight) {
    

    if (m_LastWindowW == newWidth && m_LastWindowH == newHeight) return;

    float rx = (float)newWidth / m_LastWindowW;
    float ry = (float)newHeight / m_LastWindowH;

    for (auto& pos : BeerPos) {
        pos.x *= rx;
        pos.y *= ry;
    }

    m_LastWindowW = newWidth;
    m_LastWindowH = newHeight;
}

void C_GameManager::AddBeerPosition() {
    ImGuiIO& io = ImGui::GetIO();

    float tableTop = io.DisplaySize.y * 0.70f;
    float tableBottom = io.DisplaySize.y * 0.80f;;
    float tableLeft;
    float tableRight;

    int area = rand() % 3;
    T_Pos newPos;

    switch (area) {
    case 0:
        newPos.inlight = false;
        tableLeft = io.DisplaySize.x * 0.31f;
        tableRight = io.DisplaySize.x * 0.35f;
        break;
    case 1:
        newPos.inlight = false;
        tableLeft = io.DisplaySize.x * 0.62f;
        tableRight = io.DisplaySize.x * 0.64f;
        break;
    default:
        newPos.inlight = true;
        tableTop = io.DisplaySize.y * 0.80f;
        tableBottom = io.DisplaySize.y * 0.90f;
        tableLeft = io.DisplaySize.x * 0.35f;
        tableRight = io.DisplaySize.x * 0.50f;
        break;
    }

    newPos.x = tableLeft + (rand() % (int)(tableRight - tableLeft));
    newPos.y = tableTop + (rand() % (int)(tableBottom - tableTop));

    BeerPos.push_back(newPos);

    std::sort(BeerPos.begin(), BeerPos.end(), [](const T_Pos& a, const T_Pos& b) {
        return a.y < b.y;
        });
}

void C_GameManager::DrawSpriteWindow()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::Begin("Background", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);


    ImVec2 startPos = ImGui::GetCursorScreenPos();
    
    ImGui::SetCursorScreenPos(startPos);
    ImGui::Image((ImTextureID)(intptr_t)m_TextureIDs[0], io.DisplaySize, ImVec2(0, 1), ImVec2(1, 0));

    ImGui::SetCursorScreenPos(startPos);
    ImGui::Image((ImTextureID)(intptr_t)m_TextureIDs[1], io.DisplaySize, ImVec2(0, 1), ImVec2(1, 0));

    switch (Thorne->dementia)
    {
    case 0:
        ImGui::SetCursorScreenPos(startPos);
        ImGui::Image((ImTextureID)(intptr_t)m_TextureIDs[2], io.DisplaySize, ImVec2(0, 1), ImVec2(1, 0));
        break;
    case 1:
        ImGui::SetCursorScreenPos(startPos);
        ImGui::Image((ImTextureID)(intptr_t)m_TextureIDs[3], io.DisplaySize, ImVec2(0, 1), ImVec2(1, 0));
        break;
    case 2:
        ImGui::SetCursorScreenPos(startPos);
        ImGui::Image((ImTextureID)(intptr_t)m_TextureIDs[4], io.DisplaySize, ImVec2(0, 1), ImVec2(1, 0));
        break;
    default:
        ImGui::SetCursorScreenPos(startPos);
        ImGui::Image((ImTextureID)(intptr_t)m_TextureIDs[5], io.DisplaySize, ImVec2(0, 1), ImVec2(1, 0));
        break;
    }

    float designWidth = 1500.0f;
    float designHeight = 850.0f;

    float scaleX = io.DisplaySize.x / designWidth;
    float scaleY = io.DisplaySize.y / designHeight;

    float beerW = 50.0f * scaleX;
    float beerH = 100.0f * scaleY;

    for (int i = 0; i < Thorne->BeersDrank; i++) {
        if (i == BeerPos.size()) { //End of line no more pos, load new one
            AddBeerPosition();
        }
        ImGui::SetCursorScreenPos(ImVec2(
            BeerPos[i].x,
            BeerPos[i].y
        ));
        if (BeerPos[i].inlight) {
            ImGui::Image((ImTextureID)(intptr_t)m_TextureIDs[6], ImVec2(beerW , beerH), ImVec2(0, 1), ImVec2(1, 0));
        }
        else {
            ImGui::Image((ImTextureID)(intptr_t)m_TextureIDs[6], ImVec2(beerW, beerH), ImVec2(0, 1), ImVec2(1, 0), ImVec4(0.5f, 0.55f, 0.6f, 1.0f), ImVec4(0, 0, 0, 0));
        }
    }

    ImGui::End();
    ImGui::PopStyleVar(2);
}

void C_GameManager::EndOfDayCycle()
{
    Thorne->Talk(9);
    Thorne->Talk(10);
    Thorne->UpdateDementia();
    Thorne->UpdateBias();
}

void C_GameManager::EndOfDayImage()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::Begin("Background", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);


    ImVec2 startPos = ImGui::GetCursorScreenPos();

    ImVec4 dayColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);       // white tint
    ImVec4 nightColor = ImVec4(0.1f, 0.15f, 0.3f, 1.0f);       // dark blue

    float delta = ImGui::GetIO().DeltaTime * Speed;
    m_DayNightTimer += delta;

    float cycle = m_DayNightTimer / secsOfNight; 

    float t = 0.0f;
    if (cycle <= 1.0f)
    {
        t = cycle; 
    }
    else if (cycle <= 2.0f)
    {
        t = 2.0f - cycle; 

        if (!m_EndOfDayCycleFired)
        {
            EndOfDayCycle();
            m_EndOfDayCycleFired = true;
        }
    }
    else
    {
        rounds = 0;
        m_DayNightTimer = 0.0f;
        m_EndOfDayCycleFired = false;
        t = 0.0f;
    }

    t = glm::clamp(t, 0.0f, 1.0f);

    ImVec4 tint = ImVec4(
        dayColor.x + t * (nightColor.x - dayColor.x),
        dayColor.y + t * (nightColor.y - dayColor.y),
        dayColor.z + t * (nightColor.z - dayColor.z),
        1.0f
    );

    ImGui::SetCursorScreenPos(startPos);
    ImGui::Image((ImTextureID)(intptr_t)m_TextureIDs[7], io.DisplaySize, ImVec2(0, 1), ImVec2(1, 0),tint,ImVec4(0,0,0,0));

   
    ImGui::End();
    ImGui::PopStyleVar(2);
}

void C_GameManager::DeathImage()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::Begin("Background", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);


    ImVec2 startPos = ImGui::GetCursorScreenPos();
    ImGui::SetCursorScreenPos(startPos);
    ImGui::Image((ImTextureID)(intptr_t)m_TextureIDs[8], io.DisplaySize, ImVec2(0, 1), ImVec2(1, 0));

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    const char* text = "GAME OVER";
    ImVec2 textSize = ImGui::CalcTextSize(text);
    ImVec2 pos = ImVec2(
        (io.DisplaySize.x - textSize.x) * 0.5f,
        (io.DisplaySize.y - textSize.y) * 0.5f
    );
    drawList->AddText(pos, IM_COL32(255, 255, 255, 255), text);

    ImGui::End();
    ImGui::PopStyleVar(2);
}

void C_GameManager::LoadScenarioFromCSV(std::string filePath)
{
    std::ifstream file(filePath);
    std::string line;

    // Skip Header
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string topic, text;

        std::getline(ss, text, ',');
        std::getline(ss, topic, ',');

        if (!text.empty()) {
            ScenarioQuestion sq;
            sq.topic = topic;
            sq.text = text;
            currentScenario.timelineQuestions.push_back(sq);
        }
    }
}

void C_GameManager::LoadAlibisFromCSV(std::string filePath) {
    std::ifstream file(filePath);
    std::string line;
    std::getline(file, line); // Skip header

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string pID, hr, loc, arrivedpartner, stayedwith, vouch, sawleaving, leftwith, sawArrive, sawTwice;

        std::getline(ss, pID, ',');
        std::getline(ss, loc, ',');
        std::getline(ss, arrivedpartner, ',');
        std::getline(ss, stayedwith, ',');
        std::getline(ss, vouch, ',');
        std::getline(ss, sawleaving, ',');
        std::getline(ss, leftwith, ',');
        std::getline(ss, hr, ',');
        std::getline(ss, sawArrive, ',');
        std::getline(ss, sawTwice);

       
        Statement s;
        s.location = loc;
        s.arriveWithID = std::stoi(arrivedpartner);
        s.seenWithID = std::stoi(stayedwith);
        s.vouchID = std::stoi(vouch);
        s.seenIDLeave = std::stoi(sawleaving);
        s.leaveWithID = std::stoi(leftwith);
        s.duration = std::stoi(hr);
        s.sawArriveID = std::stoi(sawArrive);
        s.sawTwiceID = std::stoi(sawTwice);
        s.cat = Safe; // It's the "Truth"


        // Store in a map: currentScenario.alibiLibrary[PathID].push_back(s)
        currentScenario.alibiLibrary[std::stoi(pID) - 101].push_back(s);
    }
}

void C_GameManager::LoadRoomsFromCSV(std::string filePath)
{
    std::ifstream file(filePath);
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string hourStr;

        std::getline(ss, hourStr, ',');

        if (!hourStr.empty()) {
            currentScenario.worldLocations.push_back(hourStr.c_str());
        }
    }

}

void C_GameManager::LoadFonts()
{
    ImGuiIO& io = ImGui::GetIO();

    ImFont* fontSmall = io.Fonts->AddFontFromFileTTF("./data/Fonts/Normal.ttf", 13.0f);
    ImFont* fontMedium = io.Fonts->AddFontFromFileTTF("./data/Fonts/Normal.ttf", 18.0f);
    ImFont* fontLarge = io.Fonts->AddFontFromFileTTF("./data/Fonts/Normal.ttf", 24.0f);
    ImFont* fontMax = io.Fonts->AddFontFromFileTTF("./data/Fonts/Normal.ttf", 40.0f);

    Fonts.push_back(fontSmall);
    Fonts.push_back(fontMedium);
    Fonts.push_back(fontLarge);
    Fonts.push_back(fontMax);

    fontSmall = io.Fonts->AddFontFromFileTTF("./data/Fonts/Normal.ttf", 39.0f);
    fontMedium = io.Fonts->AddFontFromFileTTF("./data/Fonts/Normal.ttf", 54.0f);
    fontLarge = io.Fonts->AddFontFromFileTTF("./data/Fonts/Normal.ttf", 72.0f);
    fontMax = io.Fonts->AddFontFromFileTTF("./data/Fonts/Normal.ttf", 120.0f);

    Fonts.push_back(fontSmall);
    Fonts.push_back(fontMedium);
    Fonts.push_back(fontLarge);
    Fonts.push_back(fontMax);

    ImFont* MinefontSmall = io.Fonts->AddFontFromFileTTF("./data/Fonts/Mine.ttf", 10.0f);
    ImFont* MinefontMedium = io.Fonts->AddFontFromFileTTF("./data/Fonts/Mine.ttf", 13.0f);
    ImFont* MinefontLarge = io.Fonts->AddFontFromFileTTF("./data/Fonts/Mine.ttf", 16.0f);
    ImFont* MinefontMax = io.Fonts->AddFontFromFileTTF("./data/Fonts/Mine.ttf", 23.0f);

    Fonts.push_back(MinefontSmall);
    Fonts.push_back(MinefontMedium);
    Fonts.push_back(MinefontLarge);
    Fonts.push_back(MinefontMax);

    MinefontSmall = io.Fonts->AddFontFromFileTTF("./data/Fonts/Mine.ttf", 30.0f);
    MinefontMedium = io.Fonts->AddFontFromFileTTF("./data/Fonts/Mine.ttf", 39.0f);
    MinefontLarge = io.Fonts->AddFontFromFileTTF("./data/Fonts/Mine.ttf", 48.0f);
    MinefontMax = io.Fonts->AddFontFromFileTTF("./data/Fonts/Mine.ttf", 69.0f);

    Fonts.push_back(MinefontSmall);
    Fonts.push_back(MinefontMedium);
    Fonts.push_back(MinefontLarge);
    Fonts.push_back(MinefontMax);


}

void C_GameManager::LoadImages(std::string folderPath)
{
    if (this == nullptr) {
        std::cerr << "CRITICAL ERROR: LoadImages called on a NULL C_GameManager pointer!" << std::endl;
        return;
    }
    if (!std::filesystem::exists(folderPath) || !std::filesystem::is_directory(folderPath)) {
        std::cerr << "Path does not exist: " << folderPath << std::endl;
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        if (entry.path().extension() == ".png") {
            std::string path = entry.path().string();

            int width, height, nrChannels;
            stbi_set_flip_vertically_on_load(true);
            unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);

            if (data) {
                GLuint textureID;
                glGenTextures(1, &textureID);
                glBindTexture(GL_TEXTURE_2D, textureID);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                m_TextureIDs.push_back(textureID);

                stbi_image_free(data);
            }
        }
    }
}

void C_GameManager::DrawInterface(float DeltaTime)
{
    tick += DeltaTime * Speed;
    ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always, ImVec2(0, 0));
    ImGui::Begin("Locations", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
    for (auto i : currentScenario.worldLocations) {
        ImGui::Text(i.c_str());
    }
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(0, io.DisplaySize.y), ImGuiCond_Always, ImVec2(0, 1));
    ImGui::Begin("TickSpeed", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
    ImGui::SliderFloat("Tick Speed", &Speed, 0.1f, 20.0f);
    ImGui::End();

    //--------------------------
    ImGui::SetNextWindowPos(
        ImVec2(io.DisplaySize.x, io.DisplaySize.y),
        ImGuiCond_Always,
        ImVec2(1, 1)
    );
    ImGui::Begin("Alibis", nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse);
    static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;

    if (ImGui::BeginTable("AlibiTable", 10, flags))
    {
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Location");
        ImGui::TableSetupColumn("Arrive With");
        ImGui::TableSetupColumn("Seen With");
        ImGui::TableSetupColumn("Vouch ID");
        ImGui::TableSetupColumn("Saw Leave");
        ImGui::TableSetupColumn("Leave With");
        ImGui::TableSetupColumn("Duration");
        ImGui::TableSetupColumn("Saw Arrive");
        ImGui::TableSetupColumn("Saw Twice");
        ImGui::TableHeadersRow();

        for (auto const& [suspectID, alibiList] : currentScenario.alibiLibrary)
        {
            for (auto const& alibi : alibiList)
            {
                ImGui::TableNextRow();
                if (suspects.at(suspectID)->guilty)
                {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(255, 0, 0, 100)); // Transparent Red
                }
                ImGui::TableNextColumn();
                ImGui::Text("%d", suspectID+101);

                ImGui::TableNextColumn();
                ImGui::TextUnformatted(alibi.location.c_str());

                ImGui::TableNextColumn();
                ImGui::Text("%d", alibi.arriveWithID);

                ImGui::TableNextColumn();
                ImGui::Text("%d", alibi.seenWithID);

                ImGui::TableNextColumn();
                ImGui::Text("%d", alibi.vouchID);

                ImGui::TableNextColumn();
                ImGui::Text("%d", alibi.seenIDLeave);

                ImGui::TableNextColumn();
                ImGui::Text("%d", alibi.leaveWithID);

                ImGui::TableNextColumn();
                ImGui::Text("%d min", alibi.duration);

                ImGui::TableNextColumn();
                ImGui::Text("%d", alibi.sawArriveID);

                ImGui::TableNextColumn();
                ImGui::Text("%d", alibi.sawTwiceID);
            }
        }

        ImGui::EndTable();
    }

    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, 0),
        ImGuiCond_Always,
        ImVec2(0.5f, 0));

    ImGui::Begin("Inspector Thorne's Notes", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Origin Country: %s", Thorne->CountryName[Thorne->country].c_str());
    ImGui::Text("Detectives Cluster: V%d", Thorne->cluster);
    ImGui::Text("Current Style: %s", (Thorne->frustration > 0.6f ? "Aggressive" : "Professional"));
    ImGui::Separator();
    ImGui::Text("Racism: %.2f", Thorne->racism);
    ImGui::Text("Homophobia: %.2f", Thorne->homophobia);
    ImGui::Text("Classism: %.2f", Thorne->classism);
    ImGui::Text("Sexism: %.2f", Thorne->sexism);
    ImGui::Separator();
    ImGui::ProgressBar(Thorne->GramoPorLitro / MaxComaEtilico, ImVec2(-1, 0), "Alcohol Rate");
    ImGui::Text("%.2f g/L", Thorne->GramoPorLitro);

    ImGui::End();
    static float InterrogationWidth;
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x, 0), ImGuiCond_Always, ImVec2(1, 0));
    ImGui::Begin("Interrogation Room", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    if (activeSuspectIndex != -1) {
        std::shared_ptr target = suspects[Thorne->hatefullness[activeSuspectIndex].id];

        ImGui::Text("Subject: %s", target->name.c_str());
        ImGui::Text("Race: V%d", target->cluster);
        ImGui::Text("Country of Origin: %s", target->CountryName[target->country].c_str());
        if (target->gender) ImGui::Text("Gender: Male");
        else ImGui::Text("Gender: Female");

        if(target->sexualOrientation) ImGui::Text("Orientation: Heterosexual");
        else ImGui::Text("Orientation: Other");

        ImGui::Separator();

        // Show questions from the Spreadsheet
        ImGui::Text("Investigation Topics:");
        //Ask Questions
        if (tick > Cooldown && rounds < maxroundsPerDay) {
            tick = 0;
            Thorne->Talk(0);
            bool AskFor = false;
            std::shared_ptr<C_Suspect> nextTarget = nullptr;

            if (Thorne->LastSuspectedInterrogates != -2) {
                auto& memoryID = Thorne->memory[Thorne->LastSuspectedInterrogates];
                auto it = memoryID.timeline.find(SuspectInfo::KQuestionAsked(Thorne->LastQuestionAsked));

                if (it != memoryID.timeline.end()) {
                    int answertolastquestion = it->second;

                    // Check if we need to follow up (And it's NOT the one-way question)
                    if (memoryID.cat == Category::ToAsk && Thorne->LastQuestionAsked != SuspectInfo::KQuestionAsked::WatchLeave) {
                        auto& memoryIDsecond = Thorne->memory[answertolastquestion];
                        auto itsecond = memoryIDsecond.timeline.find(SuspectInfo::KQuestionAsked(Thorne->LastQuestionAsked));

                        if (itsecond == memoryIDsecond.timeline.end()) {
                            Thorne->Talk(1, answertolastquestion);
                            AskFor = true;
                            nextTarget = suspects[answertolastquestion];
                        }
                    }
                }
            }

            int maxQuestions = SuspectInfo::KQuestionAsked::TimeStayed + 1;
            int i = 0;
            if (!AskFor) {
                nextTarget = suspects[Thorne->hatefullness[activeSuspectIndex].id];

                do {
                    i++;
                    Thorne->LastQuestionAsked = rand() % currentScenario.timelineQuestions.size();
                } while (Thorne->QuestionAskedSuspect(nextTarget, Thorne->LastQuestionAsked) && i < maxQuestions);


                // Advance index for the NEXT exploration turn
                activeSuspectIndex = (activeSuspectIndex + 1);
                
            }

            if (nextTarget) {
                Category catParam = AskFor ? Category::ToAsk : Category::Safe;
                if (i < maxQuestions) {
                    printf("\n||\n||Question asked: %s\n||\n\n", currentScenario.timelineQuestions[Thorne->LastQuestionAsked].text.c_str());
                    Thorne->Gen->AddString("\n||\n||Question asked: " + currentScenario.timelineQuestions[Thorne->LastQuestionAsked].text+"\n||\n\n");
                    Thorne->Interrogate(nextTarget, Thorne->LastQuestionAsked, catParam);

                    Thorne->LastSuspectedInterrogates = nextTarget->ID;
                }
                else {
                    Thorne->Talk(7, nextTarget->ID);
                }
                Thorne->Talk(8);
                target = nextTarget; // Update the global target for rendering
            }
        }
        int question = 0;
        for (auto& q : currentScenario.timelineQuestions) {
            if (question == Thorne->LastQuestionAsked){
                ImGui::TextColored(ImVec4(0, 255, 0, 100), ("Last Question Asked " + q.text).c_str());
            }
            else if (Thorne->QuestionAskedSuspect(target, question)) {
                ImGui::TextColored(ImVec4(255, 255, 0, 100), ("Already Stored " + q.text).c_str());
            }
            else {
                ImGui::Text(q.text.c_str());

            }
            question++;
        }

    }
    InterrogationWidth = ImGui::GetWindowWidth();
    ImGui::End();

    if (activeSuspectIndex >= quantitySustec) {
        Thorne->Talk(12);
        activeSuspectIndex = 0;
        rounds++;
    }

    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - InterrogationWidth , 0), ImGuiCond_Always, ImVec2(1, 0));
    ImGui::Begin("Suspect Files", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    for (int i = 0; i < suspects.size(); i++) {
        if (i == Thorne->hatefullness[activeSuspectIndex].id) {
            ImGui::TextColored(ImVec4(0, 255, 0, 100), ("Asking: " + suspects[i]->name).c_str());
        }
        else if (i == Thorne->LastSuspectedInterrogates - 101){
            ImGui::TextColored(ImVec4(0, 255, 255, 100), ("Last Asked: " + suspects[i]->name).c_str());
        }
        else{

            ImGui::Text((suspects[i]->name).c_str());
        }
    }
    ImGui::End();
}

void C_GameManager::LoadLowIQWords(std::string filepath)
{
    std::ifstream file(filepath);
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string tostore;
        std::getline(ss, tostore, '\n');

        Thorne->RandomLowIQWords.push_back(tostore);
    }

}

void C_GameManager::InitializeGame()
{
    srand(static_cast<unsigned int>(time(NULL)));
    C_BasePawn::CharacterOrigin CharacterOrigin = C_BasePawn::GenerateRandomOrigin(true);
    Thorne = new C_Detective(CharacterOrigin.country, CharacterOrigin.cluster, true, true,120.0f,0);
    LoadScenarioFromCSV("./data/Gala/Questions.csv");
    LoadRoomsFromCSV("./data/Gala/Locations.csv");
    LoadLowIQWords("./data/Words.txt");
    LoadImages("./data/Images");
    
    int Alibis = rand() % 5 + 1;

    //printf("Alibis: %d\n", Alibis);
    LoadAlibisFromCSV("./data/Gala/Alibis/" + std::to_string(Alibis) + ".csv");
    for (int i = 1; i < quantitySustec + 1; i++) {
        C_BasePawn::CharacterOrigin susOrigin = C_BasePawn::GenerateRandomOrigin(false);
        std::shared_ptr<C_Suspect> s = std::make_shared<C_Suspect>(susOrigin.country, susOrigin.cluster, rand() % 2, rand() % 2, false, "Suspect_" + std::to_string(i), 0.7f);
        s->ID = 100 + i;

        // Assign a random Alibi Path from the library
        s->realTimeline = currentScenario.alibiLibrary[i-1];

        suspects.push_back(s);

    }
    Thorne->GenerateSuspectOrder(suspects);

    switch (Alibis) {
    case(1):
        suspects.at(4)->guilty = true;
        break;
    case(2):
        suspects.at(1)->guilty = true;
        break;
    case(3):
        suspects.at(2)->guilty = true;
        break;
    case(4):
        suspects.at(9)->guilty = true;
        break;
    case(5):
        suspects.at(0)->guilty = true;
        break;
    }
}
