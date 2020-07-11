cd ..
ROBOCOPY . .\build\%1\%2\include\ *.h /S /XD build
set errorlevel=0
if %errorlevel% lss 16 exit 0