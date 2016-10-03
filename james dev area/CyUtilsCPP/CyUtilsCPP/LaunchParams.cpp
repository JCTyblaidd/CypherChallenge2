#include "LaunchParams.hpp"
#include <string>

namespace params {
	void LaunchParams::Load(int argCount, char *args[]) {
		flags.clear();
		integral_vars.clear();
		variable_vars.clear();
		//Parse//
		for (size_t index = 0; index < argCount; index++) {
			std::string str = std::string(args[index]);
			size_t diff = str.find('=', 0);
			if (str[0] != '-') {
				continue;
			}
			if (diff == std::string::npos) {
				//Flag//
				std::string flagType = str.substr(1);
				flags.emplace(flagType);
			}
			else {
				std::string prelim = str.substr(1, diff - 1);
				std::string postLim = str.substr(diff + 1);
				if (postLim == "true") {
					integral_vars.emplace(prelim, 1);
				}
				else if (postLim == "false") {
					integral_vars.emplace(prelim, 0);
				}
				else {
					try {
						int32_t t = std::stoi(postLim);
						integral_vars.emplace(prelim, t);
					}
					catch (std::exception err) {
						variable_vars.emplace(prelim, postLim);
					}
				}
			}
		}
	}
	bool LaunchParams::hasFlag(std::string str) {
		return flags.find(str) != flags.end();
	}

	std::string LaunchParams::getStringWithDefault(std::string var, std::string _default) {
		auto v = variable_vars.find(var);
		if (v == variable_vars.end()) {
			return _default;
		}
		else {
			return variable_vars.at(var);
		}
	}

	int32_t LaunchParams::getIntWithDefault(std::string var, int32_t _default) {
		auto v = integral_vars.find(var);
		if (v == integral_vars.end()) {
			return _default;
		}
		else {
			return integral_vars.at(var);
		}
	}
	bool LaunchParams::getConditionWithDefault(std::string var, bool _default) {
		return getIntWithDefault(var, static_cast<int32_t>(_default)) != 0;
	}
}