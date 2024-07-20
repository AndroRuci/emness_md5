## Petalinux Flow

1. Source Petalinux, Execute the appropriate command based on your installation directory:

   	source <petaLinux_tool_install_dir>/settings.sh

3. Use the petalinux-create command with the project template corresponding to your hardware:
   
	petalinux-create --type project --template zynq --name md5

3.Enter into the created project and import hardware description:

 	cd md5
	petalinux-config --get-hw-description <path_to_.xsa_file>

A configuration menu opens. Configure as below: 
	
 	->DTG Setting->Kernel Bootargs:
	Change generate boot args automatically to NO
	Update user set kernel bootargs to:
		earlycon console=ttyPS0,115200 clk_ignore_unused root=/dev/mmcblk0p2 rw rootwait cma=512M
	
	→ Image Packaging Configuration:
		change root filesystem type to EXT4
