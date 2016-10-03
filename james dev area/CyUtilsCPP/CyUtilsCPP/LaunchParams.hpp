#pragma once
#include <unordered_set>
#include <unordered_map>
#include <cinttypes>

namespace params {
	class LaunchParams {
	private:
		std::unordered_set<std::string, std::hash<std::string>> flags;
		std::unordered_map<std::string, int32_t, std::hash<std::string>> integral_vars;
		std::unordered_map<std::string, std::string, std::hash<std::string>> variable_vars;
	public:
		void Load(int argCount, char *args[]);
		bool hasFlag(std::string str);
		std::string getStringWithDefault(std::string var, std::string _default);
		int32_t getIntWithDefault(std::string var, int32_t _default);
		bool getConditionWithDefault(std::string var, bool _default);
	};
}