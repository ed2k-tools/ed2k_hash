;
; NSIS script for ed2k_hash
; Installs the application and creates registry entries
; for Shell extension
;
; WARNING this script expects a COPYING file in the current
; directory, with CR/LF line ends, which isn't the case of
; the file in ../..
;

Name "ed2k_hash installation"
OutFile "ed2k_hash_setup.exe"

LicenseText "Please read the following terms."
LicenseData "COPYING"

InstallDir $PROGRAMFILES\Ed2k_hash
InstallDirRegKey HKLM SOFTWARE\ed2k_hash "Install_Dir"

ComponentText "Please select optional components"
DirText "Choose a directory to install in to:"

; The stuff to install
Section "Ed2k_hash Windows GUI"
  SetOutPath $INSTDIR
  File "..\ed2k_hash-gui.exe"
  WriteRegStr HKLM SOFTWARE\ed2k_hash "Install_Dir" "$INSTDIR"

  ; shortcuts

  CreateDirectory "$SMPROGRAMS\ED2K tools"
  CreateShortCut "$SMPROGRAMS\ED2K tools\ed2k_hash.lnk" "$INSTDIR\ed2k_hash-gui.exe" "" "$INSTDIR\ed2k_hash-gui.exe" 0

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Ed2k_hash" "DisplayName" "Ed2k_hash (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Ed2k_hash" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteUninstaller "uninstall.exe"
SectionEnd

Section "Command-line tool"
  SetOutPath $INSTDIR
  File "..\ed2k_hash.exe"
SectionEnd

Section "Shell extensions"
  WriteRegStr HKCR "*\shell\hash" "" "&Hash for ed2k"
  WriteRegStr HKCR "*\shell\hash\command" "" "$INSTDIR\ed2k_hash-gui.exe %1"
  WriteRegStr HKCR "Directory\shell\hash" "" "&Hash for ed2k"
  WriteRegStr HKCR "Directory\shell\hash\command" "" "$INSTDIR\ed2k_hash-gui.exe %1"
SectionEnd

UninstallText "This will uninstall ed2k_hash. Hit next to continue."

Section "Uninstall"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Ed2k_hash"
  DeleteRegKey HKLM SOFTWARE\ed2k_hash
  DeleteRegKey HKCR "*\shell\hash"
  DeleteRegKey HKCR "Directory\shell\hash"

  Delete $INSTDIR\ed2k_hash-gui.exe
  Delete $INSTDIR\ed2k_hash.exe
  Delete $INSTDIR\uninstall.exe
  Delete "$SMPROGRAMS\ED2K tools\*.*"

  RMDir "$SMPROGRAMS\ED2K tools"
  RMDir "$INSTDIR"
SectionEnd
