#pragma once

#include <fstream>
#include <string>

namespace kake {
	class Block {
	protected:
		std::ofstream* out;
	private:
		int indentation;
	public:
		Block(std::ofstream* out, int indentation) : out(out), indentation(indentation) {
			
		}
	
		void indent() {
			++indentation;
		}
	
		void unindent() {
			--indentation;
		}
	
		void tag(std::string name, std::string value) {
			p("<" + name + ">" + value + "</" + name + ">");
		}
	
		void tagStart(std::string name) {
			p("<" + name + ">");
			indent();
		}
	
		void tagEnd(std::string name) {
			unindent();
			p("</" + name + ">");
		}
	
		void p(std::string line) {
			std::string tabs = "";
			for (int i = 0; i < indentation; ++i) tabs += "\t";
			*out << tabs << line << "\n";
		}
	};
}
