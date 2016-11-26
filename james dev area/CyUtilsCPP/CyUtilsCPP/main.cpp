#include "MainFramework.hpp"
#include <fstream>
#include "cy_header.h"
#include <algorithm>
#include "pwn_cy.hpp"

//Work Pool For HEAVY WORKLOADS
WorkPoolType* WorkPool;
//Parameter Parser
params::LaunchParams LaunchParameters;
//Log Utility
logging::ConsoleLogHandler* LogHandle;

std::string upperStr(std::string Str) {
	std::string str(Str);
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	return str;
}

void runHandle(RunnableCache Cache) {
	Cache.Func(Cache.Data);
}


std::string sanitizeCipherText(std::string str) {
	std::string out;
	std::string in = upperStr(str);
	for (char c : in) {
		if (c >= 'A' && c <= 'Z') {
			out += c;
		}
		else if (c >= 'a' && c <= 'z') {
			char nchar = c - 'a' + 'A';
			out += nchar;
		}
	}
	return out;
}

int main(int argCount, char *args[]) {
	//Load Parameters//
	LogHandle = new logging::ConsoleLogHandler();
	uint threadCount = std::thread::hardware_concurrency();
	INFO("Loading ThreadPool w/ Size=" + std::to_string(threadCount));
	WorkPool = new WorkPoolType(threadCount, runHandle);
	LaunchParameters.Load(argCount, args);
	//OK Now Find Work
	String srcLocation = LaunchParameters.getStringWithDefault("cipher=", "cypher.txt");
	INFO("Loading CypherText");
	std::string cypher_text;
	try {
		std::ifstream file(getFileLoc("CurrentCipher.txt"));
		std::string str;
		while (std::getline(file, str))
		{
			cypher_text += str;
			cypher_text.push_back('\n');
		}
	}
	catch (std::exception ex) {
		SEVERE("Error Loading File");
		SEVERE(ex.what());
		system("pause");
		delete WorkPool;
		delete LogHandle;
		exit(-1);
	}
	if (cypher_text.length() == 0) {
		SEVERE("Cipher Length = 0???");
		SEVERE("Error Loading File");
		system("pause");
		delete WorkPool;
		delete LogHandle;
		exit(-1);
	}
	ngram_stats::loadStats();
	cypher_text = sanitizeCipherText(cypher_text);
	String jobRequest = LaunchParameters.getStringWithDefault("type", "none");
#if USE_WORKSPACE_LAYOUT
	{
		//DEBUG CODE//
		pwn_cipher::pwn_vigenere(cypher_text);
		//END OF DEBUG CODE//
	}
#endif
	if (jobRequest == "none") {
		INFO("No Task Requested.. terminating");
	}
	else if(jobRequest == "caesar" || jobRequest == "affine") {
		pwn_cipher::pwn_affine(cypher_text);
	}
	else if (jobRequest == "frequency") {
		runFreqAnalysis(cypher_text);
	}
	else if (jobRequest == "vignere") {
		pwn_cipher::pwn_vigenere(cypher_text);
	}
	else if (jobRequest == "keyword") {
		pwn_cipher::pwn_keyword(cypher_text);
	}
	else if (jobRequest == "transposition") {
		pwn_cipher::pwn_transposition(cypher_text);
	}
	else if (jobRequest == "columnar") {
		pwn_cipher::pwn_columnar(cypher_text);
	}
	else if (jobRequest == "amsco") {
		pwn_cipher::pwn_amsco(cypher_text);
	}
	else {
		INFO("Unknown Job Type:");
		INFO(" > " + jobRequest);
	}
	
	WorkPool->awaitWorkFinished();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	WorkPool->awaitWorkFinished();
	//Await Shutdown//
	system("pause");
	delete WorkPool;
	delete LogHandle;
}