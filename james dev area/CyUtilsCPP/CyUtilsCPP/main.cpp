#include "MainFramework.hpp"
#include <fstream>
#include "cy_header.h"
#include <algorithm>

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
	}
	return out;
}

int main(int argCount, char *args[]) {
	//Load Parameters//
	LogHandle = new logging::ConsoleLogHandler();
	uint threadCount = std::thread::hardware_concurrency();
	WorkPool = new WorkPoolType(threadCount, runHandle);
	LaunchParameters.Load(argCount, args);
	//OK Now Find Work
	String srcLocation = LaunchParameters.getStringWithDefault("cipher=", "cypher.txt");
	INFO("Loading CypherText");
	std::string cypher_text;
	try {
		std::ifstream file("Read.txt");
		std::string str;
		while (std::getline(file, str))
		{
			cypher_text += str;
			cypher_text.push_back('\n');
		}
	}
	catch (std::exception ex) {
		SEVERE("Error Loading File");
		system("pause");
		delete WorkPool;
		delete LogHandle;
		exit(-1);
	}
	if (cypher_text.size() == 0) {
		SEVERE("Error Loading File");
		system("pause");
		delete WorkPool;
		delete LogHandle;
		exit(-1);
	}
	cypher_text = sanitizeCipherText(cypher_text);
	String jobRequest = LaunchParameters.getStringWithDefault("type", "none");
	if (jobRequest == "none") {
		INFO("No Task Requested.. terminating");
	}
	else if(jobRequest == "caesar") {
		runCeasar(cypher_text);
	}
	else if (jobRequest == "frequency") {
		runFreqAnalysis(cypher_text);
	}
	else {
		INFO("Unknown Job Type:");
		INFO(" > " + jobRequest);
	}
	//Await Shutdown//
	system("pause");
	delete WorkPool;
	delete LogHandle;
}