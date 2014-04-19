#pragma once

#include "Exporter.h"

namespace kake {
	class ExporterAndroid : public Exporter {
		void exportSolution(Solution* solution, kmd::Path from, kmd::Path to, Platform platform) override;
	};
}
