# Microsoft Developer Studio Project File - Name="PeShield" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=PeShield - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PeShield.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PeShield.mak" CFG="PeShield - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PeShield - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "PeShield - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PeShield - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ../Library/dll/comm.lib ../Library/dll/io.lib ../Library/dll/pe.lib ../Library/dll/crypto.lib ../Library/dll/reg.lib /nologo /subsystem:windows /machine:I386 /out:"../../Bin/PeShield.exe"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\Library\dll\comm.dll ..\..\Bin\comm.dll	copy ..\Library\dll\io.dll ..\..\Bin\io.dll	copy ..\Library\dll\pe.dll ..\..\Bin\pe.dll	copy ..\Library\dll\crypto.dll ..\..\Bin\crypto.dll	copy ..\Library\dll\reg.dll ..\..\Bin\reg.dll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "PeShield - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ../Library/dll/comm.lib ../Library/dll/io.lib ../Library/dll/pe.lib ../Library/dll/crypto.lib ../Library/dll/reg.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../Bin/PeShield.exe" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\Library\dll\comm.dll ..\..\Bin\comm.dll	copy ..\Library\dll\io.dll ..\..\Bin\io.dll	copy ..\Library\dll\pe.dll ..\..\Bin\pe.dll	copy ..\Library\dll\crypto.dll ..\..\Bin\crypto.dll	copy ..\Library\dll\reg.dll ..\..\Bin\reg.dll
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "PeShield - Win32 Release"
# Name "PeShield - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Agent.cpp
# End Source File
# Begin Source File

SOURCE=.\AgentButton.cpp
# End Source File
# Begin Source File

SOURCE=.\AgentEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\AgentEdit2.cpp
# End Source File
# Begin Source File

SOURCE=.\Balloon.cpp
# End Source File
# Begin Source File

SOURCE=.\BalloonWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\NotifySink.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionBalloon.cpp
# End Source File
# Begin Source File

SOURCE=.\PeShield.cpp
# End Source File
# Begin Source File

SOURCE=.\PeShield.rc
# End Source File
# Begin Source File

SOURCE=.\PeShieldDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Sink.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\Tool.cpp
# End Source File
# Begin Source File

SOURCE=.\WebBalloon.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Agent.h
# End Source File
# Begin Source File

SOURCE=.\AgentButton.h
# End Source File
# Begin Source File

SOURCE=.\AgentEdit.h
# End Source File
# Begin Source File

SOURCE=.\AgentEdit2.h
# End Source File
# Begin Source File

SOURCE=.\BalloonWnd.h
# End Source File
# Begin Source File

SOURCE=.\OptionBalloon.h
# End Source File
# Begin Source File

SOURCE=.\PeShield.h
# End Source File
# Begin Source File

SOURCE=.\PeShieldDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Sink.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Tool.h
# End Source File
# Begin Source File

SOURCE=.\WebBalloon.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Button.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CheckBox.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PeShield.ico
# End Source File
# Begin Source File

SOURCE=.\res\RadioButton.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Tip.bmp
# End Source File
# End Group
# End Target
# End Project
