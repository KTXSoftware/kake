#pragma once

#include "Exporter.h"

namespace kake {
	class ExporterCodeBlocks : public Exporter {
		void exportSolution(Solution* solution, Path from, Path to, Platform platform) override;
	};
}
