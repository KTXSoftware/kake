#include "ExporterTizen.h"
#include "FileReader.h"
#include "Files.h"
#include "Project.h"
#include "Solution.h"
#include "String.h"

using namespace kake;

extern Path koreDir;

namespace {
	void createDirectory(Path dir) {
		if (!Files::exists(dir)) Files::createDirectories(dir);
	}

	void copyDirectory(Path from, Path to) {
		createDirectory(to);
		for (auto path : Files::newDirectoryStream(from)) {
			if (Files::isDirectory(path)) {
				copyDirectory(path, to.resolve(from.relativize(path)));
			}
			else {
				Files::copy(path, to.resolve(from.relativize(path)), true);
			}
		}
	}

	std::string readFile(std::string filename) {
		kmd::FileReader reader(filename.c_str());
		return std::string((char*)reader.readAll(), reader.size());
	}
}

void ExporterTizen::exportSolution(Solution* solution, Path from, Path to, Platform platform) {
	Project* project = solution->getProjects()[0];
	
	copyDirectory(from.resolve(project->getDebugDir()), to.resolve("data"));

	std::string dotcproject = readFile(Paths::executableDir().resolve(Paths::get("Data", "tizen", ".cproject")).toString());
	writeFile(to.resolve(".cproject"));
	dotcproject = replace(dotcproject, "{ProjectName}", solution->getName());
	std::string includes;
	for (auto include : project->getIncludeDirs()) {
		includes += "<listOptionValue builtIn=\"false\" value=\"&quot;${workspace_loc:/${ProjName}/CopiedSources/" + include + "}&quot;\"/>";
	}
	dotcproject = replace(dotcproject, "{includes}", includes);
	std::string defines;
	for (auto define : project->getDefines()) {
		defines += "<listOptionValue builtIn=\"false\" value=\"" + define + "\"/>";
	}
	dotcproject = replace(dotcproject, "{defines}", defines);
	out->write(dotcproject.c_str(), dotcproject.size());
	closeFile();

	std::string dotproject = readFile(Paths::executableDir().resolve(Paths::get("Data", "tizen", ".project")).toString());
	writeFile(to.resolve(".project"));
	dotproject = replace(dotproject, "{ProjectName}", solution->getName());
	out->write(dotproject.c_str(), dotproject.size());
	closeFile();

	std::string manifest = readFile(Paths::executableDir().resolve(Paths::get("Data", "tizen", "manifest.xml")).toString());
	writeFile(to.resolve("manifest.xml"));
	manifest = replace(manifest, "{ProjectName}", solution->getName());
	out->write(manifest.c_str(), manifest.size());
	closeFile();

	for (auto file : project->getFiles()) {
		Path target = to.resolve("CopiedSources").resolve(file);
		createDirectory(Paths::get(target.path.substr(0, lastIndexOf(target.path, '/'))));
		Files::copy(from.resolve(file), target, true);
	}
}
