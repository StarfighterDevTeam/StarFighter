@echo off 
echo:
echo Packager for StarFighter Win32
echo:
echo First of all, make sure the project is built with params [Release ^| Win32]
echo:
set /p input=Then press [enter]

echo:
echo:
SETLOCAL ENABLEEXTENSIONS
if "%date%A" LSS "A" (set toks=1-3) else (set toks=2-4)
for /f "tokens=2-4 delims=(-)" %%a in ('echo:^|date') do (
  for /f "tokens=%toks% delims=.-/ " %%i in ('date/t') do (
    set '%%a'=%%i
    set '%%b'=%%j
    set '%%c'=%%k))
if %'yy'% LSS 100 set 'yy'=20%'yy'%
set Today=%'yy'%-%'mm'%-%'dd'% 
ENDLOCAL & SET v_year=%'yy'%& SET v_month=%'mm'%& SET v_day=%'dd'%

For /f "tokens=1-2 delims=/:" %%a in ('time /t') do (set time=%%a%%b)

set packagename=%V_Year%%V_Month%%V_Day%_%time%
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
echo All done [Press enter to exit]
