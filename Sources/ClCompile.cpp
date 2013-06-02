#include "ClCompile.h"

using namespace kake;

#if _MSC_VER
#define snprintf _snprintf
#endif

namespace {
	std::string str(int i) {
		char buffer[20];
		snprintf(buffer, sizeof(buffer), "%d", i);
		return buffer;
	}
}

ClCompile::ClCompile(std::ofstream* out, int indentation, Platform platform, Configuration configuration, std::vector<std::string> includes, std::vector<std::string> defines) : Block(out, indentation), platform(platform), includes(includes), defines(defines) {
	warningLevel = 3;
	optimization = false;
	runtimeLibrary = "MultiThreadedDebug";
	multiProcessorCompilation = true;
	objectFileName = "$(IntDir)\\build\\%(RelativeDir)";
	generateDebugInformation = false;
	//this.configuration = configuration;
	switch (platform) {
	case WindowsRT:
		this->includes.push_back("$(ProjectDir)");
		this->includes.push_back("$(IntermediateOutputPath)");
		this->includes.push_back("%(AdditionalIncludeDirectories)");
		this->defines.push_back("_UNICODE");
		this->defines.push_back("UNICODE");
		this->defines.push_back("%(PreprocessorDefinitions)");
		break;
	case Xbox360:
		this->defines.push_back("_XBOX");
		switch (configuration) {
		case Debug:
			this->defines.push_back("_DEBUG");
			warningLevel = 3;
			prefast = false;
			optimization = false;
			functionLevelLinking = false;
			stringPooling = false;
			favorSize = false;
			fastCap = false;
			break;
		case CodeAnalysis:
			this->defines.push_back("_DEBUG");
			warningLevel = 4;
			prefast = true;
			optimization = false;
			functionLevelLinking = false;
			stringPooling = false;
			favorSize = false;
			fastCap = false;
			break;
		case Profile:
			this->defines.push_back("NDEBUG");
			this->defines.push_back("PROFILE");
			warningLevel = 3;
			prefast = false;
			optimization = true;
			functionLevelLinking = true;
			stringPooling = true;
			favorSize = true;
			fastCap = false;
			break;
		case Profile_FastCap:
			this->defines.push_back("NDEBUG");
			this->defines.push_back("FASTCAP");
			warningLevel = 3;
			prefast = false;
			optimization = true;
			functionLevelLinking = true;
			stringPooling = true;
			favorSize = true;
			fastCap = true;
			break;
		case Release:
			this->defines.push_back("NDEBUG");
			warningLevel = 3;
			prefast = false;
			optimization = true;
			functionLevelLinking = true;
			stringPooling = true;
			favorSize = true;
			fastCap = false;
			break;
		case Release_LTCG:
			this->defines.push_back("NDEBUG");
			this->defines.push_back("LTCG");
			warningLevel = 3;
			prefast = false;
			optimization = true;
			functionLevelLinking = true;
			stringPooling = true;
			favorSize = true;
			fastCap = false;
		}
		break;
	default:
		break;
	}
}
	
std::string ClCompile::toLine(std::vector<std::string> options) {
	std::string line = "";
	for (std::string option : options) {
		line += option + ";";
	}
	return line;
}

void ClCompile::print() {
	std::string defineLine = toLine(defines);
	std::string includeLine = toLine(includes);
		
	tagStart("ClCompile");
	tag("AdditionalIncludeDirectories", includeLine);
	if (platform == Platform::Windows) {
		tag("WarningLevel", "Level" + str(warningLevel));
		tag("Optimization", optimization ? "Enabled" : "Disabled");
		tag("PreprocessorDefinitions", defineLine);
		tag("RuntimeLibrary", runtimeLibrary);
		tag("MultiProcessorCompilation", multiProcessorCompilation ? "true" : "false");
		tag("MinimalRebuild", minimalRebuild ? "true" : "false");
		tag("ObjectFileName", objectFileName);
	}
	else if (platform == Platform::WindowsRT) {
		//tag("PreprocessorDefinitions", defineLine);
		tag("PrecompiledHeader", "NotUsing");
		tag("ObjectFileName", objectFileName);
		tag("DisableSpecificWarnings", "4453");
	}
	else if (platform == Platform::PlayStation3) {
		tag("UserPreprocessorDefinitions", userPreprocessorDefinitions);
		tag("GenerateDebugInformation", generateDebugInformation ? "true" : "false");
		tag("PreprocessorDefinitions", defineLine);
		tag("ObjectFileName", objectFileName);
	}
	else if (platform == Platform::Xbox360) {
		tag("PrecompiledHeader", "Use");
		tag("WarningLevel", "Level" + str(warningLevel));
		tag("DebugInformationFormat", "ProgramDatabase");
		tag("Optimization", optimization ? "Full" : "Disabled");
		if (functionLevelLinking) tag("FunctionLevelLinking", "true");
		tag("ExceptionHandling", "false");
		if (stringPooling) tag("StringPooling", "true");
		tag("MinimalRebuild", "true");
		if (prefast) tag("PREfast", "AnalyzeOnly");
		if (favorSize) tag("FavorSizeOrSpeed", "Size");
		tag("BufferSecurityCheck", "false");
		tag("PrecompiledHeaderOutputFile", "$(OutDir)$(ProjectName).pch");
		tag("RuntimeLibrary", runtimeLibrary);
		tag("PreprocessorDefinitions", defineLine);
		tag("CallAttributedProfiling", fastCap ? "Fastcap" : "Callcap");
	}
	tagEnd("ClCompile");
}

/*
//Debug
if (Options.Platform == Platform.Windows) {
	p("<WarningLevel>Level3</WarningLevel>", 3);
	p("<Optimization>Disabled</Optimization>", 3);
	if (system.equals("x64")) p("<PreprocessorDefinitions>" + defines + "SYS_64;WIN32;_DEBUG;_WINDOWS;%(PreprocessorDefinitions)</PreprocessorDefinitions>", 3);
	else p("<PreprocessorDefinitions>" + defines + "WIN32;_DEBUG;_WINDOWS;%(PreprocessorDefinitions)</PreprocessorDefinitions>", 3);
	p("<RuntimeLibrary>MultiThreadedDebug</RuntimeLibrary>", 3);
	p("<MultiProcessorCompilation>true</MultiProcessorCompilation>", 3);
	p("<MinimalRebuild>false</MinimalRebuild>", 3);
	p("<ObjectFileName>$(IntDir)/%(RelativeDir)</ObjectFileName>", 3);
}
else if (Options.Platform == Platform.PlayStation3) {
	p("<UserPreprocessorDefinitions>" + defines + "_DEBUG;__CELL_ASSERT__;%(UserPreprocessorDefinitions);</UserPreprocessorDefinitions>", 3);
	p("<GenerateDebugInformation>true</GenerateDebugInformation>", 3);
	p("<PreprocessorDefinitions>%(UserPreprocessorDefinitions);$(BuiltInDefines);__INTELLISENSE__;%(PreprocessorDefinitions);</PreprocessorDefinitions>", 3);
	p("<ObjectFileName>$(IntDir)/%(RelativeDir)</ObjectFileName>", 3);
}

//Release
//p("<PrecompiledHeader>Use</PrecompiledHeader>", 3);
p("<AdditionalIncludeDirectories>" + incstring + "</AdditionalIncludeDirectories>", 3);
if (Options.Platform == Platform.Windows) {
	p("<WarningLevel>Level3</WarningLevel>", 3);
	p("<Optimization>MaxSpeed</Optimization>", 3);
	p("<FunctionLevelLinking>true</FunctionLevelLinking>", 3);
	p("<IntrinsicFunctions>true</IntrinsicFunctions>", 3);
	if (system.equals("x64")) p("<PreprocessorDefinitions>" + defines + "SYS_64;WIN32;NDEBUG;_WINDOWS;%(PreprocessorDefinitions)</PreprocessorDefinitions>", 3);
	else p("<PreprocessorDefinitions>" + defines + "WIN32;NDEBUG;_WINDOWS;%(PreprocessorDefinitions)</PreprocessorDefinitions>", 3);
	p("<RuntimeLibrary>MultiThreaded</RuntimeLibrary>", 3);
	p("<MultiProcessorCompilation>true</MultiProcessorCompilation>", 3);
	p("<MinimalRebuild>false</MinimalRebuild>", 3);
	p("<ObjectFileName>$(IntDir)/%(RelativeDir)/</ObjectFileName>", 3);
}
else if (Options.Platform == Platform.PlayStation3) {
	p("<UserPreprocessorDefinitions>" + defines + "NDEBUG;%(UserPreprocessorDefinitions);</UserPreprocessorDefinitions>", 3);
	p("<OptimizationLevel>Level2</OptimizationLevel>", 3);
	p("<PreprocessorDefinitions>%(UserPreprocessorDefinitions);$(BuiltInDefines);__INTELLISENSE__;%(PreprocessorDefinitions);</PreprocessorDefinitions>");
	p("<ObjectFileName>$(IntDir)/%(RelativeDir)</ObjectFileName>", 3);
}
 */