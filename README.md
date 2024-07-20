## Petalinux Flow

1. Source Petalinux, Execute the appropriate command based on your installation directory:

		source <petaLinux_tool_install_dir>/settings.sh


2. Use the petalinux-create command with the project template corresponding to your hardware:

		petalinux-create --type project --template zynq --name md5


3. Enter into the created project and import hardware description:

		cd md5
		petalinux-config --get-hw-description <path_to_.xsa_file>

	
 	A configuration menu opens. Configure as below: 
	
 	->DTG Setting->Kernel Bootargs:

                Change generate boot args automatically to NO
		Update user set kernel bootargs to:

   		earlycon console=ttyPS0,115200 clk_ignore_unused root=/dev/mmcblk0p2 rw rootwait cma=512M
	
	→ Image Packaging Configuration:
		
  		change root filesystem type to EXT4
  
  
4. Launch kernel configuration to customize the settings:

		petalinux-config -c kernel

	→ CPU Power Management: Disable CPU frequency scaling and CPU idle for development purposes

	  * change CPU Frequency scaling to NO
     
          * change CPU Idle to NO


	→ Device Drivers:

	  * change On-Chip Interconnect management support to YES

	→ Reset Controller Support:

          * change Simple Reset Controller Driver to YES
	
	→ Userspace I/O drivers:

	  * change Userspace I/O platform driver with generic IRQ handling to YES
	  * change Userspace platform driver with generic irq and dynamic memory to YES
	  * change Xilinx AI Engine driver to YES
