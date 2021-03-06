#include "ExporterEmscripten.h"
#include "Files.h"
#include "Project.h"
#include "Solution.h"
#include "StringHelper.h"
#include <iostream>

using namespace kmd;
using namespace kake;

namespace {
	void createDirectory(Path dir) {
		if (!Files::exists(dir)) Files::createDirectories(dir);
	}

	void copyDirectory(Path from, Path to, std::vector<std::string>& assets) {
		createDirectory(to);
		for (auto path : Files::newDirectoryStream(from)) {
			if (Files::isDirectory(path)) {
				copyDirectory(path, to.resolve(from.relativize(path)), assets);
			}
			else {
				Path aim = to.resolve(from.relativize(path));
				assets.push_back(Paths::get("build").relativize(aim).toString());
				Files::copy(path, aim, true);

			}
		}
	}
}

void ExporterEmscripten::execute(std::string exe) {
	/*try {
		Process p = Runtime.getRuntime().exec(exe);
		InputStream stderr = p.getErrorStream();
		InputStreamReader isr = new InputStreamReader(stderr);
		BufferedReader br = new BufferedReader(isr);
		String line = null;
		while ((line = br.readLine()) != null) Log.the().log(line);
		p.waitFor();
	}
	catch (Exception ex) {
		ex.printStackTrace();
	}*/
}

void ExporterEmscripten::compile(std::string inFilename, std::string outFilename) {
	if (Files::exists(Paths::get(outFilename))) return;
	//if (!new File(outFilename).getParentFile().exists()) new File(outFilename).getParentFile().mkdirs();
	std::string exe = "/Users/robert/Projekte/emscripten/emcc " + inFilename + " " + includes + " " + defines + " -c -o " + outFilename;
	std::cout << "Compiling " << inFilename << std::endl;
	execute(exe);
}

void ExporterEmscripten::link(std::vector<std::string> files, Path output) {
	std::string exe = "/Users/robert/Projekte/emscripten/emcc ";//-O2 ";
	for (std::string file : files) exe += file + " ";
	exe += "-o " + output.toString();
	std::cout << "Linking " << output.toString() << std::endl;
	execute(exe);
}

void ExporterEmscripten::exportSolution(Solution* solution, Path from, Path to, Platform platform) {
	Project* project = solution->getProjects()[0];

	std::vector<std::string> assets;
	copyDirectory(from.resolve(project->getDebugDir()), to, assets);

	defines = "";
	for (std::string def : project->getDefines()) defines += "-D" + def + " ";
	includes = "";
	for (std::string inc : project->getIncludeDirs()) includes += "-I../" + from.resolve(inc).toString() + " ";

	writeFile(to.resolve("makefile"));

	//p("CC = ~/Projekte/emscripten/emcc");
	p();
	std::string oline;
	for (std::string filename : project->getFiles()) {
		if (!endsWith(filename, ".cpp") && !endsWith(filename, ".c")) continue;
		auto lastpoint = filename.find_last_of('.');
		std::string oname = filename.substr(0, lastpoint) + ".o";
		oname = replace(oname, "../", "");
		oline += " " + oname;
	}
	std::string assetline;
	for (auto asset : assets) {
		assetline += " --preload-file " + asset;
	}
	p(std::string("kore.html:") + oline);
		p(std::string("$(CC) ") + oline + " -o kore.html" + assetline, 1);
	p();

	for (std::string filename : project->getFiles()) {
		if (!endsWith(filename, ".cpp") && !endsWith(filename, ".c")) continue;
		Path builddir = to;
		std::vector<std::string> dirs = split(filename, '/');
		std::string name;
		for (int i = 0; i < (int)dirs.size() - 1; ++i) {
			std::string s = dirs[i];
			if (s == "" || s == "..") continue;
			name += s + "/";
			builddir = builddir.resolve(s);
			if (!Files::exists(builddir)) Files::createDirectories(builddir);
		}
		auto lastpoint = filename.find_last_of('.');
		std::string oname = filename.substr(0, lastpoint) + ".o";
		oname = replace(oname, "../", "");
		p(oname + ": ../" + filename);
			p(std::string("$(CC) -c ../") + filename + " " + includes + " " + defines + " -o " + oname, 1);
	}

	closeFile();

	/*
	std::vector<std::string> objectFiles;
	for (std::string filename : project->getFiles()) if (endsWith(filename, ".c") || endsWith(filename, ".cpp")) {
		//files += "../../" + filename + " ";
		compile(directory.resolve(filename).toString(), directory.resolve(Paths::get("build", filename + ".o")).toString());
		objectFiles.push_back(directory.resolve(Paths::get("build", filename + ".o")).toString());
	}
	link(objectFiles, directory.resolve(Paths::get("build", "Kt.js")));*/
}
