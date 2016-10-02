set ProjectDir=%1
set SFMLBinDir=%~dp0\
echo "Copying DLLs"
xcopy /S %projectsrc%\Assets %packagename%\Assets /EXCLUDE:excludedextensions.txt

xcopy 