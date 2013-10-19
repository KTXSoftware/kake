#pragma once

#include "Exporter.h"
#include "UUID.h"
#include <string>
#include <vector>

namespace kake {
	class Directory {
	public:
		Directory(std::string dirname);
		std::string getName();
		std::string getLastName();
		std::string getId();
	private:
		std::string dirname;
		std::string id;
	};

	class File {
	public:
		File(std::string filename, Directory* dir);
		std::string getBuildId();
		std::string getFileId();
		bool isBuildFile();
		std::string getName();
		std::string getLastName();
		Directory* getDir();
		std::string toString();
	private:
		std::string filename;
		std::string buildid;
		std::string fileid;
		Directory* dir;
	};

	class Framework {
	public:
		Framework(std::string name);
		std::string toString();
		std::string getBuildId();
		std::string getFileId();
	private:
		std::string name;
		UUID buildid;
		UUID fileid;
	};

	class ExporterXCode : public Exporter {
	public:
		static Directory* findDirectory(std::string dirname, std::vector<Directory*>& directories);
		static Directory* addDirectory(std::string dirname, std::vector<Directory*>& directories);
		void exportWorkspace(Path directory, Solution* solution);
		static std::string newId();
		void exportSolution(Solution* solution, Path from, Path to, Platform platform) override;
	};
}
