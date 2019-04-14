setws .
set proj_path [pwd]
set app_name xvc_zturn
# 创建hardware 
createhw -name hw0 -hwspec ../${app_name}_wrapper.hdf
# 新建FSBL
createapp -name fsbl -app {Zynq FSBL} -proc ps7_cortexa9_0 -hwproject hw0 -os standalone
# 添加Debug macros
configapp -app fsbl define-compiler-symbols FSBL_DEBUG_DETAILED
# 启用LWIP库
setlib -bsp fsbl_bsp -lib lwip202
# update
updatemss -mss fsbl_bsp/system.mss
# 重编译bsp
regenbsp -bsp fsbl_bsp

# 新建Project
createapp -name ${app_name} -app {Empty Application} -bsp fsbl_bsp -proc ps7_cortexa9_0 -hwproject hw0 -os standalone
# 设置uart
#configbsp -bsp wavdatatran_bsp stdin ps_uart_1
#configbsp -bsp wavdatatran_bsp stdout ps_uart_1
#  
configapp -app ${app_name} -add compiler-misc {-std=c99}

set infile ${proj_path}/${app_name}/.project
set outfile ${proj_path}/${app_name}/.project.bak 
set infid [open ${infile} RDONLY]
set outfid [open ${outfile} w+]
while { [gets $infid line] != -1} {
	if {[string compare ${line} "\<\/projectDescription\>"] == 0} {
		puts $outfid "\t<linkedResources>"
		puts $outfid "\t\t<link>"
		puts $outfid "\t\t\t<name>sources</name>"
		puts $outfid "\t\t\t<type>2</type>"
		puts $outfid "\t\t\t<locationURI>PARENT-1-WORKSPACE_LOC/source/sdk_source</locationURI>"
		puts $outfid "\t\t</link>"
		puts $outfid "\t</linkedResources>"
	}
	puts $outfid ${line}
}
close $infid
close $outfid
# setws -switch $::env(TEMP)
file delete -force ${proj_path}/${app_name}/.project
file copy -force ${proj_path}/${app_name}/.project.bak ${proj_path}/${app_name}/.project
file delete -force ${proj_path}/${app_name}/.project.bak
# setws -switch $proj_path

# file copy -force ../source/sdk/wavtran.c wavdatatran/src/
# file delete wavdatatran/src/helloworld.c
# set eclipse $::env(RDI_APPROOT)/eclipse/$::env(RDI_PLATFORM)$::env(RDI_OPT_EXT)/eclipsec
# set vm $::env(RDI_JAVAROOT)/bin
# 
# sdk setws -switch $::env(TEMP)
# exec $eclipse -vm $vm -nosplash                                    \
#     -application org.eclipse.cdt.managedbuilder.core.headlessbuild \
#     -import "file://WORKSPACE_LOC/../source/sdk" -data $proj_path
# 	
# sdk setws -switch $proj_path

projects -build


# exec bootgen -arch zynq -image output.bif -w -o BOOT.bin
# exec program_flash -f /tmp/wrk/BOOT.bin -flash_type qspi_single -blank_check -verify -cable type xilinx_tcf url tcp:localhost:3121
