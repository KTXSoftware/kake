#pragma once

#include <string>

namespace kake {
	struct Path {
		Path resolve(std::string subpath);
		Path resolve(Path subpath);
		std::string toString();
		Path toAbsolutePath();
		bool startsWith(Path other);
		Path relativize(Path other);

		std::string path;
	};

	namespace Paths {
		Path get(std::string a);
		Path get(std::string a, std::string b);
		Path get(std::string a, std::string b, std::string c);
		Path get(std::string a, std::string b, std::string c, std::string d);
	}
}