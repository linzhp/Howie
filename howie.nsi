# howie.nsi
#
# Author: Cort Stratton (cort@andrew.cmu.edu)
#

!include "MUI.nsh"

# Basic definitions
!define MUI_PRODUCT "Howie"
!define MUI_VERSION "20030515"

#--------------------------------
#Configuration

	# General
	Name "Howie"
	OutFile "howie-install-${MUI_VERSION}.exe"

	# Folder selection page
	InstallDir "$PROGRAMFILES\${MUI_PRODUCT}"
	InstallDirRegKey HKLM "Software\${MUI_PRODUCT}" "Install_Dir"

	# Compiler configuration
	SetCompressor bzip2
	ShowInstDetails show
	ShowUninstDetails show
	InstProgressFlags smooth colored
	XPStyle on

#--------------------------------
#Modern UI Configuration

	!define MUI_WELCOMEPAGE
	# !define MUI_COMPONENTSPAGE
	!define MUI_DIRECTORYPAGE
	!define MUI_STARTMENUPAGE
	!define MUI_FINISHPAGE
		!define MUI_FINISHPAGE_RUN_NOTCHECKED
		!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\README.txt"
		!define MUI_FINISHPAGE_NOREBOOTSUPPORT

	!define MUI_ABORTWARNING

	!define MUI_UNINSTALLER
	!define MUI_UNCONFIRMPAGE

	# Use the modern2 layout, which permits longer section names.
	!define MUI_UI "${NSISDIR}\Contrib\UIs\modern.exe"

	# $9 is being used to store the Start Menu Folder.
	# Do not use this variable in your script (or Push/Pop it)!

	# To change this variable, use MUI_STARTMENUPAGE_VARIABLE.
	# Have a look at the Readme for info about other options (default folder,
	# registry).

	# Remember the Start Menu Folder
	!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM" 
	!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\${MUI_PRODUCT}" 
	!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
	!define TEMP $R0
  
#--------------------------------
#Languages
 
	!insertmacro MUI_LANGUAGE "English"
  
#--------------------------------
#Language Strings

	# Description
	LangString DESC_SecCopyMain ${LANG_ENGLISH} "Installs ${MUI_PRODUCT} files to the application folder."
	LangString DESC_SecCopyData ${LANG_ENGLISH} "Installs the standard AIML data files.  T"

#--------------------------------
#Reserve Files
  
	# Things that need to be extracted on first (keep these lines before any
	# File command!). Only useful for BZIP2 compression.
	!insertmacro MUI_RESERVEFILE_LANGDLL
	!insertmacro MUI_RESERVEFILE_WELCOMEFINISHPAGE

#--------------------------------
#Installer Sections

Section "!Howie (required)" SecCopyMain
	# Indicate that this section should always be installed.
	SectionIn RO

	# Set output path to the installation directory.
	SetOutPath $INSTDIR

	# Install core files.
	DetailPrint "Installing core files..."
	SetDetailsPrint textonly
	File howie.py
	File _jalice.dll
	File jalice.py
	File toc.py
	File README.txt
	SetDetailsPrint both

	# Install core data files.
	DetailPrint "Installing core data files..."
	SetDetailsPrint textonly
	SetOutPath $INSTDIR
	File std-startup.aiml
	File substitutions.xml
	SetDetailsPrint both

	# Install uninstaller program
	DetailPrint "Installing uninstaller..."
	SetDetailsPrint textonly
	SetOutPath $INSTDIR
	WriteUninstaller Uninstall.exe
	WriteRegStr HKLM \
		"Software\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" \
		"DisplayName" "${MUI_PRODUCT} ${MUI_VERSION} (remove only)"
	WriteRegStr HKLM \
		"Software\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" \
		"UninstallString" "$INSTDIR\Uninstall.exe"
	WriteRegDWORD HKLM \
		"Software\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" \
		"NoModify" 1
	WriteRegDWORD HKLM \
		"Software\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" \
		"NoRepair" 1
	WriteRegStr HKLM "Software\${MUI_PRODUCT}" "Installer Language" $LANGUAGE
	SetDetailsPrint both

	# Write Start Menu shortcuts
	!insertmacro MUI_STARTMENU_WRITE_BEGIN
	    # Create shortcuts
		DetailPrint "Installing Start Menu shortcuts..."
		SetDetailsPrint textonly
		CreateDirectory "$SMPROGRAMS\${MUI_STARTMENUPAGE_VARIABLE}"
		SetOutPath $INSTDIR # To set working directory for shortcut
		CreateShortCut "$SMPROGRAMS\${MUI_STARTMENUPAGE_VARIABLE}\Run ${MUI_PRODUCT}.lnk" \
			"$INSTDIR\howie.py" "sample.level"
		CreateShortCut "$SMPROGRAMS\${MUI_STARTMENUPAGE_VARIABLE}\Uninstall ${MUI_PRODUCT}.lnk" \
			"$INSTDIR\Uninstall.exe"
		CreateShortCut "$SMPROGRAMS\${MUI_STARTMENUPAGE_VARIABLE}\README.lnk" \
			"$INSTDIR\README.txt"

		# Write shortcut location to the registry (for Uninstaller)
		WriteRegStr HKLM "Software\${MUI_PRODUCT}" "Start Menu Folder" "${MUI_STARTMENUPAGE_VARIABLE}"
		SetDetailsPrint both
	!insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "Standard Data Files" SecCopyData
	# Install standard AIML data files
	DetailPrint "Installing Standard AIML data files..."
	SetDetailsPrint textonly
	SetOutPath $INSTDIR
	File /r standard
	SetDetailsPrint both
SectionEnd

# Display the Finish header
# Insert this macro after the sections if you are not using a finish page
!insertmacro MUI_SECTIONS_FINISHHEADER

#--------------------------------
#Installer Functions

Function .onInit
	# Splash screen
	#SetOutPath $TEMP
	#File /oname=spltmp.bmp "logo.bmp"
	#advsplash::show 2000 500 500  -1 $TEMP\spltmp

	# $0 has '1' if the user closed the splash screen early,
	# '0' if everything closed normal, and '-1' if some error occured.
	#Pop $0 
	#Delete $TEMP\spltmp.bmp


	# Language selection

	# Font
	Push Tahoma
	Push 8

	# Languages
	!insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

#--------------------------------
#Descriptions

!insertmacro MUI_FUNCTIONS_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SecCopyMain} $(DESC_SecCopyMain)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecCopyMain} $(DESC_SecCopyData)
!insertmacro MUI_FUNCTIONS_DESCRIPTION_END
 
#--------------------------------
#Uninstaller Section

Section "Uninstall"
	# Remove program directory.
	RMDir /r $INSTDIR

	# Remove shortcuts
	ReadRegStr ${TEMP} HKLM "Software\${MUI_PRODUCT}" "Start Menu Folder"

	StrCmp ${TEMP} "" noshortcuts

	RMDir /r "$SMPROGRAMS\${TEMP}"

	noshortcuts:

	# Remove registry keys
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Farmageddon"
	DeleteRegValue HKLM "Software\${MUI_PRODUCT}" "Installer Language"
	DeleteRegValue HKLM "Software\${MUI_PRODUCT}" "Start Menu Folder"

	# Display the Finish header
	!insertmacro MUI_UNFINISHHEADER
SectionEnd

#--------------------------------
#Uninstaller Functions

Function un.onInit
	# Get language from registry
	ReadRegStr $LANGUAGE HKLM "Software\${MUI_PRODUCT}" "Installer Language"
FunctionEnd