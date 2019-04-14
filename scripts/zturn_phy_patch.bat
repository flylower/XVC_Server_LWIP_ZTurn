@echo on

call common.bat

copy xemacpsif_physpeed.c.patch.lwip202_v1.0  %VIVADO_HOME%\SDK\%VIVADO_VERSION%\data\embeddedsw\ThirdParty\sw_services\lwip202_v1_0\src\contrib\ports\xilinx\netif\
cd %VIVADO_HOME%\SDK\%VIVADO_VERSION%\data\embeddedsw\ThirdParty\sw_services\lwip202_v1_0\src\contrib\ports\xilinx\netif\ 
copy xemacpsif_physpeed.c xemacpsif_physpeed.c.bak
call %VIVADO_HOME%\Vivado\%VIVADO_VERSION%\msys\bin\patch.exe -p0 <  %VIVADO_HOME%\SDK\%VIVADO_VERSION%\data\embeddedsw\ThirdParty\sw_services\lwip202_v1_0\src\contrib\ports\xilinx\netif\xemacpsif_physpeed.c.patch.lwip202_v1.0

pause