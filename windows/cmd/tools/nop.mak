# Microsoft Developer Studio Generated NMAKE File, Based on nop.dsp
!IF "$(CFG)" == ""
CFG=nop - Win32 Debug
!MESSAGE No configuration specified. Defaulting to nop - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "nop - Win32 Release" && "$(CFG)" != "nop - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "nop.mak" CFG="nop - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "nop - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "nop - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "nop - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\nop.exe"

!ELSE 

ALL : "ingraphs - Win32 Release" "$(OUTDIR)\nop.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"ingraphs - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\nop.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\nop.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "." /I "../../.." /I "../../lib/agraph" /I "../../lib/cdt" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "HAVE_CONFIG_H" /Fp"$(INTDIR)\nop.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\nop.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=agraph.lib cdt.lib common.lib ingraphs.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\nop.pdb" /machine:I386 /out:"$(OUTDIR)\nop.exe" /libpath:"../../lib/lib/Release" 
LINK32_OBJS= \
	"$(INTDIR)\nop.obj" \
	"$(OUTDIR)\ingraphs.lib"

"$(OUTDIR)\nop.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros


!ELSEIF  "$(CFG)" == "nop - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\nop.exe"

!ELSE 

ALL : "ingraphs - Win32 Debug" "$(OUTDIR)\nop.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"ingraphs - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\nop.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\nop.exe"
	-@erase "$(OUTDIR)\nop.ilk"
	-@erase "$(OUTDIR)\nop.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "." /I "../../.." /I "../../lib/agraph" /I "../../lib/cdt" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "HAVE_CONFIG_H" /Fp"$(INTDIR)\nop.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\nop.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=agraph.lib cdt.lib common.lib ingraphs.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\nop.pdb" /debug /machine:I386 /out:"$(OUTDIR)\nop.exe" /pdbtype:sept /libpath:"../../lib/lib/Debug" 
LINK32_OBJS= \
	"$(INTDIR)\nop.obj" \
	"$(OUTDIR)\ingraphs.lib"

"$(OUTDIR)\nop.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros


!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("nop.dep")
!INCLUDE "nop.dep"
!ELSE 
!MESSAGE Warning: cannot find "nop.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "nop - Win32 Release" || "$(CFG)" == "nop - Win32 Debug"
SOURCE=..\src\nop.c

"$(INTDIR)\nop.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!IF  "$(CFG)" == "nop - Win32 Release"

"ingraphs - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\ingraphs.mak" CFG="ingraphs - Win32 Release" 
   cd "."

"ingraphs - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\ingraphs.mak" CFG="ingraphs - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "nop - Win32 Debug"

"ingraphs - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\ingraphs.mak" CFG="ingraphs - Win32 Debug" 
   cd "."

"ingraphs - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\ingraphs.mak" CFG="ingraphs - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 


!ENDIF 

