#md5 Core ZYNQ APSoC © 2024 by Filippo Borghetto, Alex Turiani, Andro Ruci is licensed under CC BY-NC 4.0 

create_bd_design "design_cryptocore"
update_compile_order -fileset sources_1

startgroup
create_bd_cell -type ip -vlnv xilinx.com:user:ip_cryptocore:1.0 ip_cryptocore_0
endgroup

ipx::edit_ip_in_project -upgrade true -name ip_cryptocore_v1_0_project -directory ./vivado/ip_md5/ip_md5.tmp/ip_cryptocore_v1_0_project ./vivado/ip_repo/ip_cryptocore_1.0/component.xml

set_property core_revision 3 [ipx::current_core]
ipx::update_source_project_archive -component [ipx::current_core]
ipx::create_xgui_files [ipx::current_core]
ipx::update_checksums [ipx::current_core]
ipx::check_integrity [ipx::current_core]
ipx::save_core [ipx::current_core]
ipx::move_temp_component_back -component [ipx::current_core]
close_project -delete

update_ip_catalog -rebuild -repo_path ./vivado/ip_repo/ip_cryptocore_1.0

report_ip_status -name ip_status 
upgrade_ip [get_ips  design_cryptocore_ip_cryptocore_0_0] -log ip_upgrade.log

export_ip_user_files -of_objects [get_ips design_cryptocore_ip_cryptocore_0_0] -no_script -sync -force -quiet

startgroup
create_bd_cell -type ip -vlnv xilinx.com:ip:processing_system7:5.5 processing_system7_0
endgroup

startgroup
create_bd_cell -type ip -vlnv xilinx.com:ip:clk_wiz:6.0 clk_wiz_0
endgroup

startgroup
set_property -dict [list CONFIG.CLKOUT1_REQUESTED_OUT_FREQ {50.000} CONFIG.MMCM_CLKOUT0_DIVIDE_F {20.000} CONFIG.CLKOUT1_JITTER {151.636}] [get_bd_cells clk_wiz_0]
endgroup

apply_bd_automation -rule xilinx.com:bd_rule:processing_system7 -config {make_external "FIXED_IO, DDR" apply_board_preset "1" Master "Disable" Slave "Disable" }  [get_bd_cells processing_system7_0]

startgroup
apply_bd_automation -rule xilinx.com:bd_rule:board -config { Manual_Source {New External Port (ACTIVE_HIGH)}}  [get_bd_pins clk_wiz_0/reset]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {Auto} Clk_slave {Auto} Clk_xbar {Auto} Master {/processing_system7_0/M_AXI_GP0} Slave {/ip_cryptocore_0/S00_AXI} ddr_seg {Auto} intc_ip {New AXI Interconnect} master_apm {0}}  [get_bd_intf_pins ip_cryptocore_0/S00_AXI]
endgroup

startgroup
set_property -dict [list CONFIG.PCW_USE_FABRIC_INTERRUPT {1} CONFIG.PCW_IRQ_F2P_INTR {1}] [get_bd_cells processing_system7_0]
endgroup    

connect_bd_net [get_bd_pins ip_cryptocore_0/irq] [get_bd_pins processing_system7_0/IRQ_F2P]
delete_bd_objs [get_bd_nets processing_system7_0_FCLK_CLK0]
delete_bd_objs [get_bd_nets reset_rtl_1]
delete_bd_objs [get_bd_ports reset_rtl]
connect_bd_net [get_bd_pins processing_system7_0/FCLK_CLK0] [get_bd_pins clk_wiz_0/clk_in1]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins ip_cryptocore_0/s00_axi_aclk]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins ps7_0_axi_periph/ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins ps7_0_axi_periph/S00_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins ps7_0_axi_periph/M00_ACLK]
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins rst_ps7_0_100M/slowest_sync_clk]
connect_bd_net [get_bd_pins clk_wiz_0/reset] [get_bd_pins rst_ps7_0_100M/peripheral_reset]
connect_bd_net [get_bd_pins processing_system7_0/M_AXI_GP0_ACLK] [get_bd_pins clk_wiz_0/clk_out1]

validate_bd_design

make_wrapper -files [get_files ./vivado/ip_md5/ip_md5.srcs/sources_1/bd/design_cryptocore/design_cryptocore.bd] -top
add_files -norecurse ./vivado/ip_md5/ip_md5.gen/sources_1/bd/design_cryptocore/hdl/design_cryptocore_wrapper.v

launch_runs impl_1 -to_step write_bitstream -jobs 4
wait_on_runs impl_1
write_hw_platform -fixed -include_bit -force -file ./out/design_cryptocore.xsa
