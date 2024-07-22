# md5 Core ZYNQ APSoC 

<!-- /code_chunk_output -->

### The algorithm
MD5 is a cryptographic hash function algorithm that takes the message as input of any length and changes it into a fixed-length message of 16 bytes. MD5 algorithm stands for the message-digest algorithm. MD5 was developed as an improvement of MD4, with advanced security purposes. The output of MD5 (Digest size) is always 128 bits. MD5 was developed by Ronald Rivest in 1991. MD5 Algorithm is used for file authentication, security purposes in web applications etc. Using this algorithm, we can store our password in 128 bits format. 

### Setup
To be able to follow the next steps is needed Vivado 2022.1, Vitis 2022.1 and Petalinux 2022.1.
To setup the environment it's needed to clone the git. 
To realise this you have to run:

       git clone https://github.com/AndroRuci/emness_md5.git
and change the directory to the one just obtained:

       cd /path/to/emnsee_md5

### Generating the Hardware Design
The Hardware Design (.xsa file) is obtained by means of Vivado. The following steps allow you to 
create the project, realise the custom ip, implement the Hardware Design and, finally, to synthetize the 
hardware and obtain the file. 
To execute these operations the following commands are needed:

    source /path/to/Xilinx_tools/Vivado/2022.1/settings64.sh
    
    vivado -source scripts/ip_generation.tcl
On the folder ./out/ it's possible to find the result called: design_cryptocore.xsa . 
On Vivado, in Block Design, you can see how all the entire hardware is wrapped, or modify it if needed. Thanks to the previous steps you can obtain the following diagram: 

![Block Diagram](https://github.com/AndroRuci/emness_md5/blob/main/doc/block_diagram.png)

### Baremetal Platform Test
Inside src/ip/baremetal there is HelloWord.c that allows you to test the hardware realised. 
To do that you have to:
1. Run Vitis and choose the workspace folder
2. Click on 'Create Application Project'
3. Select 'Next' and click on 'Create a new platform from hardware (XSA)'

![Xsa Selection](https://github.com/AndroRuci/emness_md5/blob/main/doc/vitis_xsa_selection.png)

 4. Choose the 'Application project name' and select the 'ps7_cortex9_0' processor
 5. Press 'Next' and finally select an 'Empty Application(C)'
 6. Right-Click on src/ and select 'Import Sources...'

![Helloword Selection](https://github.com/AndroRuci/emness_md5/blob/main/doc/vitis_helloword_selection.png)

7. Click on the Assistant Window the '<name_project> [Application]' and the 'Build' button
8. Connect the Board with the JTAG Jump enabled and press on Debug then Run

### Padding
The cryptocore used in this project needs that the input string has to have a proper padding. 
It has been done using a program that gives you as output the right mapping of the bits.
On the folder /src/baremetal/padding you can find a file called padding.c that has to be run to obtain it.

### PetaLinux Flow
The PetaLinux flow is extensively documented in the reference guide UG1144. The steps performed are summarized below. 

1. Setup PetaLinux environment, execute the appropriate command based on your installation directory

       source <petaLinux_tool_install_dir>/settings.sh

2. Use the petalinux-create command with the project template corresponding to your hardware

        petalinux-create --type project --template zynq --name md5
        cd md5
        petalinux-config --get-hw-description <path_to_.xsa_file>

   A configuration menu opens. Configure as below:

   *→ DTG Settings → Kernel Bootargs*:
     * change *generate boot args automatically* to NO
     * update *user set kernel bootargs* to 
         
           earlycon console=ttyPS0,115200 clk_ignore_unused root=/dev/mmcblk0p2 rw rootwait cma=512M
         
   *→ Image Packaging Configuration*:
     * change *root filesystem type* to EXT4

3. Launch kernel configuration and customize the settings:
       
       petalinux-config -c kernel

   *→ CPU Power Management*: Disable CPU frequency scaling and CPU idle for development purposes. 
     * change *CPU Frequency scaling* to NO
     * change *CPU Idle* to NO

   *→ Device Drivers*:
     * change *On-Chip Interconnect management support* to YES

     * *→ Reset Controller Support*:
       * change *Simple Reset Controller Driver* to YES

     * *→ Userspace I/O drivers*:
       * change *Userspace I/O platform driver with generic IRQ handling* to YES
       * change *Userspace platform driver with generic irq and dynamic memory* to YES
       * change *Xilinx AI Engine driver* to YES

4. Root filesystem configuration:

       petalinux-config -c rootfs

   *→ Filesystem Packages → misc → packagegroup-core-buildessential*: to include basic development
   tools, such as `gcc`:
     * change *packagegroup-core-buildessential* to YES
     * change *packagegroup-core-buildessential-dev* to YES
       
   *→ Image Features*: 
     * change *package-management* to YES
     * change *debug-tweaks* to YES

5. Include a simple hello world application.

       petalinux-create -t apps --template c --name helloworld --enable


6. Build the entire system image:

       petalinux-build

7. Device tree customization:
      
       nano components/plnx_workspace/device-tree/device-tree/pl.dtsi 

   Copy amba_pl node description into /project-spec/meta-user/recipes-bsp/device-tree/files/system-user.dtsi and inside system-user.dtsi change interrupts line from
     
       interrupts = <0 29 4>;

   into
   
       interrupts = <0 29 1>;

   in order to change the sensitivity to rising edge.
   
   Rebuild the device tree and the entire system:

        petalinux-build -c device-tree
        petalinux-build

8. Create the custom kernel module:

       petalinux-create -t modules --name md5

   Starting from PetaLinux created project, change to the create module:

       cd project-spec/meta-user/recipes-modules/md5


   To generate the kernel module:
    
        cp <path_to_driver_files> ./files

   Edit md5.bb changing:
       
        SRC_URI = "file://Makefile \
                   file://md5.c \
                   file://COPYING \
                  "
   into:
       
        SRC_URI = "file://Makefile \
                   file://md5.c \
                   file://md5.h \
                   file://COPYING \
                  "

   To compile the module:

        petalinux-build -c md5

   Find the compiled module:

        find -name "md5.ko"

9. Using fdisk, as suggested in UG1144 manual, partition the sd card into 200 MB for the FAT32 bootable partition and the remaining space as EXT4 root file system partition


   To generate the files for SD card, from the PetaLinux project root change
   into:

       images/linux/

   and run:
       
       petalinux-package --boot --fsbl zynq_fsbl.elf --fpga system.bit --u-boot --force

   Then:
     * copy `BOOT.BIN`, `image.ub`, and `boot.scr` to the bootable partition
     * extract `rootfs.tar.gz` to the root file system partition; assuming the partition is mounted
       in `/media/rootfs`, run:
           
           sudo tar xvfp rootfs.tar.gz -C /media/rootfs
     * copy the module to `/media/rootfs/home/root`. 

10. Insert SD card into the board. Create a serial connection through PuTTY with Baud Rate 115200. A command prompt appears, enter both user and password as root.









## License

Documentation: [CC BY-NC 4.0](http://creativecommons.org/licenses/by-nc/4.0/)


## Sources

* https://www.geeksforgeeks.org/what-is-the-md5-algorithm/

* https://opencores.org/


[//]: # "Source definitions"
[hash-functions]: https://csrc.nist.gov/projects/hash-functions "Hash Functions by NIST CSRC" 
