#include "ExporterAndroid.h"
#include "Files.h"
#include "Project.h"
#include "Solution.h"
#include "String.h"

using namespace kake;

extern Path koreDir;

namespace {
	void createDirectory(Path dir) {
		if (!Files::exists(dir)) Files::createDirectories(dir);
	}

	void copyDirectory(Path from, Path to) {
		createDirectory(to);
		for (auto path : Files::newDirectoryStream(from)) {
			if (Files::isDirectory(path)) {
				copyDirectory(path, to.resolve(from.relativize(path)));
			}
			else {
				Files::copy(path, to.resolve(from.relativize(path)), true);
			}
		}
	}
}
	
void ExporterAndroid::exportSolution(Solution* solution, Path from, Path to, Platform platform) {
	Project* project = solution->getProjects()[0];
	//String libname = solution.getName().toLowerCase().replace(' ', '-');
		
	bool nvpack = false; //Configuration.getAndroidDevkit() == AndroidDevkit.nVidia;
	
	copyDirectory(from.resolve(project->getDebugDir()), to.resolve("assets"));
		
	writeFile(to.resolve(".classpath"));
	p("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	p("<classpath>");
		p("<classpathentry kind=\"src\" path=\"Java-Sources\"/>", 1);
		p("<classpathentry kind=\"src\" path=\"gen\"/>", 1);
		p("<classpathentry kind=\"con\" path=\"com.android.ide.eclipse.adt.ANDROID_FRAMEWORK\"/>", 1);
		p("<classpathentry kind=\"con\" path=\"com.android.ide.eclipse.adt.LIBRARIES\"/>", 1);
		p("<classpathentry kind=\"output\" path=\"bin/classes\"/>", 1);
	p("</classpath>");
	closeFile();
		
	if (nvpack) {
		writeFile(to.resolve(".project"));
		p("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
		p("<projectDescription>");
		p("<name>" + solution->getName() + "</name>", 1);
		p("<comment></comment>", 1);
		p("<projects>", 1);
		p("</projects>", 1);
		p("<buildSpec>", 1);
			p("<buildCommand>", 2);
				p("<name>org.eclipse.cdt.managedbuilder.core.genmakebuilder</name>", 3);
				p("<triggers>clean,full,incremental,</triggers>", 3);
				p("<arguments>", 3);
					p("<dictionary>", 4);
						p("<key>?name?</key>", 5);
						p("<value></value>", 5);
					p("</dictionary>", 4);
					p("<dictionary>", 4);
						p("<key>org.eclipse.cdt.make.core.append_environment</key>", 5);
						p("<value>true</value>", 5);
					p("</dictionary>", 4);
					p("<dictionary>", 4);
						p("<key>org.eclipse.cdt.make.core.autoBuildTarget</key>", 5);
						p("<value>all</value>", 5);
					p("</dictionary>", 4);
					p("<dictionary>", 4);
						p("<key>org.eclipse.cdt.make.core.buildArguments</key>", 5);
						p("<value>${NDKROOT}/ndk-build NDK_DEBUG=1 V=1</value>", 5);
					p("</dictionary>", 4);
					p("<dictionary>", 4);
						p("<key>org.eclipse.cdt.make.core.buildCommand</key>", 5);
						p("<value>${CYGWIN_HOME}/bin/bash</value>", 5);
					p("</dictionary>", 4);
					p("<dictionary>", 4);
						p("<key>org.eclipse.cdt.make.core.buildLocation</key>", 5);
						p("<value>${workspace_loc:/NativeBasic/jni}</value>", 5);
					p("</dictionary>", 4);
					p("<dictionary>", 4);
						p("<key>org.eclipse.cdt.make.core.cleanBuildTarget</key>", 5);
						p("<value>clean</value>", 5);
					p("</dictionary>", 4);
					p("<dictionary>", 4);
						p("<key>org.eclipse.cdt.make.core.contents</key>", 5);
						p("<value>org.eclipse.cdt.make.core.activeConfigSettings</value>", 5);
					p("</dictionary>", 4);
					p("<dictionary>", 4);
						p("<key>org.eclipse.cdt.make.core.enableAutoBuild</key>", 5);
						p("<value>false</value>", 5);
					p("</dictionary>", 4);
					p("<dictionary>", 4);
						p("<key>org.eclipse.cdt.make.core.enableCleanBuild</key>", 5);
						p("<value>true</value>", 5);
					p("</dictionary>", 4);
					p("<dictionary>", 4);
						p("<key>org.eclipse.cdt.make.core.enableFullBuild</key>", 5);
						p("<value>true</value>", 5);
					p("</dictionary>", 4);
					p("<dictionary>", 4);
						p("<key>org.eclipse.cdt.make.core.fullBuildTarget</key>", 5);
						p("<value>all</value>", 5);
					p("</dictionary>", 4);
					p("<dictionary>", 4);
						p("<key>org.eclipse.cdt.make.core.stopOnError</key>", 5);
						p("<value>true</value>", 5);
					p("</dictionary>", 4);
					p("<dictionary>", 4);
						p("<key>org.eclipse.cdt.make.core.useDefaultBuildCmd</key>", 5);
						p("<value>false</value>", 5);
					p("</dictionary>", 4);
				p("</arguments>", 3);
			p("</buildCommand>", 2);
			p("<buildCommand>", 2);
				p("<name>com.android.ide.eclipse.adt.ResourceManagerBuilder</name>", 3);
				p("<arguments>", 3);
				p("</arguments>", 3);
			p("</buildCommand>", 2);
			p("<buildCommand>", 2);
				p("<name>com.android.ide.eclipse.adt.PreCompilerBuilder</name>", 3);
				p("<arguments>", 3);
				p("</arguments>", 3);
			p("</buildCommand>", 2);
			p("<buildCommand>", 2);
				p("<name>org.eclipse.jdt.core.javabuilder</name>", 3);
				p("<arguments>", 3);
				p("</arguments>", 3);
			p("</buildCommand>", 2);
			p("<buildCommand>", 2);
				p("<name>com.android.ide.eclipse.adt.ApkBuilder</name>", 3);
				p("<arguments>", 3);
				p("</arguments>", 3);
			p("</buildCommand>", 2);
			p("<buildCommand>", 2);
				p("<name>org.eclipse.cdt.managedbuilder.core.ScannerConfigBuilder</name>", 3);
				p("<triggers>full,incremental,</triggers>", 3);
				p("<arguments>", 3);
				p("</arguments>", 3);
			p("</buildCommand>", 2);
		p("</buildSpec>", 1);
		p("<natures>", 1);
			p("<nature>com.android.ide.eclipse.adt.AndroidNature</nature>", 2);
			p("<nature>org.eclipse.jdt.core.javanature</nature>", 2);
			p("<nature>org.eclipse.cdt.core.cnature</nature>", 2);
			p("<nature>org.eclipse.cdt.core.ccnature</nature>", 2);
			p("<nature>org.eclipse.cdt.managedbuilder.core.managedBuildNature</nature>", 2);
			p("<nature>org.eclipse.cdt.managedbuilder.core.ScannerConfigNature</nature>", 2);
		p("</natures>", 1);
		p("<linkedResources>", 1);
			p("<link>", 2);
					p("<name>Sources</name>", 3);
					p("<type>2</type>", 3);
					p("<location>" + replace(from.resolve("Sources").toAbsolutePath().toString(), '\\', '/') + "</location>", 3);
				p("</link>", 2);
				p("<link>", 2);
					p("<name>Kore</name>", 3);
					p("<type>2</type>", 3);
					p("<location>" + replace(koreDir.resolve("Sources").toAbsolutePath().toString(), '\\', '/') + "</location>", 3);
				p("</link>", 2);
				p("<link>", 2);
					p("<name>Android-Backend</name>", 3);
					p("<type>2</type>", 3);
					p("<location>" + replace(koreDir.resolve(Paths::get("Backends", "Android", "Sources")).toAbsolutePath().toString(), '\\', '/') + "</location>", 3);
				p("</link>", 2);
				p("<link>", 2);
					p("<name>OpenGL2-Backend</name>", 3);
					p("<type>2</type>", 3);
					p("<location>" + replace(koreDir.resolve(Paths::get("Backends", "OpenGL2", "Sources")).toAbsolutePath().toString(), '\\', '/') + "</location>", 3);
				p("</link>", 2);
				p("<link>", 2);
					p("<name>Java-Sources</name>", 3);
					p("<type>2</type>", 3);
					p("<location>" + replace(koreDir.resolve(Paths::get("Backends", "Android", "Java-Sources")).toAbsolutePath().toString(), '\\', '/') + "</location>", 3);
				p("</link>", 2);
		p("</linkedResources>", 1);
		p("</projectDescription>");
		closeFile();
	}
	else {
		writeFile(to.resolve(".project"));
		p("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
		p("<projectDescription>");
			p("<name>" + solution->getName() + "</name>", 1);
			p("<comment></comment>", 1);
			p("<projects>", 1);
			p("</projects>", 1);
			p("<buildSpec>", 1);
				p("<buildCommand>", 2);
					p("<name>org.eclipse.cdt.managedbuilder.core.genmakebuilder</name>", 3);
					p("<triggers>clean,full,incremental,</triggers>", 3);
					p("<arguments>", 3);
						p("<dictionary>", 4);
							p("<key>?children?</key>", 5);
							p("<value>?name?=outputEntries\\|?children?=?name?=entry\\\\\\\\\\\\\\|\\\\\\|?name?=entry\\\\\\\\\\\\\\|\\\\\\|\\||</value>", 5);
						p("</dictionary>", 4);
						p("<dictionary>", 4);
							p("<key>?name?</key>", 5);
							p("<value></value>", 5);
						p("</dictionary>", 4);
						p("<dictionary>", 4);
							p("<key>org.eclipse.cdt.make.core.append_environment</key>", 5);
							p("<value>true</value>", 5);
						p("</dictionary>", 4);
						p("<dictionary>", 4);
							p("<key>org.eclipse.cdt.make.core.buildArguments</key>", 5);
							p("<value></value>", 5);
						p("</dictionary>", 4);
						p("<dictionary>", 4);
							p("<key>org.eclipse.cdt.make.core.buildCommand</key>", 5);
							p("<value>ndk-build</value>", 5);
						p("</dictionary>", 4);
						p("<dictionary>", 4);
							p("<key>org.eclipse.cdt.make.core.cleanBuildTarget</key>", 5);
							p("<value>clean</value>", 5);
						p("</dictionary>", 4);
						p("<dictionary>", 4);
							p("<key>org.eclipse.cdt.make.core.contents</key>", 5);
							p("<value>org.eclipse.cdt.make.core.activeConfigSettings</value>", 5);
						p("</dictionary>", 4);
						p("<dictionary>", 4);
							p("<key>org.eclipse.cdt.make.core.enableAutoBuild</key>", 5);
							p("<value>false</value>", 5);
						p("</dictionary>", 4);
						p("<dictionary>", 4);
							p("<key>org.eclipse.cdt.make.core.enableCleanBuild</key>", 5);
							p("<value>true</value>", 5);
						p("</dictionary>", 4);
						p("<dictionary>", 4);
							p("<key>org.eclipse.cdt.make.core.enableFullBuild</key>", 5);
							p("<value>true</value>", 5);
						p("</dictionary>", 4);
						p("<dictionary>", 4);
							p("<key>org.eclipse.cdt.make.core.stopOnError</key>", 5);
							p("<value>true</value>", 5);
						p("</dictionary>", 4);
						p("<dictionary>", 4);
							p("<key>org.eclipse.cdt.make.core.useDefaultBuildCmd</key>", 5);
							p("<value>true</value>", 5);
						p("</dictionary>", 4);
					p("</arguments>", 3);
				p("</buildCommand>", 2);
				p("<buildCommand>", 2);
					p("<name>com.android.ide.eclipse.adt.ResourceManagerBuilder</name>", 3);
					p("<arguments>", 3);
					p("</arguments>", 3);
				p("</buildCommand>", 2);
				p("<buildCommand>", 2);
					p("<name>com.android.ide.eclipse.adt.PreCompilerBuilder</name>", 3);
					p("<arguments>", 3);
					p("</arguments>", 3);
				p("</buildCommand>", 2);
				p("<buildCommand>", 2);
					p("<name>org.eclipse.jdt.core.javabuilder</name>", 3);
					p("<arguments>", 3);
					p("</arguments>", 3);
				p("</buildCommand>", 2);
				p("<buildCommand>", 2);
					p("<name>com.android.ide.eclipse.adt.ApkBuilder</name>", 3);
					p("<arguments>", 3);
					p("</arguments>", 3);
				p("</buildCommand>", 2);
				p("<buildCommand>", 2);
					p("<name>org.eclipse.cdt.managedbuilder.core.ScannerConfigBuilder</name>", 3);
					p("<triggers>full,incremental,</triggers>", 3);
					p("<arguments>", 3);
					p("</arguments>", 3);
				p("</buildCommand>", 2);
			p("</buildSpec>", 1);
			p("<natures>", 1);
				p("<nature>com.android.ide.eclipse.adt.AndroidNature</nature>", 2);
				p("<nature>org.eclipse.jdt.core.javanature</nature>", 2);
				p("<nature>org.eclipse.cdt.core.cnature</nature>", 2);
				p("<nature>org.eclipse.cdt.core.ccnature</nature>", 2);
				p("<nature>org.eclipse.cdt.managedbuilder.core.managedBuildNature</nature>", 2);
				p("<nature>org.eclipse.cdt.managedbuilder.core.ScannerConfigNature</nature>", 2);
			p("</natures>", 1);
			p("<linkedResources>", 1);
				p("<link>", 2);
					p("<name>Sources</name>", 3);
					p("<type>2</type>", 3);
					p("<location>" + replace(from.resolve("Sources").toAbsolutePath().toString(), '\\', '/') + "</location>", 3);
				p("</link>", 2);
				p("<link>", 2);
					p("<name>Kore</name>", 3);
					p("<type>2</type>", 3);
					p("<location>" + replace(koreDir.resolve("Sources").toAbsolutePath().toString(), '\\', '/') + "</location>", 3);
				p("</link>", 2);
				p("<link>", 2);
					p("<name>Android-Backend</name>", 3);
					p("<type>2</type>", 3);
					p("<location>" + replace(koreDir.resolve(Paths::get("Backends", "Android", "Sources")).toAbsolutePath().toString(), '\\', '/') + "</location>", 3);
				p("</link>", 2);
				p("<link>", 2);
					p("<name>OpenGL2-Backend</name>", 3);
					p("<type>2</type>", 3);
					p("<location>" + replace(koreDir.resolve(Paths::get("Backends", "OpenGL2", "Sources")).toAbsolutePath().toString(), '\\', '/') + "</location>", 3);
				p("</link>", 2);
				p("<link>", 2);
					p("<name>Java-Sources</name>", 3);
					p("<type>2</type>", 3);
					p("<location>" + replace(koreDir.resolve(Paths::get("Backends", "Android", "Java-Sources")).toAbsolutePath().toString(), '\\', '/') + "</location>", 3);
				p("</link>", 2);
			p("</linkedResources>", 1);
		p("</projectDescription>");
		closeFile();
	}
		
	if (nvpack) {
		writeFile(to.resolve(".cproject"));
		p("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>");
		p("<?fileVersion 4.0.0?>");

		p("<cproject storage_type_id=\"org.eclipse.cdt.core.XmlProjectDescriptionStorage\">");
			p("<storageModule moduleId=\"org.eclipse.cdt.core.settings\">", 1);
				p("<cconfiguration id=\"0.948371170\">", 2);
					p("<storageModule buildSystemId=\"org.eclipse.cdt.managedbuilder.core.configurationDataProvider\" id=\"0.948371170\" moduleId=\"org.eclipse.cdt.core.settings\" name=\"Default\">", 3);
						p("<externalSettings/>", 4);
						p("<extensions>", 4);
							p("<extension id=\"org.eclipse.cdt.core.VCErrorParser\" point=\"org.eclipse.cdt.core.ErrorParser\"/>", 5);
							p("<extension id=\"org.eclipse.cdt.core.GmakeErrorParser\" point=\"org.eclipse.cdt.core.ErrorParser\"/>", 5);
							p("<extension id=\"org.eclipse.cdt.core.CWDLocator\" point=\"org.eclipse.cdt.core.ErrorParser\"/>", 5);
							p("<extension id=\"org.eclipse.cdt.core.GCCErrorParser\" point=\"org.eclipse.cdt.core.ErrorParser\"/>", 5);
							p("<extension id=\"org.eclipse.cdt.core.GASErrorParser\" point=\"org.eclipse.cdt.core.ErrorParser\"/>", 5);
							p("<extension id=\"org.eclipse.cdt.core.GLDErrorParser\" point=\"org.eclipse.cdt.core.ErrorParser\"/>", 5);
						p("</extensions>", 4);
					p("</storageModule>", 3);
					p("<storageModule moduleId=\"cdtBuildSystem\" version=\"4.0.0\">", 3);
						p("<configuration artifactName=\"${ProjName}\" buildProperties=\"\" description=\"\" id=\"0.948371170\" name=\"Default\" parent=\"org.eclipse.cdt.build.core.prefbase.cfg\">", 4);
							p("<folderInfo id=\"0.948371170.\" name=\"/\" resourcePath=\"\">", 5);
								p("<toolChain id=\"org.eclipse.cdt.build.core.prefbase.toolchain.2144327658\" name=\"No ToolChain\" resourceTypeBasedDiscovery=\"false\" superClass=\"org.eclipse.cdt.build.core.prefbase.toolchain\">", 6);
									p("<targetPlatform id=\"org.eclipse.cdt.build.core.prefbase.toolchain.2144327658.1773654025\" name=\"\"/>", 7);
									p("<builder arguments=\"${NDKROOT}/ndk-build NDK_DEBUG=1 V=1\" buildPath=\"${workspace_loc:/NativeBasic/jni}\" command=\"${CYGWIN_HOME}/bin/bash\" id=\"org.eclipse.cdt.build.core.settings.default.builder.1337983557\" keepEnvironmentInBuildfile=\"false\" managedBuildOn=\"false\" name=\"Gnu Make Builder\" superClass=\"org.eclipse.cdt.build.core.settings.default.builder\"/>", 7);
									p("<tool id=\"org.eclipse.cdt.build.core.settings.holder.libs.1556467662\" name=\"holder for library settings\" superClass=\"org.eclipse.cdt.build.core.settings.holder.libs\"/>", 7);
									p("<tool id=\"org.eclipse.cdt.build.core.settings.holder.1056633961\" name=\"Assembly\" superClass=\"org.eclipse.cdt.build.core.settings.holder\">", 7);
										p("<inputType id=\"org.eclipse.cdt.build.core.settings.holder.inType.321860599\" languageId=\"org.eclipse.cdt.core.assembly\" languageName=\"Assembly\" sourceContentType=\"org.eclipse.cdt.core.asmSource\" superClass=\"org.eclipse.cdt.build.core.settings.holder.inType\"/>", 8);
									p("</tool>", 7);
									p("<tool id=\"org.eclipse.cdt.build.core.settings.holder.1685526741\" name=\"GNU C++\" superClass=\"org.eclipse.cdt.build.core.settings.holder\">", 7);
										p("<inputType id=\"org.eclipse.cdt.build.core.settings.holder.inType.611297456\" languageId=\"org.eclipse.cdt.core.g++\" languageName=\"GNU C++\" sourceContentType=\"org.eclipse.cdt.core.cxxSource,org.eclipse.cdt.core.cxxHeader\" superClass=\"org.eclipse.cdt.build.core.settings.holder.inType\"/>", 8);
									p("</tool>", 7);
									p("<tool id=\"org.eclipse.cdt.build.core.settings.holder.1404402761\" name=\"GNU C\" superClass=\"org.eclipse.cdt.build.core.settings.holder\">", 7);
										p("<inputType id=\"org.eclipse.cdt.build.core.settings.holder.inType.2041861228\" languageId=\"org.eclipse.cdt.core.gcc\" languageName=\"GNU C\" sourceContentType=\"org.eclipse.cdt.core.cSource,org.eclipse.cdt.core.cHeader\" superClass=\"org.eclipse.cdt.build.core.settings.holder.inType\"/>", 8);
									p("</tool>", 7);
								p("</toolChain>", 6);
							p("</folderInfo>", 5);
						p("</configuration>", 4);
					p("</storageModule>", 3);
					p("<storageModule moduleId=\"org.eclipse.cdt.core.externalSettings\"/>", 3);
				p("</cconfiguration>", 2);
			p("</storageModule>", 1);
			p("<storageModule moduleId=\"cdtBuildSystem\" version=\"4.0.0\">", 1);
				p("<project id=\"" + solution->getName() + ".null.1097473682\" name=\"" + solution->getName() + "\"/>", 2);
			p("</storageModule>", 1);
			p("<storageModule moduleId=\"scannerConfiguration\">", 1);
				p("<autodiscovery enabled=\"true\" problemReportingEnabled=\"true\" selectedProfileId=\"\"/>", 2);
				p("<scannerConfigBuildInfo instanceId=\"0.948371170\">", 2);
					p("<autodiscovery enabled=\"true\" problemReportingEnabled=\"true\" selectedProfileId=\"\"/>", 3);
				p("</scannerConfigBuildInfo>", 2);
			p("</storageModule>", 1);
			p("<storageModule moduleId=\"refreshScope\" versionNumber=\"1\">", 1);
				p("<resource resourceType=\"PROJECT\" workspacePath=\"/" + solution->getName() + "\"/>", 2);
			p("</storageModule>", 1);
		p("</cproject>");
		closeFile();
	}
	else {
		writeFile(to.resolve(".cproject"));
		p("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>");
		p("<?fileVersion 4.0.0?>");
		p();
		p("<cproject storage_type_id=\"org.eclipse.cdt.core.XmlProjectDescriptionStorage\">");
			p("<storageModule moduleId=\"org.eclipse.cdt.core.settings\">", 1);
				p("<cconfiguration id=\"com.android.toolchain.gcc.1781720887\">", 2);
					p("<storageModule buildSystemId=\"org.eclipse.cdt.managedbuilder.core.configurationDataProvider\" id=\"com.android.toolchain.gcc.1781720887\" moduleId=\"org.eclipse.cdt.core.settings\" name=\"Default\">", 3);
						p("<externalSettings/>", 4);
						p("<extensions>", 4);
							p("<extension id=\"org.eclipse.cdt.core.ELF\" point=\"org.eclipse.cdt.core.BinaryParser\"/>", 5);
							p("<extension id=\"org.eclipse.cdt.core.VCErrorParser\" point=\"org.eclipse.cdt.core.ErrorParser\"/>", 5);
							p("<extension id=\"org.eclipse.cdt.core.GmakeErrorParser\" point=\"org.eclipse.cdt.core.ErrorParser\"/>", 5);
							p("<extension id=\"org.eclipse.cdt.core.CWDLocator\" point=\"org.eclipse.cdt.core.ErrorParser\"/>", 5);
							p("<extension id=\"org.eclipse.cdt.core.MakeErrorParser\" point=\"org.eclipse.cdt.core.ErrorParser\"/>", 5);
							p("<extension id=\"org.eclipse.cdt.core.GCCErrorParser\" point=\"org.eclipse.cdt.core.ErrorParser\"/>", 5);
							p("<extension id=\"org.eclipse.cdt.core.GASErrorParser\" point=\"org.eclipse.cdt.core.ErrorParser\"/>", 5);
							p("<extension id=\"org.eclipse.cdt.core.GLDErrorParser\" point=\"org.eclipse.cdt.core.ErrorParser\"/>", 5);
						p("</extensions>", 4);
					p("</storageModule>", 3);
					p("<storageModule moduleId=\"cdtBuildSystem\" version=\"4.0.0\">", 3);
						p("<configuration artifactName=\"${ProjName}\" buildProperties=\"\" description=\"\" id=\"com.android.toolchain.gcc.1607988172\" name=\"Default\" parent=\"org.eclipse.cdt.build.core.emptycfg\">", 4);
							p("<folderInfo id=\"com.android.toolchain.gcc.1781720887.2072953625\" name=\"/\" resourcePath=\"\">", 5);
								p("<toolChain id=\"com.android.toolchain.gcc.1464882464\" name=\"com.android.toolchain.gcc\" superClass=\"com.android.toolchain.gcc\">", 6);
									p("<targetPlatform binaryParser=\"org.eclipse.cdt.core.ELF\" id=\"com.android.targetPlatform.1701868169\" isAbstract=\"false\" superClass=\"com.android.targetPlatform\"/>", 7);
									p("<builder id=\"com.android.builder.584823742\" keepEnvironmentInBuildfile=\"false\" managedBuildOn=\"false\" name=\"Android Builder\" superClass=\"com.android.builder\">", 7);
										p("<outputEntries>", 8);
											p("<entry flags=\"VALUE_WORKSPACE_PATH|RESOLVED\" kind=\"outputPath\" name=\"obj\"/>", 9);
											p("<entry flags=\"VALUE_WORKSPACE_PATH|RESOLVED\" kind=\"outputPath\" name=\"libs\"/>", 9);
										p("</outputEntries>", 8);
									p("</builder>", 7);
									p("<tool id=\"com.android.gcc.compiler.1684132155\" name=\"Android GCC Compiler\" superClass=\"com.android.gcc.compiler\">", 7);
										p("<inputType id=\"com.android.gcc.inputType.690571746\" superClass=\"com.android.gcc.inputType\"/>", 8);
									p("</tool>", 7);
								p("</toolChain>", 6);
							p("</folderInfo>", 5);
							p("<sourceEntries>", 5);
								p("<entry flags=\"VALUE_WORKSPACE_PATH|RESOLVED\" kind=\"sourcePath\" name=\"jni\"/>", 6);
								p("<entry flags=\"VALUE_WORKSPACE_PATH|RESOLVED\" kind=\"sourcePath\" name=\"Kore\"/>", 6);
								p("<entry flags=\"VALUE_WORKSPACE_PATH|RESOLVED\" kind=\"sourcePath\" name=\"Sources\"/>", 6);
								p("<entry flags=\"VALUE_WORKSPACE_PATH|RESOLVED\" kind=\"sourcePath\" name=\"Android-Backend\"/>", 6);
								p("<entry flags=\"VALUE_WORKSPACE_PATH|RESOLVED\" kind=\"sourcePath\" name=\"OpenGL2-Backend\"/>", 6);
							p("</sourceEntries>", 5);
						p("</configuration>", 4);
					p("</storageModule>", 3);
					p("<storageModule moduleId=\"org.eclipse.cdt.core.externalSettings\"/>", 3);
				p("</cconfiguration>", 2);
			p("</storageModule>", 1);
			p("<storageModule moduleId=\"cdtBuildSystem\" version=\"4.0.0\">", 1);
				p("<project id=\"" + solution->getName() + ".null.831016985\" name=\"" + solution->getName() + "\"/>", 2);
			p("</storageModule>", 1);
			p("<storageModule moduleId=\"scannerConfiguration\">", 1);
				p("<autodiscovery enabled=\"true\" problemReportingEnabled=\"true\" selectedProfileId=\"\"/>", 2);
				p("<scannerConfigBuildInfo instanceId=\"com.android.toolchain.gcc.1781720887;com.android.toolchain.gcc.1781720887.2072953625;com.android.gcc.compiler.1684132155;com.android.gcc.inputType.690571746\">", 2);
					p("<autodiscovery enabled=\"true\" problemReportingEnabled=\"true\" selectedProfileId=\"com.android.AndroidPerProjectProfile\"/>", 3);
				p("</scannerConfigBuildInfo>", 2);
			p("</storageModule>", 1);
		p("</cproject>");
		closeFile();
	}
		
	writeFile(to.resolve("AndroidManifest.xml"));
	p("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
	p("<manifest xmlns:android=\"http://schemas.android.com/apk/res/android\"");
		p("package=\"com.ktxsoftware.kore\"", 1);
		p("android:versionCode=\"1\"");
		p("android:versionName=\"1.0\">");
		p();
		p("<uses-sdk android:minSdkVersion=\"8\" android:targetSdkVersion=\"17\"/>", 1);
		p();
		p("<application android:label=\"@string/app_name\">", 1);
			p("<activity android:name=\"KoreActivity\"", 2);
				p("android:label=\"@string/app_name\"", 3);
				p("android:configChanges=\"orientation|keyboardHidden\">", 3);
				p("<intent-filter>", 3);
					p("<action android:name=\"android.intent.action.MAIN\" />", 4);
					p("<category android:name=\"android.intent.category.LAUNCHER\" />", 4);
				p("</intent-filter>", 3);
			p("</activity>", 2);
		p("</application>", 1);
	p("</manifest>");
	closeFile();
		
	writeFile(to.resolve("project.properties"));
	p("target=android-8");
	closeFile();
		
	createDirectory(to.resolve(".settings"));
	if (nvpack) {
		writeFile(to.resolve(Paths::get(".settings", "org.eclipse.jdt.core.prefs")));
		p("eclipse.preferences.version=1");
		p("org.eclipse.jdt.core.compiler.codegen.inlineJsrBytecode=enabled");
		p("org.eclipse.jdt.core.compiler.codegen.targetPlatform=1.6");
		p("org.eclipse.jdt.core.compiler.codegen.unusedLocal=preserve");
		p("org.eclipse.jdt.core.compiler.compliance=1.6");
		p("org.eclipse.jdt.core.compiler.debug.lineNumber=generate");
		p("org.eclipse.jdt.core.compiler.debug.localVariable=generate");
		p("org.eclipse.jdt.core.compiler.debug.sourceFile=generate");
		p("org.eclipse.jdt.core.compiler.problem.assertIdentifier=error");
		p("org.eclipse.jdt.core.compiler.problem.enumIdentifier=error");
		p("org.eclipse.jdt.core.compiler.source=1.6");
		closeFile();
	}
	else {
		writeFile(to.resolve(Paths::get(".settings", "org.eclipse.jdt.core.prefs")));
		p("eclipse.preferences.version=1");
		p("org.eclipse.jdt.core.compiler.codegen.targetPlatform=1.6");
		p("org.eclipse.jdt.core.compiler.compliance=1.6");
		p("org.eclipse.jdt.core.compiler.source=1.6");
		closeFile();
	}
		
	if (nvpack) {
		writeFile(to.resolve("build.xml"));
		p("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
		p("<project name=\"native_basic\" default=\"help\">");
			p("<property file=\"local.properties\" />", 1);
			p("<property file=\"ant.properties\" />", 1);
			p("<loadproperties srcFile=\"project.properties\" />", 1);
			p("<fail", 1);
				p("message=\"sdk.dir is missing. Make sure to generate local.properties using 'android update project' or to inject it through an env var\"", 2);
				p("unless=\"sdk.dir\"", 2);
			p("/>", 1);
			p("<import file=\"custom_rules.xml\" optional=\"true\" />", 1);
			p("<import file=\"${sdk.dir}/tools/ant/build.xml\" />", 1);
		p("</project>");
		closeFile();
	}
		
	createDirectory(to.resolve("res"));
	createDirectory(to.resolve(Paths::get("res", "values")));
	writeFile(to.resolve(Paths::get("res", "values", "strings.xml")));
	p("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
	p("<resources>");
	p("<string name=\"app_name\">" + solution->getName() + "</string>", 1);
	p("</resources>");
	closeFile();
		
	createDirectory(to.resolve("jni"));
		
	if (nvpack) {
		/*
			LOCAL_PATH := $(subst //,/,$(call my-dir))
include $(CLEAR_VARS)
LOCAL_MODULE := native_basic

LOCAL_SRC_FILES := $(wildcard *.cpp)
LOCAL_SRC_FILES += $(wildcard *.c)

LOCAL_ARM_MODE   := arm

LOCAL_LDLIBS :=  -lstdc++ -lc -lm -llog -landroid -ldl -lGLESv2 -lEGL
LOCAL_STATIC_LIBRARIES := nv_and_util nv_egl_util nv_bitfont nv_math nv_glesutil nv_hhdds nv_log nv_shader nv_file nv_thread

include $(BUILD_SHARED_LIBRARY)

$(call import-add-path, ../../../libs/jni)

$(call import-module,nv_and_util)
$(call import-module,nv_egl_util)
$(call import-module,nv_bitfont)
$(call import-module,nv_math)
$(call import-module,nv_glesutil)
$(call import-module,nv_hhdds)
$(call import-module,nv_log)
$(call import-module,nv_shader)
$(call import-module,nv_file)
$(call import-module,nv_thread) 
			*/
	}
	writeFile(to.resolve(Paths::get("jni", "Android.mk")));
	p("LOCAL_PATH := $(call my-dir)");
	p();
	p("include $(CLEAR_VARS)");
	p();
	p("LOCAL_MODULE    := Kore");
	std::string files = "";
	for (std::string filename : project->getFiles()) if (endsWith(filename, ".c") || endsWith(filename, ".cpp") || endsWith(filename, ".cc") || endsWith(filename, ".s")) files += "../../" + filename + " ";
	p("LOCAL_SRC_FILES := " + files);
	std::string defines = "";
	for (std::string def : project->getDefines()) defines += "-D" + replace(def, '\"', "\\\"") + " ";
	p("LOCAL_CFLAGS := " + defines);
	std::string includes = "";
	for (std::string inc : project->getIncludeDirs()) includes += "$(LOCAL_PATH)/../../" + inc + " ";
	p("LOCAL_C_INCLUDES := " + includes);
	p("LOCAL_LDLIBS    := -llog -lGLESv2");
	p();
	p("include $(BUILD_SHARED_LIBRARY)");
	p();
	closeFile();
	
	writeFile(to.resolve(Paths::get("jni", "Application.mk")));
	p("APP_CPPFLAGS += -fexceptions -frtti");
	p("APP_STL := gnustl_static");
	//p("APP_ABI := all");
	p("APP_ABI := armeabi-v7a");
	//p("LOCAL_ARM_NEON := true");
	if (nvpack) {
		//APP_ABI := armeabi-v7a
		//APP_PLATFORM := android-10
	}
	closeFile();
}
