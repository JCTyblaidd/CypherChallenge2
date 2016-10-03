#include "MainFramework.hpp"
#include <fstream>

WorkPoolType* WorkPool;
params::LaunchParams LaunchParameters;
logging::ConsoleLogHandler* LogHandle;

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
	String jobRequest = LaunchParameters.getStringWithDefault("type", "none");
	if (jobRequest == "none") {
		INFO("No Task Requested.. terminating");
	}
	else if(jobRequest == "ceasar") {
		//TODO:
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