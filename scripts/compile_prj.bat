@echo off

call common.bat

Pushd %VIVADO_HOME%\VIVADO\%VIVADO_VERSION%
call %VIVADO_HOME%\VIVADO\%VIVADO_VERSION%\settings64.bat
popd

call vivado  -nolog -nojournal -notrace -mode batch -source xvc_zturn_compile.tcl
pause