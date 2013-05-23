#pragma once

#include "Path.h"
#include "Platform.h"
#include <fstream>
#include <string>
#include <vector>

namespace kake {
	class Project;

	class Solution {
	public:
		Solution(std::string name);
		std::string getName();
		std::vector<Project*> getProjects();
		void addProject(Project* project);
		void searchFiles();
		void flatten();
		static Project* createProject(std::string filename);
		static Solution* createSolution(std::string filename, Platform platform);
		static Project* evalProjectScript(std::ifstream& script);
		static Solution* evalSolutionScript(std::ifstream& script, Platform platform);
		static Solution* create(Path directory, Platform platform);
		static Path scriptdir;
		bool isRotated();
		bool isCmd();
		void setRotated();
		void setCmd();
	private:
		bool rotated;
		bool cmd;
		std::string name;
		std::vector<Project*> projects;
	};
}
