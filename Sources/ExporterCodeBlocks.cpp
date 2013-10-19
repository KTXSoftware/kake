#include "ExporterCodeBlocks.h"
#include "Files.h"
#include "Project.h"
#include "Solution.h"
#include "String.h"

using namespace kake;

void ExporterCodeBlocks::exportSolution(Solution* solution, Path from, Path to, Platform platform) {
	Project* project = solution->getProjects()[0];

	writeFile(to.resolve(project->getName() + ".cbp"));
	p("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>");
	p("<CodeBlocks_project_file>");
		p("<FileVersion major=\"1\" minor=\"6\" />", 1);
		p("<Project>", 1);
			p("<Option title=\"" + project->getName() + "\" />", 2);
			p("<Option pch_mode=\"2\" />", 2);
			p("<Option compiler=\"gcc\" />", 2);
			p("<Build>", 2);
				p("<Target title=\"Debug\">", 3);
					p("<Option output=\"bin/Debug/" + project->getName() + "\" prefix_auto=\"1\" extension_auto=\"1\" />", 4);
					if (project->getDebugDir().size() > 0) p(std::string("<Option working_dir=\"../") + project->getDebugDir() + "\" />", 4);
					p("<Option object_output=\"obj/Debug/\" />", 4);
					p("<Option type=\"1\" />", 4);
					p("<Option compiler=\"gcc\" />", 4);
					p("<Compiler>", 4);
						p("<Add option=\"-g\" />", 5);
					p("</Compiler>", 4);
				p("</Target>", 3);
				p("<Target title=\"Release\">", 3);
					p("<Option output=\"bin/Release/" + project->getName() + "\" prefix_auto=\"1\" extension_auto=\"1\" />", 4);
					if (project->getDebugDir().size() > 0) p(std::string("<Option working_dir=\"../") + project->getDebugDir() + "\" />", 4);
					p("<Option object_output=\"obj/Release/\" />", 4);
					p("<Option type=\"0\" />", 4);
					p("<Option compiler=\"gcc\" />", 4);
					p("<Compiler>", 4);
						p("<Add option=\"-O2\" />", 5);
					p("</Compiler>", 4);
					p("<Linker>", 4);
						p("<Add option=\"-s\" />", 5);
					p("</Linker>", 4);
				p("</Target>", 3);
			p("</Build>", 2);
			p("<Compiler>", 2);
				p("<Add option=\"-Wall\" />", 3);
				for (std::string def : project->getDefines()) {
					p("<Add option=\"-D" + replace(def, "\"", "\\\"") + "\" />", 3);
				}
				for (std::string inc : project->getIncludeDirs()) {
					p("<Add directory=\"../" + inc + "\" />", 3);
				}
			p("</Compiler>", 2);
			p("<Linker>", 2);
                p("<Add option=\"-pthread\" />", 3);
				p("<Add library=\"GL\" />", 3);
				p("<Add library=\"X11\" />", 3);
				p("<Add library=\"asound\" />", 3);
				p("<Add library=\"dl\" />", 3);
			p("</Linker>", 2);
			for (std::string file : project->getFiles()) {
				if (endsWith(file, ".c") || endsWith(file, ".cpp")) {
					p("<Unit filename=\"../" + file + "\">", 2);
						p("<Option compilerVar=\"CC\" />", 3);
					p("</Unit>", 2);
				}
			}
			p("<Extensions>", 2);
				p("<code_completion />", 3);
				p("<debugger />", 3);
			p("</Extensions>", 2);
		p("</Project>", 1);
	p("</CodeBlocks_project_file>");
	closeFile();
}
