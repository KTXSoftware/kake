#pragma once

#include "Exporter.h"

namespace kake {
	class ExporterTizen : public Exporter {
		void exportSolution(Solution* solution, Path from, Path to, Platform platform) override;
	};
}
