#include "ExporterXCode.h"
#include "Ball.h"
#include "Files.h"
#include "Project.h"
#include "Solution.h"
#include "String.h"
#include <stdexcept>

using namespace kake;

Directory::Directory(std::string dirname) : dirname(dirname) {
	id = ExporterXCode::newId();
}

std::string Directory::getName() {
	return dirname;
}

std::string Directory::getLastName() {
	if (!contains(dirname, '/')) return dirname;
	return dirname.substr(lastIndexOf(dirname, '/') + 1);
}

std::string Directory::getId() {
	return id;
}

File::File(std::string filename, Directory* dir) : filename(filename), dir(dir) {
	buildid = ExporterXCode::newId();
	fileid = ExporterXCode::newId();
}

std::string File::getBuildId() {
	return buildid;
}

std::string File::getFileId() {
	return fileid;
}

bool File::isBuildFile() {
	return endsWith(filename, ".c") || endsWith(filename, ".cpp") || endsWith(filename, ".m") || endsWith(filename, ".mm") || endsWith(filename, ".cc") || endsWith(filename, ".s");
}

std::string File::getName() {
	return filename;
}

std::string File::getLastName() {
	if (!contains(filename, '/')) return filename;
	return filename.substr(lastIndexOf(filename, '/') + 1);
}

Directory* File::getDir() {
	return dir;
}

std::string File::toString() {
	return getName();
}

Framework::Framework(std::string name) : name(name) {
	buildid = UUID::randomUUID();
	fileid = UUID::randomUUID();
}

std::string Framework::toString() {
	return name + ".framework";
}

std::string Framework::getBuildId() {
	return toUpperCase(buildid.toString());
}

std::string Framework::getFileId() {
	return toUpperCase(fileid.toString());
}

Directory* ExporterXCode::findDirectory(std::string dirname, std::vector<Directory*>& directories) {
	for (Directory* d : directories) {
		if (d->getName() == dirname) {
			return d;
		}
	}
	return nullptr;
}

Directory* ExporterXCode::addDirectory(std::string dirname, std::vector<Directory*>& directories) {
	Directory* dir = findDirectory(dirname, directories);
	if (dir == nullptr) {
		dir = new Directory(dirname);
		directories.push_back(dir);
		while (contains(dirname, '/')) {
			dirname = dirname.substr(0, lastIndexOf(dirname, '/'));
			addDirectory(dirname, directories);
		}
	}
	return dir;
}

void ExporterXCode::exportWorkspace(Path to, Solution* solution) {
	Path dir = to.resolve(Paths::get(solution->getName() + ".xcodeproj", "project.xcworkspace"));
	if (!Files::exists(dir)) Files::createDirectories(dir);

	writeFile(to.resolve(Paths::get(solution->getName() + ".xcodeproj", "project.xcworkspace", "contents.xcworkspacedata")));

	p("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	p("<Workspace");
		p("version = \"1.0\">");
		p("<FileRef");
			p("location = \"self:" + solution->getName() + ".xcodeproj\">");
		p("</FileRef>");
	p("</Workspace>");

	closeFile();
}

std::string ExporterXCode::newId() {
	return toUpperCase(UUID::randomUUID().toString());
}

void ExporterXCode::exportSolution(Solution* solution, Path from, Path to, Platform platform) {
	Path xdir = to.resolve(solution->getName() + ".xcodeproj");
	if (!Files::exists(xdir)) Files::createDirectories(xdir);

	exportWorkspace(to, solution);

	std::vector<std::string> iosIconNames;
	iosIconNames.push_back("iPhone.png");
	iosIconNames.push_back("iPhoneRetina.png");
	iosIconNames.push_back("Default.png");
	iosIconNames.push_back("Default@2x.png");
	iosIconNames.push_back("Default-568h@2x.png");
	iosIconNames.push_back("iPad.png");
	iosIconNames.push_back("iPadRetina.png");
	iosIconNames.push_back("Default-Portrait~ipad.png");
	iosIconNames.push_back("Default-Portrait@2x~ipad.png");
	iosIconNames.push_back("Default-Landscape~ipad.png");
	iosIconNames.push_back("Default-Landscape@2x~ipad.png");

	if (platform == iOS) {
		Ball::the()->exportTo(to.resolve("iPhone.png"                   ),   57,   57, transparent, from);
		Ball::the()->exportTo(to.resolve("iPhoneRetina.png"             ),  114,  114, transparent, from);
		Ball::the()->exportTo(to.resolve("Default.png"                  ),  320,  480, black,       from);
		Ball::the()->exportTo(to.resolve("Default@2x.png"               ),  640,  960, black,       from);
		Ball::the()->exportTo(to.resolve("Default-568h@2x.png"          ),  640, 1136, black,       from);
		Ball::the()->exportTo(to.resolve("iPad.png"                     ),   72,   72, transparent, from);
		Ball::the()->exportTo(to.resolve("iPadRetina.png"               ),  144,  144, transparent, from);
		Ball::the()->exportTo(to.resolve("Default-Portrait~ipad.png"    ),  768, 1024, black,       from);
		Ball::the()->exportTo(to.resolve("Default-Portrait@2x~ipad.png" ), 1536, 2048, black,       from);
		Ball::the()->exportTo(to.resolve("Default-Landscape~ipad.png"   ), 1024,  768, black,       from);
		Ball::the()->exportTo(to.resolve("Default-Landscape@2x~ipad.png"), 2048, 1536, black,       from);
	}
	else {
		Ball::the()->exportToMacIcon(to.resolve("icon.icns"), from);
	}

	Project* project = solution->getProjects()[0];
	std::string plistname;
	std::vector<File*> files;
	std::vector<Directory*> directories;
	for (std::string filename : project->getFiles()) {
		if (endsWith(filename, ".plist")) plistname = filename;

		std::string dirname;
		if (contains(filename, '/')) dirname = solution->getName() + "/" + filename.substr(0, lastIndexOf(filename, '/'));
		else dirname = solution->getName();
		Directory* dir = addDirectory(dirname, directories);

		File* file = new File(filename, dir);
		files.push_back(file);
	}
	if (plistname.size() == 0) throw std::runtime_error("no plist found");

	std::vector<Framework*> frameworks;
	for (std::string lib : project->getLibs()) {
		frameworks.push_back(new Framework(lib));
	}

	std::string projectId = newId();
	std::string appFileId = newId();
	std::string frameworkBuildId = newId();
	std::string sourceBuildId = newId();
	std::string frameworksGroupId = newId();
	std::string productsGroupId = newId();
	std::string mainGroupId = newId();
	std::string targetId = newId();
	std::string nativeBuildConfigListId = newId();
	std::string projectBuildConfigListId = newId();
	std::string debugId = newId();
	std::string releaseId = newId();
	std::string nativeDebugId = newId();
	std::string nativeReleaseId = newId();
	std::string debugDirFileId = newId();
	std::string debugDirBuildId = newId();
	std::string resourcesBuildId = newId();
	std::string iconFileId = newId();
	std::string iconBuildId = newId();
	std::vector<std::string> iosIconFileIds;
	std::vector<std::string> iosIconBuildIds;
	for (unsigned i = 0; i < iosIconNames.size(); ++i) {
		iosIconFileIds.push_back(newId());
		iosIconBuildIds.push_back(newId());
	}

	writeFile(to.resolve(Paths::get(solution->getName() + ".xcodeproj", "project.pbxproj")));

	p("// !$*UTF8*$!");
	p("{");
	p("archiveVersion = 1;", 1);
	p("classes = {", 1);
	p("};", 1);
	p("objectVersion = 46;", 1);
	p("objects = {", 1);
	p();
	p("/* Begin PBXBuildFile section */");
	for (Framework* framework : frameworks)
		p(framework->getBuildId() + " /* " + framework->toString() + " in Frameworks */ = {isa = PBXBuildFile; fileRef = " + framework->getFileId() + " /* " + framework->toString() + " */; };", 2);
	p(debugDirBuildId + " /* Deployment in Resources */ = {isa = PBXBuildFile; fileRef = " + debugDirFileId + " /* Deployment */; };", 2);
	for (File* file : files)
		if (file->isBuildFile())
			p(file->getBuildId() + " /* " + file->toString() + " in Sources */ = {isa = PBXBuildFile; fileRef = " + file->getFileId() + " /* " + file->toString() + " */; };", 2);
	if (platform == OSX)
		p(iconBuildId + " /* icon.icns in Resources */ = {isa = PBXBuildFile; fileRef = " + iconFileId + " /* icon.icns */; };", 2);
	else {
		for (unsigned i = 0; i < iosIconNames.size(); ++i) {
			p(iosIconBuildIds[i] + " /* " + iosIconNames[i] + " in Resources */ = {isa = PBXBuildFile; fileRef = " + iosIconFileIds[i] + " /* " + iosIconNames[i] + " */; };", 2);
		}
	}
	p("/* End PBXBuildFile section */");
	p();
	p("/* Begin PBXFileReference section */");
	p(appFileId + " /* " + solution->getName() + ".app */ = {isa = PBXFileReference; explicitFileType = wrapper.application; includeInIndex = 0; path = " + solution->getName() + ".app; sourceTree = BUILT_PRODUCTS_DIR; };", 2);

	for (Framework* framework : frameworks)
		p(framework->getFileId() + " /* " + framework->toString() + " */ = {isa = PBXFileReference; lastKnownFileType = wrapper.framework; name = " + framework->toString() + "; path = System/Library/Frameworks/" + framework->toString() + "; sourceTree = SDKROOT; };", 2);
	p(debugDirFileId + " /* Deployment */ = {isa = PBXFileReference; lastKnownFileType = folder; name = Deployment; path = \"" + from.resolve(project->getDebugDir()).toAbsolutePath().toString() + "\"; sourceTree = \"<group>\"; };", 2);
	for (File* file : files) {
		std::string filetype = "unknown";
		if (endsWith(file->getName(), ".plist")) filetype = "text.plist.xml";
		if (endsWith(file->getName(), ".h")) filetype = "sourcecode.c.h";
		if (endsWith(file->getName(), ".m")) filetype = "sourcecode.c.objc";
		if (endsWith(file->getName(), ".c")) filetype = "sourcecode.c.c";
		if (endsWith(file->getName(), ".cpp")) filetype = "sourcecode.c.cpp";
		if (endsWith(file->getName(), ".cc")) filetype = "sourcecode.c.cpp";
		if (endsWith(file->getName(), ".mm")) filetype = "sourcecode.c.objcpp";
		if (endsWith(file->getName(), ".s")) filetype = "sourcecode.asm";
		p(file->getFileId() + " /* " + file->toString() + " */ = {isa = PBXFileReference; lastKnownFileType = " + filetype + "; name = \"" + file->getLastName() + "\"; path = \"" + from.resolve(file->toString()).toAbsolutePath().toString() + "\"; sourceTree = \"<group>\"; };", 2);
	}
	if (platform == OSX)
		p(iconFileId + " /* icon.icns */ = {isa = PBXFileReference; lastKnownFileType = image.icns; path = icon.icns; sourceTree = \"<group>\"; };", 2);
	else {
		for (unsigned i = 0; i < iosIconNames.size(); ++i) {
			p(iosIconFileIds[i] + " /* " + iosIconNames[i] + " */ = {isa = PBXFileReference; lastKnownFileType = image.icns; path = \"" + iosIconNames[i] + "\"; sourceTree = \"<group>\"; };", 2);
		}
	}
	p("/* End PBXFileReference section */");
	p();
	p("/* Begin PBXFrameworksBuildPhase section */");
	p(frameworkBuildId + " /* Frameworks */ = {", 2);
		p("isa = PBXFrameworksBuildPhase;", 3);
		p("buildActionMask = 2147483647;", 3);
		p("files = (", 3);
			for (Framework* framework : frameworks)
				p(framework->getBuildId() + " /* " + framework->toString() + " in Frameworks */,", 4);
		p(");", 3);
		p("runOnlyForDeploymentPostprocessing = 0;", 3);
	p("};", 2);
	p("/* End PBXFrameworksBuildPhase section */");
	p();
	p("/* Begin PBXGroup section */");
	p(mainGroupId + " = {", 2);
		p("isa = PBXGroup;", 3);
		p("children = (", 3);
			if (platform == OSX) p(iconFileId + " /* icon.icns */,", 4);
			else {
				for (unsigned i = 0; i < iosIconNames.size(); ++i) {
					p(iosIconFileIds[i] + " /* " + iosIconNames[i] + " */,", 4);
				}
			}
			p(debugDirFileId + " /* Deployment */,", 4);
			//p(solutionGroupId + " /* " + solution.getName() + " */,", 4);
			for (Directory* dir : directories) {
				if (!contains(dir->getName(), '/')) p(dir->getId() + " /* " + dir->getName() + " */,", 4);
			}
			p(frameworksGroupId + " /* Frameworks */,", 4);
			p(productsGroupId + " /* Products */,", 4);
		p(");", 3);
		p("sourceTree = \"<group>\";", 3);
	p("};", 2);
	p(productsGroupId + " /* Products */ = {", 2);
		p("isa = PBXGroup;", 3);
		p("children = (", 3);
			p(appFileId + " /* " + solution->getName() + ".app */,", 4);
		p(");", 3);
		p("name = Products;", 3);
		p("sourceTree = \"<group>\";", 3);
	p("};", 2);
	p(frameworksGroupId + " /* Frameworks */ = {", 2);
		p("isa = PBXGroup;", 3);
		p("children = (", 3);
			for (Framework* framework : frameworks)
				p(framework->getFileId() + " /* " + framework->toString() + " */,", 4);
		p(");", 3);
		p("name = Frameworks;", 3);
		p("sourceTree = \"<group>\";", 3);
	p("};", 2);
	for (Directory* dir : directories) {
		p(dir->getId() + " /* " + dir->getName() + " */ = {", 2);
			p("isa = PBXGroup;", 3);
			p("children = (", 3);
				for (Directory* dir2 : directories) {
					if (dir2 == dir) continue;
					if (startsWith(dir2->getName(), dir->getName())) {
						if (!contains(dir2->getName().substr(dir->getName().size() + 1), '/'))
							p(dir2->getId() + " /* " + dir2->getName() + " */,", 4);
					}
				}
				for (File* file : files) if (file->getDir() == dir) p(file->getFileId() + " /* " + file->toString() + " */,", 4);
			p(");", 3);
			if (!contains(dir->getName(), '/')) {
				p("path = ../;", 3);
				p("name = " + dir->getLastName() + ";", 3);
			}
			else p("name = " + dir->getLastName() + ";", 3);
			p("sourceTree = \"<group>\";", 3);
		p("};", 2);
	}
	p("/* End PBXGroup section */");
	p();
	p("/* Begin PBXNativeTarget section */");
	p(targetId + " /* " + solution->getName() + " */ = {", 2);
		p("isa = PBXNativeTarget;", 3);
		p("buildConfigurationList = " + nativeBuildConfigListId + " /* Build configuration list for PBXNativeTarget \"" + solution->getName() + "\" */;", 3);
		p("buildPhases = (", 3);
			p(sourceBuildId + " /* Sources */,", 4);
			p(frameworkBuildId + " /* Frameworks */,", 4);
			p(resourcesBuildId + " /* Resources */,", 4);
		p(");", 3);
		p("buildRules = (", 3);
		p(");", 3);
		p("dependencies = (", 3);
		p(");", 3);
		p("name = " + solution->getName() + ";", 3);
		p("productName = " + solution->getName() + ";", 3);
		p("productReference = " + appFileId + " /* " + solution->getName() + ".app */;", 3);
		p(std::string("productType = \"com.apple.product-type.") + (solution->isCmd() ? "tool" : "application") + "\";", 3);
	p("};", 2);
	p("/* End PBXNativeTarget section */");
	p();
	p("/* Begin PBXProject section */");
	p(projectId + " /* Project object */ = {", 2);
		p("isa = PBXProject;", 3);
		p("attributes = {", 3);
			p("LastUpgradeCheck = 0430;", 4);
			p("ORGANIZATIONNAME = \"KTX Software Development\";", 4);
		p("};", 3);
		p("buildConfigurationList = " + projectBuildConfigListId + " /* Build configuration list for PBXProject \"" + solution->getName() + "\" */;", 3);
		p("compatibilityVersion = \"Xcode 3.2\";", 3);
		p("developmentRegion = English;", 3);
		p("hasScannedForEncodings = 0;", 3);
		p("knownRegions = (", 3);
			p("en,", 4);
		p(");", 3);
		p("mainGroup = " + mainGroupId + ";", 3);
		p("productRefGroup = " + productsGroupId + " /* Products */;", 3);
		p("projectDirPath = \"\";", 3);
		p("projectRoot = \"\";", 3);
		p("targets = (", 3);
			p(targetId + " /* " + solution->getName() + " */,", 4);
		p(");", 3);
	p("};", 2);
	p("/* End PBXProject section */");
	p();
	p("/* Begin PBXResourcesBuildPhase section */");
	p(resourcesBuildId + " /* Resources */ = {", 2);
		p("isa = PBXResourcesBuildPhase;", 3);
		p("buildActionMask = 2147483647;", 3);
		p("files = (", 3);
			p(debugDirBuildId + " /* Deployment in Resources */,", 4);
			if (platform == OSX) p(iconBuildId + " /* icon.icns in Resources */,", 4);
			else {
				for (unsigned i = 0; i < iosIconNames.size(); ++i) {
					p(iosIconBuildIds[i] + " /* " + iosIconNames[i] + " in Resources */,", 4);
				}
			}
		p(");", 3);
		p("runOnlyForDeploymentPostprocessing = 0;", 3);
	p("};", 2);
	p("/* End PBXResourcesBuildPhase section */");
	p();
	p("/* Begin PBXSourcesBuildPhase section */");
	p(sourceBuildId + " /* Sources */ = {", 2);
		p("isa = PBXSourcesBuildPhase;", 3);
		p("buildActionMask = 2147483647;", 3);
		p("files = (", 3);
			for (File* file : files)
				if (file->isBuildFile())
					p(file->getBuildId() + " /* " + file->toString() + " in Sources */,", 4);
		p(");", 3);
		p("runOnlyForDeploymentPostprocessing = 0;");
	p("};");
	p("/* End PBXSourcesBuildPhase section */");
	p();
	//p("/* Begin PBXVariantGroup section */");
	//p("E1FC77F013DAA40000D635AE /* InfoPlist.strings */ = {", 2);
	//	p("isa = PBXVariantGroup;", 3);
	//	p("children = (", 3);
	//		p("E1FC77F113DAA40000D635AE /* en */,", 4);
	//	p(");", 3);
	//	p("name = InfoPlist.strings;", 3);
	//	p("sourceTree = \"<group>\";", 3);
	//p("};", 2);
	//p("E1FC77F913DAA40000D635AE /* MainWindow.xib */ = {", 2);
	//	p("isa = PBXVariantGroup;", 3);
	//	p("children = (", 3);
	//		p("E1FC77FA13DAA40000D635AE /* en */,", 4);
	//	p(");", 3);
	//	p("name = MainWindow.xib;", 3);
	//	p("sourceTree = \"<group>\";", 3);
	//p("};", 2);
	//p("E1FC780613DAA40000D635AE /* TestViewController.xib */ = {", 2);
	//	p("isa = PBXVariantGroup;", 3);
	//	p("children = (", 3);
	//		p("E1FC780713DAA40000D635AE /* en */,", 4);
	//	p(");", 3);
	//	p("name = TestViewController.xib;", 3);
	//	p("sourceTree = \"<group>\";", 3);
	//p("};", 2);
	//p("/* End PBXVariantGroup section */");
	//p();
	p("/* Begin XCBuildConfiguration section */");
	p(debugId + " /* Debug */ = {", 2);
		p("isa = XCBuildConfiguration;", 3);
		p("buildSettings = {", 3);
			p("ALWAYS_SEARCH_USER_PATHS = NO;", 4);
			if (platform == iOS) {
				p("CLANG_CXX_LANGUAGE_STANDARD = \"gnu++0x\";", 4);
				p("CLANG_CXX_LIBRARY = \"libc++\";", 4);
				p("CLANG_WARN_EMPTY_BODY = YES;", 4);
				p("CLANG_WARN__DUPLICATE_METHOD_MATCH = YES;", 4);
			}
			else p("ARCHS = \"$(ARCHS_STANDARD_64_BIT)\";", 4);
			//p("CLANG_ENABLE_OBJC_ARC = YES;", 4); //default, but incompatible with MacOS < 10.6 and iOS < 4.0
			if (platform == iOS) p("\"CODE_SIGN_IDENTITY[sdk=iphoneos*]\" = \"iPhone Developer\";", 4);
			p("COPY_PHASE_STRIP = NO;", 4);
			p("GCC_C_LANGUAGE_STANDARD = gnu99;", 4);
			p("GCC_DYNAMIC_NO_PIC = NO;", 4);
			if (platform == OSX) p("GCC_ENABLE_OBJC_EXCEPTIONS = YES;", 4);
			p("GCC_OPTIMIZATION_LEVEL = 0;", 4);
			p("GCC_PREPROCESSOR_DEFINITIONS = (", 4);
				p("\"DEBUG=1\",", 5);
				for (std::string define : project->getDefines())
					if (contains(define, '=')) p("\"" + replace(define, '\"', "\\\\\\\"") + "\",", 5);
					else p(define + ",", 5);
				p("\"$(inherited)\",", 5);
			p(");", 4);
			p("GCC_SYMBOLS_PRIVATE_EXTERN = NO;", 4);
			p("GCC_VERSION = com.apple.compilers.llvm.clang.1_0;", 4);
			if (platform == OSX) p("GCC_WARN_64_TO_32_BIT_CONVERSION = YES;", 4);
			p("GCC_WARN_ABOUT_RETURN_TYPE = YES;", 4);
			p("GCC_WARN_UNINITIALIZED_AUTOS = YES;", 4);
			p("GCC_WARN_UNUSED_VARIABLE = YES;", 4);
			if (platform == iOS) {
				p("IPHONEOS_DEPLOYMENT_TARGET = 5.0;", 4);
				p("ONLY_ACTIVE_ARCH = YES;", 4);
				p("SDKROOT = iphoneos;", 4);
				p("TARGETED_DEVICE_FAMILY = \"1,2\";", 4);
			}
			else {
				p("MACOSX_DEPLOYMENT_TARGET = 10.5;", 4);
				p("ONLY_ACTIVE_ARCH = YES;", 4);
				p("SDKROOT = macosx;", 4);
			}
		p("};", 3);
		p("name = Debug;", 3);
	p("};", 2);
	p(releaseId + " /* Release */ = {", 2);
		p("isa = XCBuildConfiguration;", 3);
		p("buildSettings = {", 3);
			p("ALWAYS_SEARCH_USER_PATHS = NO;", 4);
			if (platform == iOS) {
				p("CLANG_CXX_LANGUAGE_STANDARD = \"gnu++0x\";", 4);
				p("CLANG_CXX_LIBRARY = \"libc++\";", 4);
				p("CLANG_WARN_EMPTY_BODY = YES;", 4);
				p("CLANG_WARN__DUPLICATE_METHOD_MATCH = YES;", 4);
			}
			else p("ARCHS = \"$(ARCHS_STANDARD_64_BIT)\";", 4);
			//p("CLANG_ENABLE_OBJC_ARC = YES;", 4); //default, but incompatible with MacOS < 10.6 and iOS < 4.0
			if (platform == iOS) p("\"CODE_SIGN_IDENTITY[sdk=iphoneos*]\" = \"iPhone Developer\";", 4);
			p("COPY_PHASE_STRIP = YES;", 4);
			if (platform == OSX) p("DEBUG_INFORMATION_FORMAT = \"dwarf-with-dsym\";", 4);
			p("GCC_C_LANGUAGE_STANDARD = gnu99;", 4);
			if (platform == OSX) p("GCC_ENABLE_OBJC_EXCEPTIONS = YES;", 4);
			p("GCC_PREPROCESSOR_DEFINITIONS = (", 4);
			for (std::string define : project->getDefines())
				if (contains(define, '=')) p("\"" + replace(define, '\"', "\\\\\\\"") + "\",", 5);
				else p(define + ",", 5);
			p("\"$(inherited)\",", 5);
			p(");", 4);
			p("GCC_VERSION = com.apple.compilers.llvm.clang.1_0;", 4);
			if (platform == OSX) p("GCC_WARN_64_TO_32_BIT_CONVERSION = YES;", 4);
			p("GCC_WARN_ABOUT_RETURN_TYPE = YES;", 4);
			p("GCC_WARN_UNINITIALIZED_AUTOS = YES;", 4);
			p("GCC_WARN_UNUSED_VARIABLE = YES;", 4);
			if (platform == iOS) {
				p("IPHONEOS_DEPLOYMENT_TARGET = 5.0;", 4);
				p("OTHER_CFLAGS = \"-DNS_BLOCK_ASSERTIONS=1\";", 4);
				p("SDKROOT = iphoneos;", 4);
				p("TARGETED_DEVICE_FAMILY = \"1,2\";", 4);
				p("VALIDATE_PRODUCT = YES;", 4);
			}
			else {
				p("MACOSX_DEPLOYMENT_TARGET = 10.5;", 4);
				p("SDKROOT = macosx;", 4);
			}
		p("};", 3);
		p("name = Release;", 3);
	p("};", 2);
	p(nativeDebugId + " /* Debug */ = {", 2);
		p("isa = XCBuildConfiguration;", 3);
		p("buildSettings = {", 3);
			//p("GCC_PRECOMPILE_PREFIX_HEADER = YES;", 4);
			//p("GCC_PREFIX_HEADER = \"Test/Test-Prefix.pch\";", 4);
			if (platform == iOS) {
				p("\"GCC_PREPROCESSOR_DEFINITIONS[arch=*]\" = (", 4);
					p("\"DEBUG=1\",", 5);
					for (std::string define : project->getDefines())
						if (contains(define, '=')) p("\"" + replace(define, '\"', "\\\"") + "\",", 5);
						else p(define + ",", 5);
					p("\"$(inherited)\",", 5);
				p(");", 4);
				p("\"GCC_THUMB_SUPPORT[arch=armv6]\" = \"\";", 4);
			}
			p("HEADER_SEARCH_PATHS = (", 4);
				for (std::string path : project->getIncludeDirs()) p(from.resolve(path).toAbsolutePath().toString() + ",", 5);
			p(");", 4);
			p("INFOPLIST_FILE = \"" + from.resolve(plistname).toAbsolutePath().toString() + "\";", 4);
			p("PRODUCT_NAME = \"$(TARGET_NAME)\";", 4);
			p("WRAPPER_EXTENSION = app;", 4);
		p("};", 3);
		p("name = Debug;", 3);
	p("};", 2);
	p(nativeReleaseId + " /* Release */ = {", 2);
		p("isa = XCBuildConfiguration;", 3);
		p("buildSettings = {", 3);
			//p("GCC_PRECOMPILE_PREFIX_HEADER = YES;", 4);
			//p("GCC_PREFIX_HEADER = \"Test/Test-Prefix.pch\";", 4);
			if (platform == iOS) {
				p("\"GCC_PREPROCESSOR_DEFINITIONS[arch=*]\" = (", 4);
					for (std::string define : project->getDefines())
						if (contains(define, '=')) p("\"" + replace(define, '\"', "\\\"") + "\",", 5);
						else p(define + ",", 5);
					p("\"$(inherited)\",", 5);
				p(");", 4);
				p("\"GCC_THUMB_SUPPORT[arch=armv6]\" = \"\";", 4);
			}
			p("HEADER_SEARCH_PATHS = (", 4);
				for (std::string path : project->getIncludeDirs()) p(from.resolve(path).toAbsolutePath().toString() + ",", 5);
			p(");", 4);
			p("INFOPLIST_FILE = \"" + from.resolve(plistname).toAbsolutePath().toString() + "\";", 4);
			p("PRODUCT_NAME = \"$(TARGET_NAME)\";", 4);
			p("WRAPPER_EXTENSION = app;", 4);
		p("};", 3);
		p("name = Release;", 3);
	p("};", 2);
	p("/* End XCBuildConfiguration section */");
	p();
	p("/* Begin XCConfigurationList section */");
	p(projectBuildConfigListId + " /* Build configuration list for PBXProject \"" + solution->getName() + "\" */ = {", 2);
		p("isa = XCConfigurationList;", 3);
		p("buildConfigurations = (", 3);
			p(debugId + " /* Debug */,", 4);
			p(releaseId + " /* Release */,", 4);
		p(");", 3);
		p("defaultConfigurationIsVisible = 0;", 3);
		p("defaultConfigurationName = Release;", 3);
	p("};", 2);
	p(nativeBuildConfigListId + " /* Build configuration list for PBXNativeTarget \"" + solution->getName() + "\" */ = {", 2);
		p("isa = XCConfigurationList;", 3);
		p("buildConfigurations = (", 3);
			p(nativeDebugId + " /* Debug */,", 4);
			p(nativeReleaseId + " /* Release */,", 4);
		p(");", 3);
		p("defaultConfigurationIsVisible = 0;", 3);
	p("};", 2);
	p("/* End XCConfigurationList section */");
p("};", 1);
p("rootObject = " + projectId + " /* Project object */;", 1);
	p("}");
	closeFile();
}
