# make_it_afs.tcl
#-----------------------------------------------------------
open_bd_design {/afs/auger.mtu.edu/common/scratch/dfnitz/git_clones/uub-firmware/wp2/uub_v3/uub_v3.srcs/sources_1/bd/uub_v3/uub_v3.bd}
reset_target all [get_files  /afs/auger.mtu.edu/common/scratch/dfnitz/git_clones/uub-firmware/wp2/uub_v3/uub_v3.srcs/sources_1/bd/uub_v3/uub_v3.bd]
reset_run synth_1
launch_runs synth_1 -jobs 4
wait_on_run synth_1
launch_runs impl_1 -jobs 4
wait_on_run impl_1
launch_runs impl_1 -to_step write_bitstream -jobs 4
wait_on_run impl_1
open_run impl_1
file copy -force /afs/auger.mtu.edu/common/scratch/dfnitz/git_clones/uub-firmware/wp2/uub_v3/uub_v3.runs/impl_1/uub_v3_wrapper.sysdef /afs/auger.mtu.edu/common/scratch/dfnitz/git_clones/uub-firmware/wp2/uub_v3/uub_v3.sdk/uub_v3_wrapper.hdf

