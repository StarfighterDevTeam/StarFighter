@echo off 
echo:
echo Packager for StarFighter Win32
echo:
echo First of all, make sure the project is built with params [Release ^| Win32]
echo:
set /p input=Then press [enter]

echo:
echo:

set packagename=StarFighter
set projectsrc=.\..\StarFighter
set releasefolder=.\..\Release

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
xcopy /S %releasefolder% %packagename% /EXCLUDE:excludedextensions.txt
echo bins copied
echo:
echo:
set /p input=All done [Press enter to exit]
