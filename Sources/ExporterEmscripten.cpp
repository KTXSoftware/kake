#include "ExporterEmscripten.h"
#include "Files.h"
#include "Project.h"
#include "Solution.h"
#include "String.h"
#include <iostream>

using namespace kake;
	
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
	
void ExporterEmscripten::exportSolution(Solution* solution, Path directory, Platform platform) {
	Project* project = solution->getProjects()[0];
		
	defines = "";
	for (std::string def : project->getDefines()) defines += "-D" + def + " ";
	includes = "";
	for (std::string inc : project->getIncludeDirs()) includes += "-I" + directory.resolve(inc).toString() + " ";
	
	Files::createDirectories(directory.resolve("build"));
	std::vector<std::string> objectFiles;
	for (std::string filename : project->getFiles()) if (endsWith(filename, ".c") || endsWith(filename, ".cpp")) {
		//files += "../../" + filename + " ";
		compile(directory.resolve(filename).toString(), directory.resolve(Paths::get("build", filename + ".o")).toString());
		objectFiles.push_back(directory.resolve(Paths::get("build", filename + ".o")).toString());
	}
	link(objectFiles, directory.resolve(Paths::get("build", "Kt.js")));
}
