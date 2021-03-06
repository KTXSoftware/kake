#pragma once

#include "Path.h"
#include "Platform.h"
#include <fstream>

namespace kake {
	class Solution;

	class Exporter {
	public:
		virtual void exportSolution(Solution* solution, kmd::Path from, kmd::Path to, Platform platform) = 0;
	protected:
		void writeFile(kmd::Path file);
		void closeFile();
		void p();
		void p(std::string line);
		void p(std::string line, int indent);
		
		std::ofstream* out;
	};
}
