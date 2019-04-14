@echo off

echo 1.Generate Fpga project
echo 2.Generate Fpga project And Generate Bit
echo 3.Generate sdk project
echo 4.Generate Fpga project And Generate Bit, generate sdk project

set/p a1=which options:

if %a1% == 1 goto 1
if %a1% == 2 goto 2
if %a1% == 3 goto 3
if %a1% == 4 goto 4

:1
call gen_prj.bat
goto :eof



:2
call gen_prj.bat
call compile_prj.bat
goto :eof

:3
call gen_sdk.bat 
goto :eof

:4
call gen_prj.bat
call compile_prj.bat
call gen_sdk.bat 

:eof
pause