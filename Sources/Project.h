#pragma once

#include "Path.h"
#include "UUID.h"
#include <map>
#include <string>
#include <vector>

namespace kake {
	class Project {
	
	public:
		Project(std::string name);
		void flatten();
		std::string getName();
		UUID getUuid();
		bool matches(std::string text, std::string pattern);
		bool matchesAllSubdirs(kmd::Path dir, std::string pattern);
		std::string stringify(kmd::Path path);
		void searchFiles(kmd::Path current);
		void searchFiles();
		void addFile(std::string file);
		void addExclude(std::string exclude);
		void addDefine(std::string define);
		void addIncludeDir(std::string include);
		void addSubProject(Project* project);
		void addLib(std::string lib);
		void addLibFor(std::string system, std::string lib);
		std::vector<std::string>& getFiles();
		kmd::Path getBasedir();
		std::vector<Project*> getSubProjects();
		std::vector<std::string> getIncludeDirs();
		std::vector<std::string> getDefines();
		std::vector<std::string> getLibs();
		std::vector<std::string> getLibsFor(std::string system);
		std::string getDebugDir();
		void setDebugDir(std::string debugDir);
		std::vector<std::string> includes;
		std::vector<std::string> excludes;
	private:
		std::string name;
		std::vector<std::string> files;
		kmd::Path basedir;
		std::vector<Project*> subProjects;
		std::vector<std::string> includeDirs;
		std::vector<std::string> defines;
		std::vector<std::string> libs;
		std::map<std::string, std::vector<std::string> > systemDependendLibraries;
		std::string debugDir;
		UUID uuid;
	};
}
