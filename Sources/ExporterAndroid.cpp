#include "ExporterAndroid.h"
#include "Files.h"
#include <FileReader.h>
#include "Project.h"
#include "Solution.h"
#include "StringHelper.h"

using namespace kmd;
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

void ExporterAndroid::exportSolution(Solution* solution, Path from, Path to, Platform platform) {
	Project* project = solution->getProjects()[0];
	//String libname = solution.getName().toLowerCase().replace(' ', '-');

	bool nvpack = false; //Configuration.getAndroidDevkit() == AndroidDevkit.nVidia;

	copyDirectory(from.resolve(project->getDebugDir()), to.resolve("assets"));

	Files::copy(Paths::executableDir().resolve(Paths::get("Data", "android", ".classpath")), to.resolve(".classpath"), true);

	if (nvpack) {
		std::string file = readFile(Paths::executableDir().resolve(Paths::get("Data", "android", "nvidia", ".project")).toString());
		writeFile(to.resolve(".project"));
		file = replace(file, "{ProjectName}", solution->getName());
		out->write(file.c_str(), file.size());
		closeFile();
	}
	else {
		std::string file = readFile(Paths::executableDir().resolve(Paths::get("Data", "android", ".project")).toString());
		writeFile(to.resolve(".project"));
		file = replace(file, "{ProjectName}", solution->getName());
		file = replace(file, "{Java-Sources}", koreDir.resolve(Paths::get("Backends", "Android", "Java-Sources")).toAbsolutePath().toString());
		out->write(file.c_str(), file.size());
		closeFile();
	}

	if (nvpack) {
		std::string file = readFile(Paths::executableDir().resolve(Paths::get("Data", "android", "nvidia", ".cproject")).toString());
		writeFile(to.resolve(".cproject"));
		file = replace(file, "{ProjectName}", solution->getName());
		out->write(file.c_str(), file.size());
		closeFile();
	}
	else {
		std::string file = readFile(Paths::executableDir().resolve(Paths::get("Data", "android", ".cproject")).toString());
		writeFile(to.resolve(".cproject"));
		file = replace(file, "{ProjectName}", solution->getName());
		out->write(file.c_str(), file.size());
		closeFile();
	}

	Files::copy(Paths::executableDir().resolve(Paths::get("Data", "android", "AndroidManifest.xml")), to.resolve("AndroidManifest.xml"), true);
	Files::copy(Paths::executableDir().resolve(Paths::get("Data", "android", "project.properties")), to.resolve("project.properties"), true);

	createDirectory(to.resolve(".settings"));
	if (nvpack) {
		Files::copy(Paths::executableDir().resolve(Paths::get("Data", "android", "nvidia", "org.eclipse.jdt.core.prefs")), to.resolve(Paths::get(".settings", "org.eclipse.jdt.core.prefs")), true);
	}
	else {
		Files::copy(Paths::executableDir().resolve(Paths::get("Data", "android", "org.eclipse.jdt.core.prefs")), to.resolve(Paths::get(".settings", "org.eclipse.jdt.core.prefs")), true);
	}

	if (nvpack) {
		Files::copy(Paths::executableDir().resolve(Paths::get("Data", "android", "nvidia", "build.xml")), to.resolve("build.xml"), true);
	}

	createDirectory(to.resolve("res"));
	createDirectory(to.resolve(Paths::get("res", "values")));
	std::string file = readFile(Paths::executableDir().resolve(Paths::get("Data", "android", "strings.xml")).toString());
	writeFile(to.resolve(Paths::get("res", "values", "strings.xml")));
	file = replace(file, "{ProjectName}", solution->getName());
	out->write(file.c_str(), file.size());
	closeFile();

	createDirectory(to.resolve("jni"));

	if (nvpack) {
/*
LOCAL_PATH := $(subst //,/,$(call my-dir))
include $(CLEAR_VARS)
LOCAL_MODULE := native_basic

LOCAL_SRC_FILES := $(wildcard *.cpp)
LOCAL_SRC_FILES += $(wildcard *.c)

LOCAL_ARM_MODE   := arm

LOCAL_LDLIBS :=  -lstdc++ -lc -lm -llog -landroid -ldl -lGLESv2 -lEGL
LOCAL_STATIC_LIBRARIES := nv_and_util nv_egl_util nv_bitfont nv_math nv_glesutil nv_hhdds nv_log nv_shader nv_file nv_thread

include $(BUILD_SHARED_LIBRARY)

$(call import-add-path, ../../../libs/jni)

$(call import-module,nv_and_util)
$(call import-module,nv_egl_util)
$(call import-module,nv_bitfont)
$(call import-module,nv_math)
$(call import-module,nv_glesutil)
$(call import-module,nv_hhdds)
$(call import-module,nv_log)
$(call import-module,nv_shader)
$(call import-module,nv_file)
$(call import-module,nv_thread)
*/
	}
	writeFile(to.resolve(Paths::get("jni", "Android.mk")));
	p("LOCAL_PATH := $(call my-dir)");
	p();
	p("include $(CLEAR_VARS)");
	p();
	p("LOCAL_MODULE    := Kore");
	std::string files = "";
	for (std::string filename : project->getFiles()) if (endsWith(filename, ".c") || endsWith(filename, ".cpp") || endsWith(filename, ".cc") || endsWith(filename, ".s")) files += filename + " ";
	p("LOCAL_SRC_FILES := " + files);
	std::string defines = "";
	for (std::string def : project->getDefines()) defines += "-D" + replace(def, '\"', "\\\"") + " ";
	p("LOCAL_CFLAGS := " + defines);
	std::string includes = "";
	for (std::string inc : project->getIncludeDirs()) includes += "$(LOCAL_PATH)/" + inc + " ";
	p("LOCAL_C_INCLUDES := " + includes);
	p("LOCAL_LDLIBS    := -llog -lGLESv2");
	p();
	p("include $(BUILD_SHARED_LIBRARY)");
	p();
	closeFile();

	/*
	writeFile(to.resolve(Paths::get("jni", "Application.mk")));
	p("APP_CPPFLAGS += -fexceptions -frtti");
	p("APP_STL := gnustl_static");
	//p("APP_ABI := all");
	p("APP_ABI := armeabi-v7a");
	//p("LOCAL_ARM_NEON := true");
	if (nvpack) {
		//APP_ABI := armeabi-v7a
		//APP_PLATFORM := android-10
	}
	closeFile();
	*/
	Files::copy(Paths::executableDir().resolve(Paths::get("Data", "android", "Application.mk")), to.resolve(Paths::get("jni", "Application.mk")), true);

	for (auto file : project->getFiles()) {
		Path target = to.resolve("jni").resolve(file);
		createDirectory(Paths::get(target.path.substr(0, lastIndexOf(target.path, '/'))));
		Files::copy(from.resolve(file), target, true);
	}
}
