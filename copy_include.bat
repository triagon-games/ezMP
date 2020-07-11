cd ..
ROBOCOPY . .\build\%1\%2\include\ *.h /S /XD build
if %errorlevel% lss 16 exit /b 0x00