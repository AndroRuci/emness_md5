#md5 Core ZYNQ APSoC © 2024 by Filippo Borghetto, Alex Turiani, Andro Ruci is licensed under CC BY-NC 4.0 


create_project ip_md5 ./vivado/ip_md5 -part xc7z020clg400-1
set_property board_part tul.com.tw:pynq-z2:part0:1.0 [current_project]

create_peripheral xilinx.com user ip_cryptocore 1.0 -dir ./vivado/ip_md5/../ip_repo
add_peripheral_interface S00_AXI -interface_mode slave -axi_type lite [ipx::find_open_core xilinx.com:user:ip_cryptocore:1.0]
set_property VALUE 17 [ipx::get_bus_parameters WIZ_NUM_REG -of_objects [ipx::get_bus_interfaces S00_AXI -of_objects [ipx::find_open_core xilinx.com:user:ip_cryptocore:1.0]]]

generate_peripheral -driver -bfm_example_design -debug_hw_example_design [ipx::find_open_core xilinx.com:user:ip_cryptocore:1.0]
write_peripheral [ipx::find_open_core xilinx.com:user:ip_cryptocore:1.0]
set_property  ip_repo_paths ./vivado/ip_repo/ip_cryptocore_1.0 [current_project]

update_ip_catalog -rebuild

ipx::edit_ip_in_project -upgrade true -name edit_ip_cryptocore_v1_0 -directory ./vivado/ip_repo ./vivado/ip_repo/ip_cryptocore_1.0/component.xml

update_compile_order -fileset sources_1

update_files -from_files ./src/ip/v/ip_cryptocore_v1_1.v -to_files ./vivado/ip_md5/../ip_repo/ip_cryptocore_1.0/hdl/ip_cryptocore_v1_0.v -filesets [get_filesets *]
update_files -from_files ./src/ip/v/ip_cryptocore_v1_1_S00_AXI.v -to_files ./vivado/ip_md5/../ip_repo/ip_cryptocore_1.0/hdl/ip_cryptocore_v1_0_S00_AXI.v -filesets [get_filesets *]
add_files -norecurse -copy_to ./vivado/ip_repo/ip_cryptocore_1.0/src {./src/ip/v/md5.v ./src/ip/v/wrapper.v}

update_compile_order -fileset sources_1

synth_design -rtl -rtl_skip_mlo -name rtl_1

ipx::merge_project_changes files [ipx::current_core]
ipx::merge_project_changes hdl_parameters [ipx::current_core]
ipx::infer_bus_interface irq xilinx.com:signal:interrupt_rtl:1.0 [ipx::current_core]
set_property core_revision 2 [ipx::current_core]
ipx::update_source_project_archive -component [ipx::current_core]
ipx::create_xgui_files [ipx::current_core]
ipx::update_checksums [ipx::current_core]
ipx::check_integrity [ipx::current_core]
ipx::save_core [ipx::current_core]

close_project -delete

source ./scripts/platform_generation.tcl
