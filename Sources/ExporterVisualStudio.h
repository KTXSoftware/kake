#pragma once

#include "Exporter.h"
#include "UUID.h"
#include <string>
#include <vector>

namespace kake {
	class Project;

	class ExporterVisualStudio : public Exporter {
	public:
		void exportUserFile(kmd::Path from, kmd::Path to, Project* project, Platform platform);
		void writeProjectDeclarations(Project* project, UUID solutionUuid);
		std::vector<std::string> getConfigs(Platform platform);
		std::vector<std::string> getSystems(Platform platform);
		std::string GetSys(Platform platform);
		void writeProjectBuilds(Project* project, Platform platform);
		void exportSolution(Solution* solution, kmd::Path from, kmd::Path to, Platform platform) override;
		void exportManifest(kmd::Path directory, Project* project);
		void exportResourceScript(kmd::Path directory);
		void exportFilters(kmd::Path from, kmd::Path to, Project* project, Platform platform);
		void addPropertyGroup(std::string buildType, Platform platform);
		void addPropertyGroup(std::string buildType, bool wholeProgramOptimization, Platform platform);
		void addWin8PropertyGroup(bool debug, std::string platform);
		void addItemDefinitionGroup(std::string incstring, std::string defines, std::string buildType, int warningLevel,
				bool prefast, bool optimization, bool functionLevelLinking, bool stringPooling, bool favorSize, bool release,
				bool profile, bool nocomdatfolding, bool ignoreXapilib, bool optimizeReferences, bool checksum, bool fastCap,
				bool comdatfolding, bool ltcg, Platform platform);
		void exportProject(kmd::Path from, kmd::Path to, Project* project, Platform platform, bool cmd);
		void exportAssetPath(kmd::Path assetPath);
		void exportAssetPathFilter(kmd::Path assetPath, std::vector<std::string>& dirs, std::vector<std::string>& assets);
	};
}
