connect -url tcp:127.0.0.1:3121
source F:/Xilinx/SDK/2018.2/scripts/sdk/util/zynqmp_utils.tcl
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Xilinx HW-FTDI-TEST FT232H 93429"} -index 1
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Xilinx HW-FTDI-TEST FT232H 93429" && level==0} -index 0
fpga -file E:/SISLAB/zcu106_work/Advanced-Embedded-System-Design-Flow-on-Zynq/sources/zcu106/lab1/lab1/lab1.sdk/system_wrapper_hw_platform_0/system_wrapper.bit
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Xilinx HW-FTDI-TEST FT232H 93429"} -index 1
loadhw -hw E:/SISLAB/zcu106_work/Advanced-Embedded-System-Design-Flow-on-Zynq/sources/zcu106/lab1/lab1/lab1.sdk/system_wrapper_hw_platform_0/system.hdf -mem-ranges [list {0x80000000 0xbfffffff} {0x400000000 0x5ffffffff} {0x1000000000 0x7fffffffff}]
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Xilinx HW-FTDI-TEST FT232H 93429"} -index 1
source E:/SISLAB/zcu106_work/Advanced-Embedded-System-Design-Flow-on-Zynq/sources/zcu106/lab1/lab1/lab1.sdk/system_wrapper_hw_platform_0/psu_init.tcl
psu_init
after 1000
psu_ps_pl_isolation_removal
after 1000
psu_ps_pl_reset_config
catch {psu_protection}
targets -set -nocase -filter {name =~"*A53*0" && jtag_cable_name =~ "Xilinx HW-FTDI-TEST FT232H 93429"} -index 1
rst -processor
dow E:/SISLAB/zcu106_work/Advanced-Embedded-System-Design-Flow-on-Zynq/sources/zcu106/lab1/lab1/lab1.sdk/lab1/Release/lab1.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~"*A53*0" && jtag_cable_name =~ "Xilinx HW-FTDI-TEST FT232H 93429"} -index 1
con
