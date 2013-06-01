#include "ExporterAndroid.h"
#include "ExporterCodeBlocks.h"
#include "ExporterVisualStudio.h"
#include "ExporterXCode.h"
#include "Files.h"
#include "Options.h"
#include "Path.h"
#include "Platform.h"
#include "Project.h"
#include "Random.h"
#include "Solution.h"
#include "String.h"
#include <iostream>

#ifdef SYS_OSX
#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <cstring>
#include <errno.h>
#endif

using namespace kake;

extern Path koreDir;

namespace {
	std::string fromPlatform(Platform platform) {
		switch (platform) {
		case Windows:
			return "Windows";
		case WindowsRT:
			return "WindowsRT";
		case PlayStation3:
			return "PlayStation 3";
		case iOS:
			return "iOS";
		case OSX:
			return "OS X";
		case Android:
			return "Android";
		case Xbox360:
			return "Xbox 360";
		case Linux:
			return "Linux";
		default:
			return "unknown";
		}
	}

	std::string shaderLang(Platform platform) {
		switch (platform) {
			case Windows:
				switch (Options::getGraphicsApi()) {
					case kake::OpenGL:
					case kake::OpenGL2:
						return "glsl";
					case kake::Direct3D9:
						return "d3d9";
					case kake::Direct3D11:
						return "d3d11";
					default:
						return "d3d9";
				}
			case WindowsRT:
				return "d3d11";
			case PlayStation3:
				return "d3d9";
			case iOS:
				return "essl";
			case OSX:
				return "glsl";
			case Android:
				return "essl";
			case Xbox360:
				return "d3d9";
			case Linux:
				return "glsl";
			default:
				return "unknown";
		}
	}
}
	void compileShader(std::string type, std::string from, std::string to, std::string temp) {
#ifdef SYS_OSX
		if (koreDir.toString().size() > 0) {
			Path path = koreDir.resolve(Paths::get("Tools", "kfx", "kfx-osx"));
			std::string exe = path.toString() + " " + type + " " + from + " " + to + " " + temp;
			system(exe.c_str());
		}
#endif
#ifdef SYS_LINUX
		if (koreDir.toString().size() > 0) {
			Path path = koreDir.resolve(Paths::get("Tools", "kfx", "kfx-linux"));
			std::string exe = path.toString() + " " + type + " " + from + " " + to + " " + temp;
			system(exe.c_str());
		}
#endif
	}

namespace {
	std::string exportKakeProject(Path directory, Platform platform) {
		std::cout << "kake.lua found, generating build files." << std::endl;

		if (platform == HTML5) std::cout << "Compiling to JavaScript." << std::endl;
		else std::cout << "Generating " << fromPlatform(platform) + " solution";

		Solution* solution = Solution::create(directory, platform);
		std::cout << ".";
		solution->searchFiles();
		std::cout << ".";
		solution->flatten();
		std::cout << ".";
		Exporter* exporter = nullptr;
		if (platform == iOS || platform == OSX) exporter = new ExporterXCode();
		else if (platform == Android) exporter = new ExporterAndroid();
		//else if (platform == HTML5) exporter = new ExporterEmscripten();
		else if (platform == Linux) exporter = new ExporterCodeBlocks();
		else exporter = new ExporterVisualStudio();
		exporter->exportSolution(solution, directory, platform);
#ifndef SYS_WINDOWS
		Project* project = solution->getProjects()[0];
		std::vector<std::string> files = project->getFiles();
		for (std::string file : files) {
			if (endsWith(file, ".glsl")) {
				std::string outfile = file;
				int index = lastIndexOf(outfile, '/');
				if (index > 0) outfile = outfile.substr(index);
				outfile = outfile.substr(0, outfile.size() - 5);
				compileShader(shaderLang(platform), file, project->getDebugDir() + "/" + outfile, "build");
			}
		}
#endif
		std::cout << ".done." << std::endl;
		return solution->getName();
	}

	bool isKakeProject(Path directory) {
		return Files::exists(directory.resolve("kake.lua"));
	}

	std::string exportProject(Path directory, Platform platform) {
		if (isKakeProject(directory)) {
			return exportKakeProject(directory, platform);
		}
		else {
			std::cerr << "kake.lua not found." << std::endl;
			return "";
		}
	}
}

int main(int argc, char** argv) {
	Random::init(rand());
	std::string path = ".";
	#ifdef SYS_WINDOWS
	Platform platform = Platform::Windows;
	#endif
	#ifdef SYS_LINUX
	Platform platform = Platform::Linux;
	#endif
	#ifdef SYS_OSX
	Platform platform = Platform::OSX;
	#endif
	for (int i = 1; i < argc; ++i) {
		std::string arg(argv[i]);
		if (arg == "windows") platform = Windows;
		else if (arg == "linux") platform = Linux;
		else if (arg == "xbox360") platform = Xbox360;
		else if (arg == "ps3") platform = PlayStation3;
		else if (arg == "android") platform = Android;
		else if (arg == "windowsrt") platform = WindowsRT;
		else if (arg == "osx") platform = OSX;
		else if (arg == "ios") platform = iOS;

		else if (arg == "pch") Options::setPrecompiledHeaders(true);
		else if (startsWith(arg, "intermediate=")) Options::setIntermediateDrive(arg.substr(13));
		else if (startsWith(arg, "gfx=")) Options::setGraphicsApi(arg.substr(4));
		else if (startsWith(arg, "vs=")) Options::setVisualStudioVersion(arg.substr(3));

		else path = arg;
	}
	exportProject(Paths::get(path), platform);
}
