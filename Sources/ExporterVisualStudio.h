#pragma once

#include "Exporter.h"
#include "UUID.h"
#include <string>
#include <vector>

namespace kake {
	class Project;

	class ExporterVisualStudio : public Exporter {
	public:
		void exportUserFile(Path directory, Project* project, Platform platform);
		void writeProjectDeclarations(Project* project, UUID solutionUuid);
		std::vector<std::string> getConfigs(Platform platform);
		std::vector<std::string> getSystems(Platform platform);
		std::string GetSys(Platform platform);
		void writeProjectBuilds(Project* project, Platform platform);
		void exportSolution(Solution* solution, Path directory, Platform platform) override;
		void exportManifest(Path directory, Project* project);
		void exportResourceScript(Path directory);
		void exportFilters(Path directory, Project* project, Platform platform);
		void addPropertyGroup(std::string buildType, Platform platform);
		void addPropertyGroup(std::string buildType, bool wholeProgramOptimization, Platform platform);
		void addWin8PropertyGroup(bool debug, std::string platform);
		void addItemDefinitionGroup(std::string incstring, std::string defines, std::string buildType, int warningLevel,
				bool prefast, bool optimization, bool functionLevelLinking, bool stringPooling, bool favorSize, bool release,
				bool profile, bool nocomdatfolding, bool ignoreXapilib, bool optimizeReferences, bool checksum, bool fastCap,
				bool comdatfolding, bool ltcg, Platform platform);
		void exportProject(Path directory, Project* project, Platform platform, bool cmd);
	};
}
