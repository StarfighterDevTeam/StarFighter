@echo off 
echo:
echo Packager for %1 Win32
echo:
echo First of all, make sure the project is built with params [Release ^| Win32]
echo:
set /p input=Then press [enter]

echo:
echo:

set packagename=%1
set projectsrc=..\%1
set exename=%1.exe
set vcredistfolder=redist-vs2015

echo Creating folder %packagename%
rd /S /Q %packagename%
mkdir %packagename%
mkdir %packagename%\Assets
mkdir %packagename%\Saves

echo Copying Assets
xcopy /S %projectsrc%\Assets %packagename%\Assets /EXCLUDE:excludedextensions.txt
echo Assets copied
echo:
echo Copying Saves
xcopy /S %projectsrc%\Saves %packagename%\Saves /EXCLUDE:excludedextensions.txt
echo Saves copied
echo:
echo Copying bins
xcopy /S %projectsrc%\%exename% %releasefolder% %packagename% /EXCLUDE:excludedextensions.txt
xcopy /S %projectsrc%\openal32.dll %releasefolder% %packagename% /EXCLUDE:excludedextensions.txt
echo bins copied
echo:
echo Copying VC Redist
xcopy /S %vcredistfolder% %packagename%
echo VC Redist copied
echo:
echo:
set /p input=All done [Press enter to exit]
