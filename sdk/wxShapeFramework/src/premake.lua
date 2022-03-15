--*****************************************************************************
--*	Author:		Michal Bližňák
--*	Date:		18/11/2007
--*	Version:	1.00
--*	
--*	NOTES:
--*		- use the '/' slash for all paths.
--*****************************************************************************

dofile('../build/premake/scripts/init.lua')

--******* Initial Setup ************
--*	Most of the setting are set here.
--**********************************

-- Set the name of your package.
package.name = "wxSF"

-- Set this if you want a different name for your target than the package's name.
targetName = "wxsf"

-- Set the kind of package you want to create.
--		Options: exe | winexe | lib | dll
if( options["shared"] ) then
	package.kind = "dll"
else
	package.kind = "lib"
end

-- Setup the package compiler settings.
if( options["shared"] ) then
	package.defines = { "WXMAKINGDLL_WXSF", "WXMAKINGDLL_WXXS" }
end
if ( target == "vs2005" ) then
	-- Windows and Visual C++ 2005
	table.insert(package.defines,"_CRT_SECURE_NO_DEPRECATE" )
end
if( target == "vs2003" or target == "vs2005" ) then
         table.insert( package.defines, "_DISWARNINGS_MSVC" ) 
	table.insert(package.config["Debug"].defines, "_DEBUG_MSVC")
end
if( ( target == "vs2003" or target == "vs2005" ) and options["no-builtin-wchar"] ) then
	table.insert(package.buildoptions, "/Zc:wchar_t-")
end


-- Set the files to include.
package.files = { matchrecursive( "*.cpp", "*.h", "*.rc", "../include/*.h" ) }
if ( not windows ) then
	table.insert( package.excludes, matchrecursive( "*.rc" ) )
end

-- Set the include paths.
package.includepaths = { "../include", "../src" }

-- Set precompiled headers support
package.pchheader = "wx_pch.h"
package.pchsource = "wx_pch.cpp"

dofile('../build/premake/scripts/wxpresets.lua')
