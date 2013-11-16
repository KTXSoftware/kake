#include "ExporterTizen.h"
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
}

void ExporterTizen::exportSolution(Solution* solution, Path from, Path to, Platform platform) {
	Project* project = solution->getProjects()[0];
	
	copyDirectory(from.resolve(project->getDebugDir()), to.resolve("data"));

	std::ifstream dotproject(Paths::executableDir().resolve(".project").toString());
	
	writeFile(to.resolve(".project"));
	
	closeFile();

	
	writeFile(to.resolve(Paths::get("jni", "Android.mk")));
	p("LOCAL_PATH := $(call my-dir)");
	p();
	p("include $(CLEAR_VARS)");
	p();
	p("LOCAL_MODULE    := Kore");
	std::string files = "";
	for (std::string filename : project->getFiles()) if (endsWith(filename, ".c") || endsWith(filename, ".cpp") || endsWith(filename, ".cc") || endsWith(filename, ".s")) files += "../../" + filename + " ";
	p("LOCAL_SRC_FILES := " + files);
	std::string defines = "";
	for (std::string def : project->getDefines()) defines += "-D" + replace(def, '\"', "\\\"") + " ";
	p("LOCAL_CFLAGS := " + defines);
	std::string includes = "";
	for (std::string inc : project->getIncludeDirs()) includes += "$(LOCAL_PATH)/../../" + inc + " ";
	p("LOCAL_C_INCLUDES := " + includes);
	p("LOCAL_LDLIBS    := -llog -lGLESv2");
	p();
	p("include $(BUILD_SHARED_LIBRARY)");
	p();
	closeFile();

	writeFile(to.resolve(Paths::get("jni", "Application.mk")));
	p("APP_CPPFLAGS += -fexceptions -frtti");
	p("APP_STL := gnustl_static");
	//p("APP_ABI := all");
	p("APP_ABI := armeabi-v7a");
	//p("LOCAL_ARM_NEON := true");

	closeFile();
}
