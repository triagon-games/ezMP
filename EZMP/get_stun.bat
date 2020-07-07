wget http://www.stunprotocol.org/stunserver_win64_1_2_16.zip -P .\Stun
tar -xf .\Stun\stunserver_win64_1_2_16.zip -C .\Stun
xcopy /s .\Stun\release .\Stun
rmdir Stun\release /s /q
del /f .\Stun\stunserver_win64_1_2_16.zip