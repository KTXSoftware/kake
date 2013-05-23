solution = Solution.new("kake")
project = Project.new("kake")

solution:cmd()

project:addExclude(".git/**")
project:addExclude("build/**")

project:addFile("Sources/**")

project:addFile("lua/lapi.c")
project:addFile("lua/lcode.c")
project:addFile("lua/lctype.c")
project:addFile("lua/ldebug.c")
project:addFile("lua/ldo.c")
project:addFile("lua/ldump.c")
project:addFile("lua/lfunc.c")
project:addFile("lua/lgc.c")
project:addFile("lua/llex.c")
project:addFile("lua/lmem.c")
project:addFile("lua/lobject.c")
project:addFile("lua/lopcodes.c")
project:addFile("lua/lparser.c")
project:addFile("lua/lstate.c")
project:addFile("lua/lstring.c")
project:addFile("lua/ltable.c")
project:addFile("lua/ltm.c")
project:addFile("lua/lundump.c")
project:addFile("lua/lvm.c")
project:addFile("lua/lzio.c")
project:addFile("lua/lauxlib.c")
project:addFile("lua/lbaselib.c")
project:addFile("lua/lbitlib.c")
project:addFile("lua/lcorolib.c")
project:addFile("lua/ldblib.c")
project:addFile("lua/liolib.c")
project:addFile("lua/lmathlib.c")
project:addFile("lua/loslib.c")
project:addFile("lua/lstrlib.c")
project:addFile("lua/ltablib.c")
project:addFile("lua/loadlib.c")
project:addFile("lua/linit.c")

solution:addProject(project)