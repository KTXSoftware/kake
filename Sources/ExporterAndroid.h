#pragma once

#include "Exporter.h"

namespace kake {
	class ExporterAndroid : public Exporter {
		void createDirectory(Path dir);
		void exportSolution(Solution* solution, Path directory, Platform platform) override;
	};
}
