@echo off
if exist .\build\%1\%2\bin\Stun\stunclient.exe (
@echo Stun is already installed
exit 0
) else ( 
	wget http://www.stunprotocol.org/stunserver_win64_1_2_16.zip -P .\build\%1\%2\bin\Stun
	tar -xf .\build\%1\%2\bin\Stun\stunserver_win64_1_2_16.zip -C .\build\%1\%2\bin\Stun
	xcopy /s .\build\%1\%2\bin\Stun\release .\build\%1\%2\bin\Stun
	rmdir .\build\%1\%2\bin\Stun\release /s /q
	del /f .\build\%1\%2\bin\Stun\stunserver_win64_1_2_16.zip
)