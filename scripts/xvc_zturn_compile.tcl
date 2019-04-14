# Set the reference directory for source file relative paths (by default the value is script directory path)
set origin_dir "../"
set root "[file normalize "$origin_dir/"]"

set _xil_proj_name_ "xvc_zturn"
open_project ${root}/${_xil_proj_name_}/${_xil_proj_name_}.xpr
puts "INFO: Synthesis Start"
launch_runs impl_1 -to_step write_bitstream -jobs 2
wait_on_run impl_1
file copy -force ${root}/${_xil_proj_name_}/${_xil_proj_name_}.runs/impl_1/${_xil_proj_name_}_wrapper.sysdef ${root}/design_1_wrapper.hdf