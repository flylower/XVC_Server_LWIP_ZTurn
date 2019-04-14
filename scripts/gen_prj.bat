@echo off

call common.bat

Pushd %VIVADO_HOME%\VIVADO\%VIVADO_VERSION%
call %VIVADO_HOME%\VIVADO\%VIVADO_VERSION%\settings64.bat
popd

del /f /s /q ..\xvc_zturn
rmdir /S /q ..\xvc_zturn
@rem mkdir ..\Wavplay
call vivado  -nolog -nojournal -notrace -mode batch -source xvc_zturn.tcl
pause