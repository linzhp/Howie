# Microsoft Developer Studio Project File - Name="jalice_py" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=jalice_py - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "jalice_py.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "jalice_py.mak" CFG="jalice_py - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "jalice_py - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "jalice_py - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "jalice_py - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "JALICE_PY_EXPORTS" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "C:\Program Files\Python\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "JALICE_PY_EXPORTS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /dll /machine:I386 /out:"..\..\howie\backends\_jalice.dll" /libpath:"C:\Program Files\Python\libs"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing Python interface...
PostBuild_Cmds=copy /y jalice.py ..\..\howie\backends\jalice.py
# End Special Build Tool

!ELSEIF  "$(CFG)" == "jalice_py - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "JALICE_PY_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "C:\Program Files\Python\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "JALICE_PY_EXPORTS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /dll /debug /machine:I386 /out:"..\..\howie\backends\_jalice.dll" /pdbtype:sept /libpath:"C:\Program Files\Python\libs"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing Python interface...
PostBuild_Cmds=copy /y jalice.py ..\..\howie\backends\jalice.py
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "jalice_py - Win32 Release"
# Name "jalice_py - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\AimlWriter.cpp
# End Source File
# Begin Source File

SOURCE=..\Database.cpp
# End Source File
# Begin Source File

SOURCE=..\Element.cpp
# End Source File
# Begin Source File

SOURCE=..\Handler.cpp
# End Source File
# Begin Source File

SOURCE=..\HTTPProcessor.cpp
# End Source File
# Begin Source File

SOURCE=..\IrcResponder.cpp
# End Source File
# Begin Source File

SOURCE="..\jalice-py.cpp"
# End Source File
# Begin Source File

SOURCE=..\jalice_wrap.cxx
# End Source File
# Begin Source File

SOURCE=..\Kernel.cpp
# End Source File
# Begin Source File

SOURCE=..\Logger.cpp
# End Source File
# Begin Source File

SOURCE=..\Main.cpp
# End Source File
# Begin Source File

SOURCE=..\Match.cpp
# End Source File
# Begin Source File

SOURCE=..\Memory.cpp
# End Source File
# Begin Source File

SOURCE=..\Nodemaster.cpp
# End Source File
# Begin Source File

SOURCE=..\Parser.cpp
# End Source File
# Begin Source File

SOURCE=..\PreParser.cpp
# End Source File
# Begin Source File

SOURCE=..\ServerSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\Socket.cpp
# End Source File
# Begin Source File

SOURCE=..\SocketHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\StringTokenizer.cpp
# End Source File
# Begin Source File

SOURCE=..\Substituter.cpp
# End Source File
# Begin Source File

SOURCE=..\Template.cpp
# End Source File
# Begin Source File

SOURCE=..\TemplateProcessor.cpp
# End Source File
# Begin Source File

SOURCE=..\TokenProxyListener.cpp
# End Source File
# Begin Source File

SOURCE=..\Utils.cpp
# End Source File
# Begin Source File

SOURCE=..\XmlSocketResponder.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\AimlProcessor.h
# End Source File
# Begin Source File

SOURCE=..\AimlWriter.h
# End Source File
# Begin Source File

SOURCE=..\BotProcessor.h
# End Source File
# Begin Source File

SOURCE=..\BrProcessor.h
# End Source File
# Begin Source File

SOURCE=..\Compat.h
# End Source File
# Begin Source File

SOURCE=..\ConditionProcessor.h
# End Source File
# Begin Source File

SOURCE=..\Database.h
# End Source File
# Begin Source File

SOURCE=..\Element.h
# End Source File
# Begin Source File

SOURCE=..\FormalProcessor.h
# End Source File
# Begin Source File

SOURCE=..\GetProcessor.h
# End Source File
# Begin Source File

SOURCE=..\Handler.h
# End Source File
# Begin Source File

SOURCE=..\HTTPProcessor.h
# End Source File
# Begin Source File

SOURCE=..\IdProcessor.h
# End Source File
# Begin Source File

SOURCE=..\InputProcessor.h
# End Source File
# Begin Source File

SOURCE=..\IrcProcessor.h
# End Source File
# Begin Source File

SOURCE=..\IrcResponder.h
# End Source File
# Begin Source File

SOURCE=..\Kernel.h
# End Source File
# Begin Source File

SOURCE=..\LearnProcessor.h
# End Source File
# Begin Source File

SOURCE=..\Logger.h
# End Source File
# Begin Source File

SOURCE=..\LowercaseProcessor.h
# End Source File
# Begin Source File

SOURCE=..\Match.h
# End Source File
# Begin Source File

SOURCE=..\Memory.h
# End Source File
# Begin Source File

SOURCE=..\NameProcessor.h
# End Source File
# Begin Source File

SOURCE=..\Nodemapper.h
# End Source File
# Begin Source File

SOURCE=..\Nodemaster.h
# End Source File
# Begin Source File

SOURCE=..\NormalizeProcessor.h
# End Source File
# Begin Source File

SOURCE=..\Parser.h
# End Source File
# Begin Source File

SOURCE=..\PersonProcessor.h
# End Source File
# Begin Source File

SOURCE=..\Pointer.h
# End Source File
# Begin Source File

SOURCE=..\PreParser.h
# End Source File
# Begin Source File

SOURCE=..\RandomProcessor.h
# End Source File
# Begin Source File

SOURCE=..\Responder.h
# End Source File
# Begin Source File

SOURCE=..\SecureProcessor.h
# End Source File
# Begin Source File

SOURCE=..\SentenceProcessor.h
# End Source File
# Begin Source File

SOURCE=..\ServerSocket.h
# End Source File
# Begin Source File

SOURCE=..\SetProcessor.h
# End Source File
# Begin Source File

SOURCE=..\Socket.h
# End Source File
# Begin Source File

SOURCE=..\SocketHandler.h
# End Source File
# Begin Source File

SOURCE=..\SraiProcessor.h
# End Source File
# Begin Source File

SOURCE=..\SrProcessor.h
# End Source File
# Begin Source File

SOURCE=..\StarProcessor.h
# End Source File
# Begin Source File

SOURCE=..\StringTokenizer.h
# End Source File
# Begin Source File

SOURCE=..\Substituter.h
# End Source File
# Begin Source File

SOURCE=..\Template.h
# End Source File
# Begin Source File

SOURCE=..\TemplateProcessor.h
# End Source File
# Begin Source File

SOURCE=..\ThatProcessor.h
# End Source File
# Begin Source File

SOURCE=..\ThatstarProcessor.h
# End Source File
# Begin Source File

SOURCE=..\ThinkProcessor.h
# End Source File
# Begin Source File

SOURCE=..\TokenProxyListener.h
# End Source File
# Begin Source File

SOURCE=..\TopicstarProcessor.h
# End Source File
# Begin Source File

SOURCE=..\UppercaseProcessor.h
# End Source File
# Begin Source File

SOURCE=..\Utils.h
# End Source File
# Begin Source File

SOURCE=..\XmlSocketResponder.h
# End Source File
# End Group
# Begin Group "Other Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\howie.nsi
# End Source File
# Begin Source File

SOURCE=..\jalice.i

!IF  "$(CFG)" == "jalice_py - Win32 Release"

# Begin Custom Build - Generating Python Interface
ProjDir=.
InputPath=..\jalice.i
InputName=jalice

"$(ProjDir)\..\\$(InputName)_wrap.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	swig -c++ -python -o "$(ProjDir)\..\\$(InputName)_wrap.cxx" "$(InputPath)"

# End Custom Build

!ELSEIF  "$(CFG)" == "jalice_py - Win32 Debug"

# Begin Custom Build - Generating Python Interface
ProjDir=.
InputPath=..\jalice.i
InputName=jalice

"$(ProjDir)\..\\$(InputName)_wrap.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	swig -c++ -python -o "$(ProjDir)\..\\$(InputName)_wrap.cxx" "$(InputPath)"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\README.txt
# End Source File
# End Group
# End Target
# End Project
