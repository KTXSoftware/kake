#pragma once

#include "Block.h"
#include "Platform.h"
#include <string>
#include <vector>

namespace kake {
	enum Configuration {
		Debug, CodeAnalysis, Profile, Profile_FastCap, Release, Release_LTCG
	};

	class ClCompile : public Block {
	public:
		ClCompile(std::ofstream* out, int indentation, Platform platform, Configuration configuration, std::vector<std::string> includes, std::vector<std::string> defines);
		static std::string toLine(std::vector<std::string> options);
		void print();
	private:
		int warningLevel;
		bool optimization;
		std::vector<std::string> defines;
		std::string runtimeLibrary;
		bool multiProcessorCompilation;
		bool minimalRebuild;
		std::string objectFileName;
		std::vector<std::string> includes;
	
		//PS3
		std::string userPreprocessorDefinitions;
		bool generateDebugInformation;
	
		//Xbox360
		bool functionLevelLinking;
		bool stringPooling;
		bool prefast;
		bool favorSize;
		bool fastCap;
	
		Platform platform;
		//Configuration configuration;
	};
}
