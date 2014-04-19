#pragma once

#include "Exporter.h"

namespace kake {
	class ExporterCodeBlocks : public Exporter {
		void exportSolution(Solution* solution, kmd::Path from, kmd::Path to, Platform platform) override;
	};
}
