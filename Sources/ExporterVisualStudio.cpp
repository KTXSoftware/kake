#include "ExporterVisualStudio.h"
#include "Ball.h"
#include "ClCompile.h"
#include "Files.h"
#include "Options.h"
#include "Project.h"
#include "Platform.h"
#include "Solution.h"
#include "StringHelper.h"
#include <algorithm>
#include <set>
#include <stdexcept>

namespace {
	std::vector<std::string> standardconfs;// = new String[]{"Debug", "Release"};
	std::vector<std::string> xboxconfs;// = new String[]{"CodeAnalysis", "Debug", "Profile_FastCap", "Profile", "Release_LTCG", "Release"};
	std::vector<std::string> windows8systems;// = new String[]{"ARM", "Win32", "x64"};
	std::vector<std::string> xboxsystems;// = new String[]{"Xbox 360"};
	std::vector<std::string> ps3systems;// = new String[]{"PS3"};
	std::vector<std::string> windowssystems;// = new String[]{"Win32", "x64"};
}

using namespace kake;

extern Path koreDir;

void ExporterVisualStudio::exportUserFile(Path from, Path to, Project* project, Platform platform) {
	if (project->getDebugDir() == "") return;

	writeFile(to.resolve(project->getName() + ".vcxproj.user"));

	p("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
	p("<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">");
	p("<PropertyGroup>", 1);
	if (platform == Platform::Windows) {
		p("<LocalDebuggerWorkingDirectory>" + replace(from.resolve(Paths::get(project->getDebugDir())).toAbsolutePath().toString(), '/', '\\') + "</LocalDebuggerWorkingDirectory>", 2);
		p("<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>", 2);
		//java.io.File baseDir = new File(project.getBasedir());
		//p("<LocalDebuggerCommandArguments>\"SOURCEDIR=" + baseDir.getAbsolutePath() + "\" \"KTSOURCEDIR=" + baseDir.getAbsolutePath() + "\\Kt\"</LocalDebuggerCommandArguments>", 2);
	}
	else if (platform == Platform::PlayStation3) {
		p("<LocalDebuggerFileServingDirectory>" + replace(from.resolve(Paths::get(project->getDebugDir())).toAbsolutePath().toString(), '/', '\\') + "</LocalDebuggerFileServingDirectory>", 2);
		p("<DebuggerFlavor>PS3Debugger</DebuggerFlavor>", 2);
	}
	p("</PropertyGroup>", 1);
	p("</Project>");

	closeFile();
}

void ExporterVisualStudio::writeProjectDeclarations(Project* project, UUID solutionUuid) {
	p("Project(\"{" + toUpperCase(solutionUuid.toString()) + "}\") = \"" + project->getName() + "\", \"" + project->getName() + ".vcxproj\", \"{" + toUpperCase(project->getUuid().toString()) + "}\"");
	if (project->getSubProjects().size() > 0) {
		p("ProjectSection(ProjectDependencies) = postProject", 1);
		for (Project* proj : project->getSubProjects()) p("{" + toUpperCase(proj->getUuid().toString()) + "} = {" + toUpperCase(proj->getUuid().toString()) + "}", 2);
		p("EndProjectSection", 1);
	}
	p("EndProject");
	for (Project* proj : project->getSubProjects()) writeProjectDeclarations(proj, solutionUuid);
}

std::vector<std::string> ExporterVisualStudio::getConfigs(Platform platform) {
	if (platform == Platform::Xbox360) return xboxconfs;
	else return standardconfs;
}

std::vector<std::string> ExporterVisualStudio::getSystems(Platform platform) {
	if (platform == Platform::WindowsRT) return windows8systems;
	if (platform == Platform::PlayStation3) return ps3systems;
	else if (platform == Platform::Xbox360) return xboxsystems;
	else return windowssystems;
}

std::string ExporterVisualStudio::GetSys(Platform platform) {
	return getSystems(platform)[0];
}

void ExporterVisualStudio::writeProjectBuilds(Project* project, Platform platform) {
	for (std::string config : getConfigs(platform)) {
		for (std::string system : getSystems(platform)) {
			p("{" + toUpperCase(project->getUuid().toString()) + "}." + config + "|" + system + ".ActiveCfg = " + config + "|" + system, 2);
			p("{" + toUpperCase(project->getUuid().toString()) + "}." + config + "|" + system + ".Build.0 = " + config + "|" + system, 2);
			if (platform == Platform::WindowsRT) {
				p("{" + toUpperCase(project->getUuid().toString()) + "}." + config + "|" + system + ".Deploy.0 = " + config + "|" + system, 2);
			}
		}
	}
	for (Project* proj : project->getSubProjects()) writeProjectBuilds(proj, platform);
}

void ExporterVisualStudio::exportSolution(Solution* solution, Path from, Path to, Platform platform) {
	standardconfs.push_back("Debug");
	standardconfs.push_back("Release");
	xboxconfs.push_back("CodeAnalysis");
	xboxconfs.push_back("Debug");
	xboxconfs.push_back("Profile_FastCap");
	xboxconfs.push_back("Profile");
	xboxconfs.push_back("Release_LTCG");
	xboxconfs.push_back("Release");
	windows8systems.push_back("ARM");
	windows8systems.push_back("Win32");
	windows8systems.push_back("x64");
	xboxsystems.push_back("Xbox 360");
	ps3systems.push_back("PS3");
	windowssystems.push_back("Win32");
	windowssystems.push_back("x64");

	writeFile(to.resolve(solution->getName() + ".sln"));

	if ((platform == Platform::WindowsRT || platform == Platform::Windows) && Options::getVisualStudioVersion() == VS2013) {
		p("Microsoft Visual Studio Solution File, Format Version 12.00");
		p("# Visual Studio 2013");
		p("VisualStudioVersion = 12.0.21005.1");
		p("MinimumVisualStudioVersion = 10.0.40219.1");
	}
	if (platform == Platform::WindowsRT || (platform == Platform::Windows && Options::getVisualStudioVersion() == VS2012)) {
		p("Microsoft Visual Studio Solution File, Format Version 12.00");
		p("# Visual Studio 2012");
	}
	else {
		p("Microsoft Visual Studio Solution File, Format Version 11.00");
		p("# Visual Studio 2010");
	}
	UUID solutionUuid = UUID::randomUUID();
	for (Project* project : solution->getProjects()) writeProjectDeclarations(project, solutionUuid);
	p("Global");
	p("GlobalSection(SolutionConfigurationPlatforms) = preSolution", 1);
	for (std::string config : getConfigs(platform)) {
		for (std::string system : getSystems(platform)) {
			p(config + "|" + system + " = " + config + "|" + system, 2);
		}
	}
	p("EndGlobalSection", 1);
	p("GlobalSection(ProjectConfigurationPlatforms) = postSolution", 1);
	for (Project* project : solution->getProjects()) writeProjectBuilds(project, platform);
	p("EndGlobalSection", 1);
	p("GlobalSection(SolutionProperties) = preSolution", 1);
	p("HideSolutionNode = FALSE", 2);
	p("EndGlobalSection", 1);
	p("EndGlobal");
	closeFile();

	for (Project* project : solution->getProjects()) {
		exportProject(from, to, project, platform, solution->isCmd());
		exportFilters(from, to, project, platform);
		exportUserFile(from, to, project, platform);
		if (platform == Platform::WindowsRT) {
			exportManifest(to, project);
			Ball::the()->exportTo(to.resolve("Logo.png"), 150, 150, white, from);
			Ball::the()->exportTo(to.resolve("SmallLogo.png"), 30, 30, white, from);
			Ball::the()->exportTo(to.resolve("SplashScreen.png"), 620, 300, white, from);
			Ball::the()->exportTo(to.resolve("StoreLogo.png"), 50, 50, white, from);
		}
		else if (platform == Platform::Windows) {
			exportResourceScript(to);
			Ball::the()->exportToWindowsIcon(to.resolve("icon.ico"), from);
		}
	}
}

void ExporterVisualStudio::exportManifest(Path to, Project* project) {
	writeFile(to.resolve("Package.appxmanifest"));

	p("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
	p("<Package xmlns=\"http://schemas.microsoft.com/appx/2010/manifest\">");
		p("<Identity Name=\"d01da498-a1da-4cd7-9092-65483ca1e410\" Publisher=\"CN=Robert\" Version=\"1.0.0.0\" />", 1);
		p("<Properties>", 1);
			p("<DisplayName>" + project->getName() + "</DisplayName>", 2);
			p("<PublisherDisplayName>KTX Software Development</PublisherDisplayName>", 2);
			p("<Logo>StoreLogo.png</Logo>", 2);
			p("<Description>" + project->getName() + "</Description>", 2);
		p("</Properties>", 1);
		p("<Prerequisites>", 1);
			p("<OSMinVersion>6.2</OSMinVersion>", 2);
			p("<OSMaxVersionTested>6.2</OSMaxVersionTested>", 2);
		p("</Prerequisites>", 1);
		p("<Resources>", 1);
			p("<Resource Language=\"x-generate\"/>", 2);
		p("</Resources>", 1);
		p("<Applications>", 1);
			p("<Application Id=\"App\" Executable=\"$targetnametoken$.exe\" EntryPoint=\"" + project->getName() + ".App\">", 2);
				p("<VisualElements DisplayName=\"" + project->getName() + "\" Logo=\"Logo.png\" SmallLogo=\"SmallLogo.png\" Description=\"" + project->getName() + "\" ForegroundText=\"dark\" BackgroundColor=\"#FFFFFF\">", 3);
					p("<DefaultTile ShowName=\"allLogos\" />", 4);
					p("<SplashScreen Image=\"SplashScreen.png\" />", 4);
				p("</VisualElements>", 3);
			p("</Application>", 2);
		p("</Applications>", 1);
		p("<Capabilities>", 1);
			p("<Capability Name=\"internetClient\" />", 2);
		p("</Capabilities>", 1);
	p("</Package>");

	closeFile();
}

void ExporterVisualStudio::exportResourceScript(Path to) {
	writeFile(to.resolve("resources.rc"));
	p("107       ICON         \"icon.ico\"");
	closeFile();
}

void ExporterVisualStudio::exportAssetPathFilter(Path assetPath, std::vector<std::string>& dirs, std::vector<std::string>& assets) {
	std::string dir = assetPath.toString();
	if (!contains(dirs, dir)) dirs.push_back(dir);
	for (auto path : Files::newDirectoryStream(assetPath)) {
		if (Files::isDirectory(path)) exportAssetPathFilter(path, dirs, assets);
		else assets.push_back(path.toString());
	}
}

void ExporterVisualStudio::exportFilters(Path from, Path to, Project* project, Platform platform) {
	for (Project* proj : project->getSubProjects()) exportFilters(from, to, proj, platform);

	writeFile(to.resolve(project->getName() + ".vcxproj.filters"));

	p("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
	p("<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">");

	std::sort(project->getFiles().begin(), project->getFiles().end());
	std::string lastdir = "";
	std::vector<std::string> dirs;
	for (std::string file : project->getFiles()) {
		if (contains(file, '/')) {
			std::string dir = file.substr(0, lastIndexOf(file, '/'));
			if (dir != lastdir) {
				std::string subdir = dir;
				while (contains(subdir, "/")) {
					subdir = subdir.substr(0, lastIndexOf(subdir, '/'));
					if (!contains(dirs, subdir)) dirs.push_back(subdir);
				}
				dirs.push_back(dir);
				lastdir = dir;
			}
		}
	}
	std::vector<std::string> assets;
	if (platform == Platform::WindowsRT) exportAssetPathFilter(from.resolve(project->getDebugDir()), dirs, assets);

	p("<ItemGroup>", 1);
	for (std::string dir : dirs) {
		p("<Filter Include=\"" + replace(dir, '/', '\\') + "\">", 2);
		p("<UniqueIdentifier>{" + toUpperCase(UUID::randomUUID().toString()) + "}</UniqueIdentifier>", 3);
		p("</Filter>", 2);
	}
	if (platform == Platform::WindowsRT) {
		p("<Filter Include=\"Package\">", 2);
		p("<UniqueIdentifier>{" + toUpperCase(UUID::randomUUID().toString()) + "}</UniqueIdentifier>", 3);
		p("</Filter>", 2);
	}
	p("</ItemGroup>", 1);

	if (platform == Platform::WindowsRT) {
		p("<ItemGroup>", 1);
		p("<AppxManifest Include=\"Package.appxmanifest\">", 2);
		p("<Filter>Package</Filter>", 3);
		p("</AppxManifest>", 2);
		p("</ItemGroup>", 1);

		p("<ItemGroup>", 1);
		p("<Image Include=\"Logo.png\">", 2);
		p("<Filter>Package</Filter>", 3);
		p("</Image>", 2);
		p("<Image Include=\"SmallLogo.png\">", 2);
		p("<Filter>Package</Filter>", 3);
		p("</Image>", 2);
		p("<Image Include=\"StoreLogo.png\">", 2);
		p("<Filter>Package</Filter>", 3);
		p("</Image>", 2);
		p("<Image Include=\"SplashScreen.png\">", 2);
		p("<Filter>Package</Filter>", 3);
		p("</Image>", 2);
		p("</ItemGroup>", 1);
	}

	lastdir = "";
	p("<ItemGroup>", 1);
	for (std::string file : project->getFiles()) {
		if (contains(file, '/')) {
			std::string dir = file.substr(0, lastIndexOf(file, '/'));
			if (dir != lastdir) lastdir = dir;
			if (endsWith(file, ".h")) {
				p(std::string("<ClInclude Include=\"") + from.resolve(file).toAbsolutePath().toString() + "\">", 2);
				p("<Filter>" + replace(dir, '/', '\\') + "</Filter>", 3);
				p("</ClInclude>", 2);
			}
		}
	}
	p("</ItemGroup>", 1);

	lastdir = "";
	p("<ItemGroup>", 1);
	for (std::string file : project->getFiles()) {
		if (contains(file, '/')) {
			std::string dir = file.substr(0, lastIndexOf(file, '/'));
			if (dir != lastdir) lastdir = dir;
			if (endsWith(file, ".cpp") || endsWith(file, ".c") || endsWith(file, "cc")) {
				p(std::string("<ClCompile Include=\"") + from.resolve(file).toAbsolutePath().toString() + "\">", 2);
				p("<Filter>" + replace(dir, '/', '\\') + "</Filter>", 3);
				p("</ClCompile>", 2);
			}
		}
	}
	p("</ItemGroup>", 1);

	lastdir = "";
	p("<ItemGroup>", 1);
	for (std::string file : project->getFiles()) {
		if (contains(file, "/")) {
			std::string dir = file.substr(0, lastIndexOf(file, '/'));
			if (dir != lastdir) lastdir = dir;
			if (endsWith(file, ".cg") || endsWith(file, ".hlsl")) {
				p(std::string("<CustomBuild Include=\"") + from.resolve(file).toAbsolutePath().toString() + "\">", 2);
				p("<Filter>" + replace(dir, '/', '\\') + "</Filter>", 3);
				p("</CustomBuild>", 2);
			}
		}
	}
	p("</ItemGroup>", 1);

	lastdir = "";
	p("<ItemGroup>", 1);
	for (std::string file : project->getFiles()) {
		if (contains(file, "/")) {
			std::string dir = file.substr(0, lastIndexOf(file, '/'));
			if (dir != lastdir) lastdir = dir;
			if (endsWith(file, ".asm")) {
				p(std::string("<CustomBuild Include=\"") + from.resolve(file).toAbsolutePath().toString() + "\">", 2);
				p("<Filter>" + replace(dir, '/', '\\') + "</Filter>", 3);
				p("</CustomBuild>", 2);
			}
		}
	}
	p("</ItemGroup>", 1);

	if (platform == Platform::WindowsRT) {
		lastdir = "";
		p("<ItemGroup>", 1);
		for (std::string file : assets) {
			if (contains(file, "/")) {
				std::string dir = file.substr(0, lastIndexOf(file, '/'));
				if (dir != lastdir) lastdir = dir;
				p(std::string("<None Include=\"") + from.resolve(file).toAbsolutePath().toString() + "\">", 2);
				p("<Filter>" + replace(dir, '/', '\\') + "</Filter>", 3);
				p("</None>", 2);
			}
		}
		p("</ItemGroup>", 1);
	}

	if (platform == Platform::Windows) {
		p("<ItemGroup>", 1);
			p("<None Include=\"icon.ico\">", 2);
				p("<Filter>Ressourcendateien</Filter>", 3);
			p("</None>", 2);
		p("</ItemGroup>", 1);
		p("<ItemGroup>", 1);
			p("<ResourceCompile Include=\"resources.rc\">", 2);
				p("<Filter>Ressourcendateien</Filter>", 3);
			p("</ResourceCompile>", 2);
		p("</ItemGroup>", 1);
	}

	p("</Project>");
	closeFile();
}

void ExporterVisualStudio::addPropertyGroup(std::string buildType, Platform platform) {
	addPropertyGroup(buildType, false, platform);
}

void ExporterVisualStudio::addPropertyGroup(std::string buildType, bool wholeProgramOptimization, Platform platform) {
	p("<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='" + buildType + "|" + GetSys(platform) + "'\" Label=\"Configuration\">", 1);
	p("<ConfigurationType>Application</ConfigurationType>", 2);
	p(std::string("<WholeProgramOptimization>") + (wholeProgramOptimization ? "true" : "false") + "</WholeProgramOptimization>", 2);
	p("<CharacterSet>MultiByte</CharacterSet>", 2);
	p("</PropertyGroup>", 1);
}

void ExporterVisualStudio::addWin8PropertyGroup(bool debug, std::string platform) {
	p(std::string("<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='") + (debug ? "Debug" : "Release") + "|" + platform + "'\" Label=\"Configuration\">", 1);
	p("<ConfigurationType>Application</ConfigurationType>", 2);
	p(std::string("<UseDebugLibraries>") + (debug ? "true" : "false") + "</UseDebugLibraries>", 2);
	if (!debug) p("<WholeProgramOptimization>true</WholeProgramOptimization>", 2);
	p("<PlatformToolset>v110</PlatformToolset>", 2);
	p("</PropertyGroup>", 1);
}

namespace {
	Configuration valueOf(std::string string) {
		if (string == "Debug") return Debug;
		if (string == "CodeAnalysis") return CodeAnalysis;
		if (string == "Profile") return Profile;
		if (string == "Profile_FastCap") return Profile_FastCap;
		if (string == "Release") return Release;
		if (string == "Release_LTCG") return Release_LTCG;
		throw std::runtime_error("Unknown configuration");
	}
}

void ExporterVisualStudio::addItemDefinitionGroup(std::string incstring, std::string defines, std::string buildType, int warningLevel,
		bool prefast, bool optimization, bool functionLevelLinking, bool stringPooling, bool favorSize, bool release,
		bool profile, bool nocomdatfolding, bool ignoreXapilib, bool optimizeReferences, bool checksum, bool fastCap, bool comdatfolding, bool ltcg, Platform platform) {
	p("<ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='" + buildType + "|" + GetSys(platform) + "'\">", 1);

	ClCompile* compile = new ClCompile(out, 2, Platform::Xbox360, valueOf(buildType), split(incstring, ';'), split(defines, ';'));
	compile->print();

	p("<Link>", 2);
	p("<GenerateDebugInformation>true</GenerateDebugInformation>", 3);
	if (nocomdatfolding) p("<EnableCOMDATFolding>false</EnableCOMDATFolding>", 3);
	if (comdatfolding) p("<EnableCOMDATFolding>true</EnableCOMDATFolding>", 3);
	if (ignoreXapilib) p("<IgnoreSpecificDefaultLibraries>xapilib.lib</IgnoreSpecificDefaultLibraries>", 3);
	if (optimizeReferences) p("<OptimizeReferences>true</OptimizeReferences>", 3);
	p("<ProgramDatabaseFile>$(OutDir)$(ProjectName).pdb</ProgramDatabaseFile>", 3);
	if (checksum) p("<SetChecksum>true</SetChecksum>", 3);
	if (profile) p("<AdditionalDependencies>xapilibi.lib;d3d9i.lib;d3dx9.lib;xgraphics.lib;xboxkrnl.lib;xnet.lib;xaudio2.lib;xact3i.lib;x3daudioi.lib;xmcorei.lib;xbdm.lib;vcomp.lib</AdditionalDependencies>", 3);
	else if (ltcg) p("<AdditionalDependencies>xapilib.lib;d3d9ltcg.lib;d3dx9.lib;xgraphics.lib;xboxkrnl.lib;xnet.lib;xaudio2.lib;xact3ltcg.lib;x3daudioltcg.lib;xmcoreltcg.lib;vcomp.lib</AdditionalDependencies>", 3);
	else if (release) p("<AdditionalDependencies>xapilib.lib;d3d9.lib;d3dx9.lib;xgraphics.lib;xboxkrnl.lib;xnet.lib;xaudio2.lib;xact3.lib;x3daudio.lib;xmcore.lib;vcomp.lib</AdditionalDependencies>", 3);
	else p("<AdditionalDependencies>xapilibd.lib;d3d9d.lib;d3dx9d.lib;xgraphicsd.lib;xboxkrnl.lib;xnetd.lib;xaudiod2.lib;xactd3.lib;x3daudiod.lib;xmcored.lib;xbdm.lib;vcompd.lib</AdditionalDependencies>", 3);
	p("</Link>", 2);

	p("</ItemDefinitionGroup>", 1);
}

//private void addWinMD(String name) {
//	p("<Reference Include=\"" + name + ".winmd\">", 2);
//	p("<IsWinMDFile>true</IsWinMDFile>", 3);
//	p("</Reference>", 2);
//}

void ExporterVisualStudio::exportProject(Path from, Path to, Project* project, Platform platform, bool cmd) {
	for (Project* proj : project->getSubProjects()) exportProject(from, to, proj, platform, cmd);

	writeFile(to.resolve(project->getName() + ".vcxproj"));

	p("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
	if (Options::getVisualStudioVersion() == VS2013) p("<Project DefaultTargets=\"Build\" ToolsVersion=\"12.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">");
	else p("<Project DefaultTargets=\"Build\" ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">");
	p("<ItemGroup Label=\"ProjectConfigurations\">", 1);
	for (std::string system : getSystems(platform)) {
		for (std::string config : getConfigs(platform)) {
			p("<ProjectConfiguration Include=\"" + config + "|" + system + "\">", 2);
			p("<Configuration>" + config + "</Configuration>", 3);
			p("<Platform>" + system + "</Platform>", 3);
			p("</ProjectConfiguration>", 2);
		}
	}
	p("</ItemGroup>", 1);
	p("<PropertyGroup Label=\"Globals\">", 1);
	p(std::string("<ProjectGuid>{") + toUpperCase(project->getUuid().toString()) + "}</ProjectGuid>", 2);
	//p("<Keyword>Win32Proj</Keyword>", 2);
	//p("<RootNamespace>" + project.Name + "</RootNamespace>", 2);
	if (platform == Platform::WindowsRT) {
		p("<DefaultLanguage>en-US</DefaultLanguage>", 2);
		p("<MinimumVisualStudioVersion>11.0</MinimumVisualStudioVersion>", 2);
		p("<AppContainerApplication>true</AppContainerApplication>", 2);
	}
	p("</PropertyGroup>", 1);
	p("<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\" />", 1);
	if (platform == Platform::Xbox360) {
		addPropertyGroup("CodeAnalysis", platform);
		addPropertyGroup("Debug", platform);
		addPropertyGroup("Profile", platform);
		addPropertyGroup("Profile_FastCap", platform);
		addPropertyGroup("Release", platform);
		addPropertyGroup("Release_LTCG", true, platform);
	}
	else if (platform == Platform::WindowsRT) {
		addWin8PropertyGroup(true, "Win32");
		addWin8PropertyGroup(true, "ARM");
		addWin8PropertyGroup(true, "x64");
		addWin8PropertyGroup(false, "Win32");
		addWin8PropertyGroup(false, "ARM");
		addWin8PropertyGroup(false, "x64");
	}
	else {
		p("<PropertyGroup Condition=\"'$(Configuration)'=='Debug'\" Label=\"Configuration\">", 1);
		p("<ConfigurationType>Application</ConfigurationType>", 2);
		p("<UseDebugLibraries>true</UseDebugLibraries>", 2);
		if (platform == Platform::Windows && Options::getVisualStudioVersion() == VS2013) {
			p("<PlatformToolset>v120</PlatformToolset>", 2);
		}
		else if (platform == Platform::Windows && Options::getVisualStudioVersion() == VS2012) {
			p("<PlatformToolset>v110</PlatformToolset>", 2);
		}
		if (platform == Platform::Windows) {
			p("<CharacterSet>Unicode</CharacterSet>", 2);
		}
		else if (platform == Platform::PlayStation3) {
			p("<PlatformToolset>SNC</PlatformToolset>", 2);
			p("<ExceptionsAndRtti>WithExceptsWithRtti</ExceptionsAndRtti>", 2);
		}
		p("</PropertyGroup>", 1);
		p("<PropertyGroup Condition=\"'$(Configuration)'=='Release'\" Label=\"Configuration\">", 1);
		p("<ConfigurationType>Application</ConfigurationType>", 2);
		p("<UseDebugLibraries>false</UseDebugLibraries>", 2);
		if (platform == Platform::Windows && Options::getVisualStudioVersion() == VS2013) {
			p("<PlatformToolset>v120</PlatformToolset>", 2);
		}
		if (platform == Platform::Windows && Options::getVisualStudioVersion() == VS2012) {
			p("<PlatformToolset>v110</PlatformToolset>", 2);
		}
		if (platform == Platform::Windows) {
			p("<WholeProgramOptimization>true</WholeProgramOptimization>", 2);
			p("<CharacterSet>Unicode</CharacterSet>", 2);
		}
		else if (platform == Platform::PlayStation3) {
			p("<PlatformToolset>SNC</PlatformToolset>", 2);
			p("<ExceptionsAndRtti>WithExceptsWithRtti</ExceptionsAndRtti>", 2);
		}
		p("</PropertyGroup>", 1);
	}
	p("<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\" />", 1);
	p("<ImportGroup Label=\"ExtensionSettings\">", 1);
	p("</ImportGroup>", 1);
	if (platform == Platform::WindowsRT) {
		std::vector<std::string> configurations;
		configurations.push_back("Debug");
		configurations.push_back("Release");
		for (std::string configuration : configurations) {
			for (std::string system : getSystems(platform)) {
				p("<ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='" + configuration + "|" + system + "'\">", 1);
				p("<Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />", 2);
				p("</ImportGroup>", 1);
			}
		}
	}
	else {
		for (std::string system : getSystems(platform)) {
			p("<ImportGroup Label=\"PropertySheets\" Condition=\"'$(Platform)'=='" + system + "'\">", 1);
			p("<Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />", 2);
			p("</ImportGroup>", 1);
		}
	}
	p("<PropertyGroup Label=\"UserMacros\" />", 1);

	if (platform == Platform::WindowsRT) {
	//<PropertyGroup>
	//<PackageCertificateKeyFile>Direct3DApplication1_TemporaryKey.pfx</PackageCertificateKeyFile>
	//</PropertyGroup>
	}

	if (platform == Platform::Windows || platform == Platform::Xbox360) {
		for (std::string system : getSystems(platform)) {
			for (std::string config : getConfigs(platform)) {
				p("<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='" + config + "|" + system + "'\">", 1);
					if (system == "Win32") {
						if (Options::getIntermediateDrive() == "") p("<IntDir>$(Configuration)\\" + project->getName() + "\\</IntDir>", 2);
						else p("<IntDir>" + Options::getIntermediateDrive() + ":\\$(projectname)\\$(Configuration)\\" + project->getName() + "\\</IntDir>", 2);
					}
					else p("<IntDir>$(Platform)\\$(Configuration)\\" + project->getName() + "\\</IntDir>", 2);
					p(std::string("<LinkIncremental>") + ((config == "Debug" || config == "CodeAnalysis") ? "true" : "false") + "</LinkIncremental>", 2);
				p("</PropertyGroup>", 1);
			}
		}
	}
	else if (platform == Platform::PlayStation3) {
		p("<PropertyGroup />", 1);
	}

	std::string defines = "";
	for (std::string define : project->getDefines()) defines += define + ";";

	std::string incstring = "";
	for (std::string inc : project->getIncludeDirs()) incstring += from.resolve(inc).toAbsolutePath().toString() + ";";
	if (incstring.length() > 0) incstring = incstring.substr(0, incstring.length() - 1);

	std::string debuglibs = "";
	for (Project* proj : project->getSubProjects()) debuglibs += "Debug\\" + proj->getName() + ".lib;";
	for (std::string lib : project->getLibs()) {
		if (Files::exists(from.resolve(lib + ".lib"))) debuglibs += from.resolve(lib).toAbsolutePath().toString() + ".lib;";
		else debuglibs += lib + ".lib;";
	}

	std::string releaselibs = "";
	for (Project* proj : project->getSubProjects()) releaselibs += "Release\\" + proj->getName() + ".lib;";
	for (std::string lib : project->getLibs()) {
		if (Files::exists(from.resolve(lib + ".lib"))) releaselibs += from.resolve(lib).toAbsolutePath().toString() + ".lib;";
		else releaselibs += lib + ".lib;";
	}

	if (platform == Platform::Xbox360) {
		addItemDefinitionGroup(incstring, defines, "Debug",           3, false, false, false, false, false, false, false, false, false, false, false, false, false, false, platform);
		addItemDefinitionGroup(incstring, defines, "CodeAnalysis",    4, true,  false, false, false, false, false, false, false, false, false, false, false, false, false, platform);
		addItemDefinitionGroup(incstring, defines, "Profile",         3, false, true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  false, false, false, platform);
		addItemDefinitionGroup(incstring, defines, "Profile_FastCap", 3, false, true,  true,  true,  true,  true,  true,  true,  false, true,  true,  true,  false, false, platform);
		addItemDefinitionGroup(incstring, defines, "Release",         3, false, true,  true,  true,  true,  true,  false, false, false, true,  true,  false, true,  false, platform);
		addItemDefinitionGroup(incstring, defines, "Release_LTCG",    3, false, true,  true,  true,  true,  true,  false, false, false, true,  true,  false, true,  true , platform);
	}
	else if (platform == Platform::WindowsRT) {
		p("<ItemDefinitionGroup>", 1);
			p("<Link>", 2);
				p("<AdditionalDependencies>MMDevAPI.lib;MFuuid.lib;MFReadWrite.lib;MFplat.lib;d2d1.lib;d3d11.lib;dxgi.lib;ole32.lib;windowscodecs.lib;dwrite.lib;%(AdditionalDependencies)</AdditionalDependencies>", 3);
			p("</Link>", 2);
			ClCompile* compile = new ClCompile(out, 2, Platform::WindowsRT, Configuration::Debug, split(incstring, ';'), split(defines, ';'));
			compile->print();
		p("</ItemDefinitionGroup>", 1);
	}
	else {
		for (std::string system : getSystems(platform)) {
			p("<ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|" + system + "'\">", 1);
			p("<ClCompile>", 2);

			if (Options::usesPrecompiledHeaders()) p("<PrecompiledHeader>Use</PrecompiledHeader>", 3);
			p("<AdditionalIncludeDirectories>" + incstring + "</AdditionalIncludeDirectories>", 3);

			if (platform == Platform::Windows) {
				p("<WarningLevel>Level3</WarningLevel>", 3);
				p("<Optimization>Disabled</Optimization>", 3);
				if (system == "x64") p("<PreprocessorDefinitions>" + defines + "SYS_64;WIN32;_DEBUG;_WINDOWS;%(PreprocessorDefinitions)</PreprocessorDefinitions>", 3);
				else p("<PreprocessorDefinitions>" + defines + "WIN32;_DEBUG;_WINDOWS;%(PreprocessorDefinitions)</PreprocessorDefinitions>", 3);
				p("<RuntimeLibrary>MultiThreadedDebug</RuntimeLibrary>", 3);
				p("<MultiProcessorCompilation>true</MultiProcessorCompilation>", 3);
				p("<MinimalRebuild>false</MinimalRebuild>", 3);
				if (Options::getVisualStudioVersion() == VS2013) p("<SDLCheck>true</SDLCheck>", 3);
			}
			else if (platform == Platform::PlayStation3) {
				p("<UserPreprocessorDefinitions>" + defines + "_DEBUG;__CELL_ASSERT__;%(UserPreprocessorDefinitions);</UserPreprocessorDefinitions>", 3);
				p("<GenerateDebugInformation>true</GenerateDebugInformation>", 3);
				p("<PreprocessorDefinitions>%(UserPreprocessorDefinitions);$(BuiltInDefines);__INTELLISENSE__;%(PreprocessorDefinitions);</PreprocessorDefinitions>", 3);
			}
			p("</ClCompile>", 2);
			if (platform == Platform::Windows) {
				p("<Link>", 2);
				if (cmd) p("<SubSystem>Console</SubSystem>", 3);
				else p("<SubSystem>Windows</SubSystem>", 3);
				p("<GenerateDebugInformation>true</GenerateDebugInformation>", 3);
				std::string libs = debuglibs;
				for (std::string lib : project->getLibsFor("debug_" + system)) {
					if (Files::exists(from.resolve(lib + ".lib"))) libs += from.resolve(lib).toAbsolutePath().toString() + ".lib;";
					else libs += lib + ".lib;";
				}
				for (std::string lib : project->getLibsFor(system)) {
					if (Files::exists(from.resolve(lib + ".lib"))) libs += from.resolve(lib).toAbsolutePath().toString() + ".lib;";
					else libs += lib + ".lib;";
				}
				for (std::string lib : project->getLibsFor("debug")) {
					if (Files::exists(from.resolve(lib + ".lib"))) libs += from.resolve(lib).toAbsolutePath().toString() + ".lib;";
					else libs += lib + ".lib;";
				}
				p("<AdditionalDependencies>" + libs + "kernel32.lib;user32.lib;gdi32.lib;winspool.lib;comdlg32.lib;advapi32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;odbc32.lib;odbccp32.lib;%(AdditionalDependencies)</AdditionalDependencies>", 3);
				p("</Link>", 2);
			}
			else if (platform == Platform::PlayStation3) {
				p("<Link>", 2);
				p("<AdditionalDependencies>libgcm_cmd.a;libgcm_sys_stub.a;libsysmodule_stub.a;libsysutil_stub.a;%(AdditionalDependencies)</AdditionalDependencies>", 3);
				p("</Link>", 2);
			}
			p("</ItemDefinitionGroup>", 1);
			p("<ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|" + system + "'\">", 1);
			p("<ClCompile>", 2);
			if (Options::usesPrecompiledHeaders()) p("<PrecompiledHeader>Use</PrecompiledHeader>", 3);
			p("<AdditionalIncludeDirectories>" + incstring + "</AdditionalIncludeDirectories>", 3);
			if (platform == Platform::Windows) {
				p("<WarningLevel>Level3</WarningLevel>", 3);
				p("<Optimization>MaxSpeed</Optimization>", 3);
				p("<FunctionLevelLinking>true</FunctionLevelLinking>", 3);
				p("<IntrinsicFunctions>true</IntrinsicFunctions>", 3);
				if (system == "x64") p("<PreprocessorDefinitions>" + defines + "SYS_64;WIN32;NDEBUG;_WINDOWS;%(PreprocessorDefinitions)</PreprocessorDefinitions>", 3);
				else p("<PreprocessorDefinitions>" + defines + "WIN32;NDEBUG;_WINDOWS;%(PreprocessorDefinitions)</PreprocessorDefinitions>", 3);
				p("<RuntimeLibrary>MultiThreaded</RuntimeLibrary>", 3);
				p("<MultiProcessorCompilation>true</MultiProcessorCompilation>", 3);
				p("<MinimalRebuild>false</MinimalRebuild>", 3);
				if (Options::getVisualStudioVersion() == VS2013) p("<SDLCheck>true</SDLCheck>", 3);
			}
			else if (platform == Platform::PlayStation3) {
				p("<UserPreprocessorDefinitions>" + defines + "NDEBUG;%(UserPreprocessorDefinitions);</UserPreprocessorDefinitions>", 3);
				p("<OptimizationLevel>Level2</OptimizationLevel>", 3);
				p("<PreprocessorDefinitions>%(UserPreprocessorDefinitions);$(BuiltInDefines);__INTELLISENSE__;%(PreprocessorDefinitions);</PreprocessorDefinitions>");
			}

			p("</ClCompile>", 2);
			if (platform == Platform::Windows) {
				p("<Link>", 2);
				if (cmd) p("<SubSystem>Console</SubSystem>", 3);
				else p("<SubSystem>Windows</SubSystem>", 3);
				p("<GenerateDebugInformation>true</GenerateDebugInformation>", 3);
				p("<EnableCOMDATFolding>true</EnableCOMDATFolding>", 3);
				p("<OptimizeReferences>true</OptimizeReferences>", 3);
				std::string libs = releaselibs;
				for (std::string lib : project->getLibsFor("release_" + system)) {
					if (Files::exists(from.resolve(lib + ".lib"))) libs += from.resolve(lib).toAbsolutePath().toString() + ".lib;";
					else libs += lib + ".lib;";
				}
				for (std::string lib : project->getLibsFor(system)) {
					if (Files::exists(from.resolve(lib + ".lib"))) libs += from.resolve(lib).toAbsolutePath().toString() + ".lib;";
					else libs += lib + ".lib;";
				}
				for (std::string lib : project->getLibsFor("release")) {
					if (Files::exists(from.resolve(lib + ".lib"))) libs += from.resolve(lib).toAbsolutePath().toString() + ".lib;";
					else libs += lib + ".lib;";
				}
				p("<AdditionalDependencies>" + libs + "kernel32.lib;user32.lib;gdi32.lib;winspool.lib;comdlg32.lib;advapi32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;odbc32.lib;odbccp32.lib;%(AdditionalDependencies)</AdditionalDependencies>", 3);
				p("</Link>", 2);
			}
			else if (platform == Platform::PlayStation3) {
				p("<Link>", 2);
				p("<AdditionalDependencies>libgcm_cmd.a;libgcm_sys_stub.a;libsysmodule_stub.a;libsysutil_stub.a;%(AdditionalDependencies)</AdditionalDependencies>", 3);
				p("</Link>", 2);
			}
			p("</ItemDefinitionGroup>", 1);
		}
	}

	p("<ItemGroup>", 1);
	for (std::string file : project->getFiles()) {
		if (endsWith(file, ".h")) p(std::string("<ClInclude Include=\"") + from.resolve(file).toAbsolutePath().toString() + "\" />", 2);
	}
	p("</ItemGroup>", 1);

	if (platform == Platform::WindowsRT) {
		p("<ItemDefinitionGroup Condition=\"'$(Configuration)'=='Release'\">", 1);
			p("<ClCompile>", 2);
				p("<PreprocessorDefinitions>" + defines + "NDEBUG;%(PreprocessorDefinitions)</PreprocessorDefinitions>", 3);
			p("</ClCompile>", 2);
		p("</ItemDefinitionGroup>", 1);

		p("<ItemDefinitionGroup Condition=\"'$(Configuration)'=='Debug'\">", 1);
			p("<ClCompile>", 2);
				p("<PreprocessorDefinitions>" + defines + "_DEBUG;%(PreprocessorDefinitions)</PreprocessorDefinitions>", 3);
			p("</ClCompile>", 2);
		p("</ItemDefinitionGroup>", 1);

		p("<ItemGroup>", 1);
		p("<Image Include=\"Logo.png\" />", 2);
		p("<Image Include=\"SmallLogo.png\" />", 2);
		p("<Image Include=\"StoreLogo.png\" />", 2);
		p("<Image Include=\"SplashScreen.png\" />", 2);
		p("</ItemGroup>", 1);
		p("<ItemGroup>", 1);
		p("<AppxManifest Include=\"Package.appxmanifest\" />", 2);
		p("</ItemGroup>", 1);

		p("<ItemGroup>", 1);
		exportAssetPath(from.resolve(project->getDebugDir()));
		p("</ItemGroup>", 1);
	}

	p("<ItemGroup>", 1);
	std::set<std::string> objects;
	std::string stdafx;
	for (std::string file : project->getFiles()) {
		if (endsWith(file, ".cpp") || endsWith(file, ".c") || endsWith(file, "cc")) {
			if (Options::usesPrecompiledHeaders() && (endsWith(file, "stdafx.cpp") || endsWith(file, "pch.cpp"))) {
				stdafx = file;
				continue;
			}
			std::string name = toLowerCase(file);
			if (contains(name, '/')) name = name.substr(lastIndexOf(name, '/') + 1);
			name = name.substr(0, lastIndexOf(name, '.'));
			if (objects.count(name) == 0) {
				p(std::string("<ClCompile Include=\"") + from.resolve(file).toAbsolutePath().toString() + "\" />", 2);
				objects.insert(name);
			}
			else {
				while (objects.count(name) > 0) {
					name = name + "_";
				}
				p(std::string("<ClCompile Include=\"") + from.resolve(file).toAbsolutePath().toString() + "\">", 2);
				p(std::string("<ObjectFileName>$(IntDir)\\") + name + ".obj</ObjectFileName>", 3);
				p("</ClCompile>", 2);
				objects.insert(name);
			}
		}
	}
	if (Options::usesPrecompiledHeaders()) {
		if (stdafx.size() == 0) throw std::runtime_error("stdafx.cpp not found.");
		p("<ClCompile Include=\"../" + stdafx + "\">", 2);
		p("<PrecompiledHeader Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">Create</PrecompiledHeader>", 3);
		p("<PrecompiledHeader Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">Create</PrecompiledHeader>", 3);
		p("</ClCompile>", 2);
	}
	p("</ItemGroup>", 1);

	if (platform == Platform::PlayStation3) {
		p("<ItemGroup>", 1);
		for (std::string file : project->getFiles()) {
			if (endsWith(file, ".vp.cg")) {
				p(std::string("<CustomBuild Include=\"") + from.resolve(file).toAbsolutePath().toString() + "\">", 2);
				p("<FileType>Document</FileType>", 2);
				p("<Command>$(SCE_PS3_ROOT)\\host-win32\\Cg\\bin\\sce-cgc -quiet -profile sce_vp_rsx -o \"%(Filename).vpo\" \"%(FullPath)\"\n$(SCE_PS3_ROOT)\\host-win32\\ppu\\bin\\ppu-lv2-objcopy  -I binary -O elf64-powerpc-celloslv2 -B powerpc \"%(Filename).vpo\" \"%(Filename).ppu.o\"</Command>", 2);
				p("<Outputs>%(Filename).vpo;%(Filename).ppu.o;%(Outputs)</Outputs>", 2);
				p("</CustomBuild>", 2);
			}
			else if (endsWith(file, ".fp.cg")) {
				p(std::string("<CustomBuild Include=\"") + from.resolve(file).toAbsolutePath().toString() + "\">", 2);
				p("<FileType>Document</FileType>", 2);
				p("<Command>$(SCE_PS3_ROOT)\\host-win32\\Cg\\bin\\sce-cgc -quiet -profile sce_fp_rsx -o \"%(Filename).fpo\" \"%(FullPath)\"\n$(SCE_PS3_ROOT)\\host-win32\\ppu\\bin\\ppu-lv2-objcopy  -I binary -O elf64-powerpc-celloslv2 -B powerpc \"%(Filename).fpo\" \"%(Filename).ppu.o\"</Command>", 2);
				p("<Outputs>%(Filename).fpo;%(Filename).ppu.o;%(Outputs)</Outputs>", 2);
				p("</CustomBuild>", 2);
			}
		}
		p("</ItemGroup>", 1);
	}

	if (platform == Platform::Windows) {
		p("<ItemGroup>", 1);
		for (std::string file : project->getFiles()) {
			if (endsWith(file, ".cg")) {
				p(std::string("<CustomBuild Include=\"") + from.resolve(file).toAbsolutePath().toString() + "\">", 2);
				p("<FileType>Document</FileType>", 2);
				p(std::string("<Command>..\\") + "Kt\\Tools\\ShaderCompiler.exe " + ((Options::getGraphicsApi() == OpenGL || Options::getGraphicsApi() == OpenGL2) ? "glsl" : "d3d9") + " \"%(FullPath)\" " + replace(from.resolve(project->getDebugDir()).toAbsolutePath().toString(), '/', '\\') + "\\Shaders\\%(Filename)</Command>", 2);
				p("<Outputs>" + replace(from.resolve(project->getDebugDir()).toAbsolutePath().toString(), '/', '\\') + "\\Shaders\\%(Filename)" + ((Options::getGraphicsApi() == OpenGL || Options::getGraphicsApi() == OpenGL2) ? ".glsl" : ".d3d9") + ";%(Outputs)</Outputs>", 2);
				p("</CustomBuild>", 2);
			}
		}
		p("</ItemGroup>", 1);
		p("<ItemGroup>", 1);
		for (std::string file : project->getFiles()) {
			if (koreDir.toString() != "" && endsWith(file, ".glsl")) {
				p(std::string("<CustomBuild Include=\"") + from.resolve(file).toAbsolutePath().toString() + "\">", 2);
				p("<FileType>Document</FileType>", 2);
				p(std::string("<Command>") + replace(from.resolve(koreDir).toAbsolutePath().toString(), '/', '\\') + "\\Tools\\kfx\\kfx.exe " + ((Options::getGraphicsApi() == OpenGL || Options::getGraphicsApi() == OpenGL2) ? "glsl" : (Options::getGraphicsApi() == Direct3D11 ? "d3d11" : "d3d9")) + " \"%(FullPath)\" ..\\" + replace(project->getDebugDir(), '/', '\\') + "\\%(Filename) ..\\build</Command>", 2);
				p("<Outputs>" + replace(from.resolve(project->getDebugDir()).toAbsolutePath().toString(), '/', '\\') + "\\%(Filename);%(Outputs)</Outputs>", 2);
				p("<Message>Compiling %(FullPath)</Message>", 2);
				p("</CustomBuild>", 2);
			}
		}
		p("</ItemGroup>", 1);
		p("<ItemGroup>", 1);
		for (std::string file : project->getFiles()) {
			if (koreDir.toString() != "" && endsWith(file, ".asm")) {
				p(std::string("<CustomBuild Include=\"") + from.resolve(file).toAbsolutePath().toString() + "\">", 2);
				p("<FileType>Document</FileType>", 2);
				p(std::string("<Command>") + replace(from.resolve(koreDir).toAbsolutePath().toString(), '/', '\\') + "\\Tools\\yasm-1.2.0-win32.exe -Xvc -f Win32 -g cv8 -o $(OutDir)\\%(Filename).obj -I ..\\Kt\\WebM\\src -I ..\\Kt\\WebM\\build -rnasm -pnasm \"%(FullPath)\"</Command>", 2);
				p("<Outputs>$(OutDir)\\%(Filename).obj</Outputs>", 2);
				p("<Message>Compiling %(FullPath)</Message>", 2);
				p("</CustomBuild>", 2);
			}
		}
		p("</ItemGroup>", 1);
		p("<ItemGroup>", 1);
			p("<None Include=\"icon.ico\" />", 2);
		p("</ItemGroup>", 1);
		p("<ItemGroup>", 1);
			p("<ResourceCompile Include=\"resources.rc\" />", 2);
		p("</ItemGroup>", 1);
	}

	if (platform == Platform::PlayStation3) {
			p("<Import Condition=\"'$(ConfigurationType)' == 'Makefile' and Exists('$(VCTargetsPath)\\Platforms\\$(Platform)\\SCE.Makefile.$(Platform).targets')\" Project=\"$(VCTargetsPath)\\Platforms\\$(Platform)\\SCE.Makefile.$(Platform).targets\" />", 1);
	}
	p("<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\" />", 1);
	p("<ImportGroup Label=\"ExtensionTargets\">", 2);
	p("</ImportGroup>", 1);
	p("</Project>");
	closeFile();
}

void ExporterVisualStudio::exportAssetPath(Path assetPath) {
	for (auto path : Files::newDirectoryStream(assetPath)) {
		if (Files::isDirectory(path)) {
			exportAssetPath(path);
		}
		else {
			p(std::string("<None Include=\"../") + path.toString() + "\">", 2);
				p("<DeploymentContent>true</DeploymentContent>", 3);
			p("</None>", 2);
		}
	}
}
