#pragma once

#include "Exporter.h"

namespace kake {
	class ExporterCodeBlocks : public Exporter {
		void exportSolution(Solution* solution, Path directory, Platform platform) override;
	};
}
