@echo off
call common.bat
rmdir /s/q ..\sdk
mkdir ..\sdk
cd ..\sdk
call %VIVADO_HOME%\SDK\%VIVADO_VERSION%\bin\xsct.bat ../scripts/xvc_zturn_sdk.tcl
pause