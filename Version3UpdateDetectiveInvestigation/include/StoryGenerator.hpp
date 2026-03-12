#ifndef STORYGENERATOR_HPP
#define STORYGENERATOR_HPP

#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
	output->append((char*)contents, size * nmemb);
	return size * nmemb;
}

class C_StoryGen {
public:
	C_StoryGen(std::string filepathLog);

	~C_StoryGen();

	FILE* LogFile;
	std::string m_filepathLog;

	void AddString(std::string stringtoadd);

	std::string GenerateStory();

private:
	std::string sendToLlama(const std::string& fileContent);

};

#endif // !SUSPECT_PAWN_HPP
