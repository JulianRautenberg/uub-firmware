# make_it_afs.tcl
#-----------------------------------------------------------
open_bd_design {/afs/auger.mtu.edu/common/scratch/dfnitz/git_clones/uub-firmware/wp2/uub_proto2/bd/uub_proto2/uub_proto2.bd}
reset_target all [get_files  /afs/auger.mtu.edu/common/scratch/dfnitz/git_clones/uub-firmware/wp2/uub_proto2/bd/uub_proto2/uub_proto2.bd]
launch_runs synth_1 -jobs 4
wait_on_run synth_1
launch_runs impl_1 -jobs 4
wait_on_run impl_1
launch_runs impl_1 -to_step write_bitstream -jobs 4
wait_on_run impl_1
open_run impl_1
file copy -force /afs/auger.mtu.edu/common/scratch/dfnitz/git_clones/uub-firmware/wp2/uub_proto2/uub_proto2.runs/impl_1/uub_proto2_wrapper.sysdef /afs/auger.mtu.edu/common/scratch/dfnitz/git_clones/uub-firmware/wp2/uub_proto2/uub_proto2.sdk/uub_proto2_wrapper.hdf

