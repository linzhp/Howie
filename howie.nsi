# howie.nsi
#
# Author: Cort Stratton (cortATcortstrattonDOTorg)
#

!include "MUI.nsh"

#--------------------------------
#Configuration

	# Basic definitions
	Var MY_PRODUCT
	Var MY_VERSION
	!define MY_PRODUCT "Howie"
	!define MY_VERSION "0.6.0"
	
	# General
	Name "${MY_PRODUCT}"
	OutFile "..\dist\howie-install-${MY_VERSION}.exe"

	# Folder selection page
	InstallDir "$PROGRAMFILES\${MY_PRODUCT}"
	InstallDirRegKey HKLM "Software\${MY_PRODUCT}" "Install_Dir"

	# MUI stuff
	Var MUI_TEMP
	Var STARTMENU_FOLDER
	
	# Compiler configuration
	SetCompressor bzip2
	ShowInstDetails show
	ShowUninstDetails show
	InstProgressFlags smooth colored
	XPStyle on

#--------------------------------
#Modern UI Configuration

	!insertmacro MUI_PAGE_COMPONENTS
	!insertmacro MUI_PAGE_DIRECTORY
		!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM" 
		!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\${MY_PRODUCT}" 
		!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
	!insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER
	!insertmacro MUI_PAGE_INSTFILES
		!define MUI_FINISHPAGE_RUN_NOTCHECKED
		!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\README.txt"
		!define MUI_FINISHPAGE_NOREBOOTSUPPORT
	!insertmacro MUI_PAGE_FINISH
	
	!define MUI_ABORTWARNING
	  
	!insertmacro MUI_UNPAGE_CONFIRM
	!insertmacro MUI_UNPAGE_INSTFILES
  
#--------------------------------
#Languages
 
	!insertmacro MUI_LANGUAGE "English"
  
#--------------------------------
#Language Strings

	# Description
	LangString DESC_SecCopyMain ${LANG_ENGLISH} "Installs ${MY_PRODUCT} files to the application folder."
	LangString DESC_SecCopyAddOns ${LANG_ENGLISH} "Installs some optional add-on modules to extend ${MY_PRODUCT}'s capabilities."
    LangString DESC_SecCopyAcronym ${LANG_ENGLISH} "Handles 'what does X stand for' questions."
	LangString DESC_SecCopyGooglism ${LANG_ENGLISH} "Handles 'who/what/where/when is X' questions."
	LangString DESC_SecCopyRhyme ${LANG_ENGLISH} "Handles 'what rhymes with X' questions."
	
#--------------------------------
#Reserve Files
  
	# Things that need to be extracted on first (keep these lines before any
	# File command!). Only useful for BZIP2 compression.
	!insertmacro MUI_RESERVEFILE_LANGDLL

#--------------------------------
#Installer Sections

Section "!${MY_PRODUCT} (required)" SecCopyMain
	# Indicate that this section should always be installed.
	SectionIn RO

	# Set output path to the installation directory.
	SetOutPath $INSTDIR

	# Install core files.
	DetailPrint "Installing core files..."
	SetDetailsPrint textonly
	File /r dist\*
	Rename runme.exe howie.exe
	SetDetailsPrint both
	
	# Install documentation
	DetailPrint "Installing documentation..."
	SetDetailsPrint textonly
	SetOutPath $INSTDIR\docs
	File /r docs\*.html
	SetDetailsPrint both

	# Install uninstaller program
	DetailPrint "Installing uninstaller..."
	SetDetailsPrint textonly
	SetOutPath $INSTDIR
	WriteUninstaller Uninstall.exe
	WriteRegStr HKLM \
		"Software\Microsoft\Windows\CurrentVersion\Uninstall\${MY_PRODUCT}" \
		"DisplayName" "Howie ${MY_VERSION} (remove only)"
	WriteRegStr HKLM \
		"Software\Microsoft\Windows\CurrentVersion\Uninstall\${MY_PRODUCT}" \
		"UninstallString" "$INSTDIR\Uninstall.exe"
	WriteRegDWORD HKLM \
		"Software\Microsoft\Windows\CurrentVersion\Uninstall\${MY_PRODUCT}" \
		"NoModify" 1
	WriteRegDWORD HKLM \
		"Software\Microsoft\Windows\CurrentVersion\Uninstall\${MY_PRODUCT}" \
		"NoRepair" 1
	WriteRegStr HKLM "Software\${MY_PRODUCT}" "Installer Language" $LANGUAGE
	SetDetailsPrint both

	# Write Start Menu shortcuts
	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
	    # Create shortcuts
		DetailPrint "Installing Start Menu shortcuts..."
		SetDetailsPrint textonly
		CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
		SetOutPath $INSTDIR # To set working directory for shortcut		
	        CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Configure ${MY_PRODUCT}.lnk" \
        		"$INSTDIR\howie.ini"
        	CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Run ${MY_PRODUCT}.lnk" \
			"$INSTDIR\howie.exe"
		CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Run ${MY_PRODUCT} (Local Mode).lnk" \
			"$INSTDIR\howie.exe" "-l"
		CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall ${MY_PRODUCT}.lnk" \
			"$INSTDIR\Uninstall.exe"
		CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\README.lnk" \
			"$INSTDIR\README.txt"
		
		# Write shortcut location to the registry (for Uninstaller)
		WriteRegStr HKLM "Software\${MY_PRODUCT}" "Start Menu Folder" "$STARTMENU_FOLDER"
		SetDetailsPrint both
	!insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

SubSection "Add-ons" SecCopyAddOns
    Section "Acronyms" SecCopyAcronym
        # Install acronym
        DetailPrint "Installing Acronym module..."
        SetDetailsPrint textonly
        SetOutPath $INSTDIR\scripts
        File scripts\acronym.py
        SetOutPath $INSTDIR\standard
        File standard\howie-acronym.aiml
        SetDetailsPrint both    
    SectionEnd

    Section "Googlisms" SecCopyGooglism
        # Install googlism
        DetailPrint "Installing Googlism module..."
        SetDetailsPrint textonly
        SetOutPath $INSTDIR\scripts
        File scripts\googlism.py
        SetOutPath $INSTDIR\standard
        File standard\howie-googlism.aiml
        SetDetailsPrint both    
    SectionEnd
	
	# Install rhyme
	Section "Rhymes" SecCopyRhyme
        DetailPrint "Installing Rhymes module..."
        SetDetailsPrint textonly
        SetOutPath $INSTDIR\scripts
        File scripts\rhyme.py
        File scripts\*.db
        SetOutPath $INSTDIR\standard
        File standard\howie-rhyme.aiml
        SetDetailsPrint both
    SectionEnd
SubSectionEnd

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

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SecCopyMain} $(DESC_SecCopyMain)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecCopyAddOns} $(DESC_SecCopyAddOns)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecCopyAcronym} $(DESC_SecCopyAcronym)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecCopyGooglism} $(DESC_SecCopyGooglism)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecCopyRhyme} $(DESC_SecCopyRhyme)
!insertmacro MUI_FUNCTION_DESCRIPTION_END
 
#--------------------------------
#Uninstaller Section

Section "Uninstall"
	RMDir /r "$INSTDIR"
	!insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP

	Delete "$SMPROGRAMS\$MUI_TEMP\Configure ${MY_PRODUCT}.lnk"
	Delete "$SMPROGRAMS\$MUI_TEMP\Uninstall ${MY_PRODUCT}.lnk"
	Delete "$SMPROGRAMS\$MUI_TEMP\Run ${MY_PRODUCT}.lnk"
	Delete "$SMPROGRAMS\$MUI_TEMP\Run ${MY_PRODUCT} (Local Mode).lnk"
	Delete "$SMPROGRAMS\$MUI_TEMP\README.lnk"
	  
	;Delete empty start menu parent diretories
	StrCpy $MUI_TEMP "$SMPROGRAMS\$MUI_TEMP"
	 
	startMenuDeleteLoop:
		RMDir $MUI_TEMP
		GetFullPathName $MUI_TEMP "$MUI_TEMP\.."
	    
		IfErrors startMenuDeleteLoopDone
	  
		StrCmp $MUI_TEMP $SMPROGRAMS startMenuDeleteLoopDone startMenuDeleteLoop
	startMenuDeleteLoopDone:

	DeleteRegKey /ifempty HKLM "Software\${MY_PRODUCT}"
SectionEnd

#--------------------------------
#Uninstaller Functions

Function un.onInit
	# Get language from registry
	ReadRegStr $LANGUAGE HKLM "Software\${MY_PRODUCT}" "Installer Language"
FunctionEnd
