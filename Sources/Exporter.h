#pragma once

#include "Path.h"
#include "Platform.h"
#include <fstream>

namespace kake {
	class Solution;

	class Exporter {
	public:
		virtual void exportSolution(Solution* solution, Path directory, Platform platform) = 0;
	protected:
		void writeFile(Path file);
		void closeFile();
		void p();
		void p(std::string line);
		void p(std::string line, int indent);
		
		std::ofstream* out;
	};
}