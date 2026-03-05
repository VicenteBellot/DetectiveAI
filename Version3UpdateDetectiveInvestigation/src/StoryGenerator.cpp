

#include "StoryGenerator.hpp"

C_StoryGen::C_StoryGen(std::string filepathLog)
{
    LogFile = fopen(filepathLog.c_str(), "w");
    if (LogFile == NULL) {
        perror("fopen failed");  // prints the exact OS error reason
        fprintf(stderr, "Path was: '%s'\n", filepathLog.c_str());
    }
}
C_StoryGen::~C_StoryGen()
{
    if(LogFile) fclose(LogFile);
}

void C_StoryGen::AddString(std::string stringtoadd)
{
    if (LogFile) {
        fputs(stringtoadd.c_str(), LogFile);
        fflush(LogFile); 
    }
}
/*

std::string C_StoryGen::GenerateStory()
{
    if (LogFile) {
        fclose(LogFile);
        LogFile = nullptr;
    }

    std::ifstream file(m_filepathLog);
    if (!file.is_open()) {
        std::cerr << "Error: could not read log file\n";
        return "";
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    std::string fileContent = ss.str();

    // 3. Send to Llama3 and return the story
    return sendToLlama(fileContent);
}

std::string C_StoryGen::sendToLlama(const std::string& fileContent)
{
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        json body = {
            {"model", "llama3"},
            {"messages", json::array({
                {
                    {"role", "user"},
                    {"content", "Based on this investigation log, generate a story:\n\n" + fileContent}
                }
            })},
            {"stream", false}
        };

        std::string bodyStr = body.dump();
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:11434/api/chat");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyStr.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Error: Ollama not running. Start it with 'ollama serve'\n";
            return "";
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    auto jsonResp = json::parse(response);
    return jsonResp["message"]["content"];
}
*/