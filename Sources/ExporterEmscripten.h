#pragma once

#include "Exporter.h"
#include <vector>

namespace kake {
	class ExporterEmscripten : public Exporter {
	private:
		std::string defines;
		std::string includes;
	public:
		void execute(std::string exe);
		void compile(std::string inFilename, std::string outFilename);
		void link(std::vector<std::string> files, kmd::Path output);
		void exportSolution(Solution* solution, kmd::Path from, kmd::Path to, Platform platform) override;
	};
}
