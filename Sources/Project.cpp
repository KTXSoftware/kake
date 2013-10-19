#include "Project.h"
#include "Files.h"
#include "Solution.h"
#include "String.h"
#include <set>
#ifdef __GNUC__
#include <boost/regex.hpp>
#else
#include <regex>
#endif

using namespace kake;

Path koreDir;

Project::Project(std::string name) : name(name) {
	debugDir = "";
	basedir = Solution::scriptdir;
	if (name == "Kore") koreDir = basedir;
	uuid = UUID::randomUUID();
}

void Project::flatten() {
	for (Project* sub : subProjects) sub->flatten();
	for (Project* sub : subProjects) {
		Path basedir = this->basedir;
		//if (basedir.startsWith("./")) basedir = basedir.substring(2);
		Path subbasedir = sub->basedir;
		//if (subbasedir.startsWith("./")) subbasedir = subbasedir.substring(2);
		//if (subbasedir.startsWith(basedir)) subbasedir = subbasedir.substring(basedir.length());
		if (subbasedir.startsWith(basedir)) subbasedir = basedir.relativize(subbasedir);

		for (std::string define : sub->defines) if (!contains(defines, define)) defines.push_back(define);
		for (std::string file : sub->files) files.push_back(stringify(subbasedir.resolve(file)));
		for (std::string include : sub->includeDirs) if (!contains(includeDirs, stringify(subbasedir.resolve(include)))) includeDirs.push_back(stringify(subbasedir.resolve(include)));
		for (std::string lib : sub->libs) {
			if (!contains(lib, '/') && !contains(lib, '\\')) {
				if (!contains(libs, lib)) libs.push_back(lib);
			}
			else {
				if (!contains(libs, stringify(subbasedir.resolve(lib)))) libs.push_back(stringify(subbasedir.resolve(lib)));
			}
		}
		for (auto system : sub->systemDependendLibraries) {
			for (std::string lib : system.second) {
				if (systemDependendLibraries.find(system.first) == systemDependendLibraries.end()) systemDependendLibraries[system.first] = std::vector<std::string>();
				if (!contains(systemDependendLibraries[system.first], stringify(subbasedir.resolve(lib)))) {
					systemDependendLibraries[system.first].push_back(stringify(subbasedir.resolve(lib)));
				}
			}
		}
	}
	subProjects.clear();
}

std::string Project::getName() {
	return name;
}

UUID Project::getUuid() {
	return uuid;
}

bool Project::matches(std::string text, std::string pattern) {
	std::string regexstring = replace(replace(replace(replace(pattern, '.', "\\."), "**", ".?"), '*', "[^/]*"), '?', '*');
#ifdef __GNUC__
    boost::regex regex(regexstring);
	return boost::regex_match(text.begin(), text.end(), regex);
#else
	std::regex regex(regexstring);
	return std::regex_match(text.begin(), text.end(), regex);
#endif
}

bool Project::matchesAllSubdirs(Path dir, std::string pattern) {
	if (endsWith(pattern, "/**")) {
		return matches(stringify(dir), pattern.substr(0, pattern.length() - 3));
	}
	else return false;
}

std::string Project::stringify(Path path) {
	return replace(path.toString(), '\\', '/');
}

namespace {
	bool isAbsolute(std::string path) {
		return (path.size() > 0 && path[0] == '/') || (path.size() > 1 && path[1] == ':');
	}
}

void Project::searchFiles(Path current) {
	for (Path file : Files::newDirectoryStream(current)) {
		if (Files::isDirectory(file)) continue;
		//if (!current.isAbsolute())
			file = basedir.relativize(file);
		for (std::string exclude : excludes) {
			if (matches(stringify(file), exclude)) goto nextfile;
		}
		for (std::string include : includes) {
			if (isAbsolute(include)) {
				Path inc = Paths::get(include);
				inc = basedir.relativize(inc);
				include = inc.path;
			}
			if (matches(stringify(file), include)) {
				files.push_back(stringify(file));
			}
		}
		nextfile:;
	}
	for (Path dir : Files::newDirectoryStream(current)) {
		if (!Files::isDirectory(dir)) continue;
		for (std::string exclude : excludes) {
			if (matchesAllSubdirs(basedir.relativize(dir), exclude)) {
				goto nextdir;
			}
		}
		searchFiles(dir);
		nextdir:;
	}
}

void Project::searchFiles() {
	for (Project* sub : subProjects) sub->searchFiles();
	searchFiles(basedir);
	//std::set<std::string> starts;
	//for (std::string include : includes) {
	//	if (!isAbsolute(include)) continue;
	//	std::string start = include.substr(0, firstIndexOf(include, '*'));
	//	if (starts.count(start) > 0) continue;
	//	starts.insert(start);
	//	searchFiles(Paths::get(start));
	//}
}

void Project::addFile(std::string file) {
	includes.push_back(file);
}

void Project::addExclude(std::string exclude) {
	excludes.push_back(exclude);
}

void Project::addDefine(std::string define) {
	if (contains(defines, define)) return;
	defines.push_back(define);
}

void Project::addIncludeDir(std::string include) {
	if (contains(includeDirs, include)) return;
	includeDirs.push_back(include);
}

void Project::addSubProject(Project* project) {
	subProjects.push_back(project);
}

void Project::addLib(std::string lib) {
	libs.push_back(lib);
}

void Project::addLibFor(std::string system, std::string lib) {
	if (systemDependendLibraries.find(system) == systemDependendLibraries.end()) systemDependendLibraries[system] = std::vector<std::string>();
	systemDependendLibraries[system].push_back(lib);
}

std::vector<std::string>& Project::getFiles() {
	return files;
}

Path Project::getBasedir() {
	return basedir;
}

std::vector<Project*> Project::getSubProjects() {
	return subProjects;
}

std::vector<std::string> Project::getIncludeDirs() {
	return includeDirs;
}

std::vector<std::string> Project::getDefines() {
	return defines;
}

std::vector<std::string> Project::getLibs() {
	return libs;
}

std::vector<std::string> Project::getLibsFor(std::string system) {
	if (systemDependendLibraries.find(system) == systemDependendLibraries.end()) return std::vector<std::string>();
	return systemDependendLibraries[system];
}

std::string Project::getDebugDir() {
	return debugDir;
}

void Project::setDebugDir(std::string debugDir) {
	this->debugDir = debugDir;
}
