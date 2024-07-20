# md5 Core Linux Driver on a ZYNQ APSoC 


  

## Contents

<!-- @import "[TOC]" {cmd="toc" depthFrom=2 depthTo=6 orderedList=false} -->

<!-- code_chunk_output -->

- [Contributors](#contributors)
- [Project Advancement](#project-advancement)
- [Contents](#contents)
- [What is this?](#what-is-this)
- [Getting Started](#getting-started)
  - [Generating the Hardware Design](#generating-the-hardware-design)
  - [Baremetal Platform Test](#baremetal-platform-test)
  - [PetaLinux Flow](#petalinux-flow)
- [Deploy from Release](#deploy-from-release)
- [Usage (TUL PYNQ-Z2 board)](#usage-tul-pynq-z2-board)
- [The Driver](#the-driver)
  - [The Platform Device API](#the-platform-device-api)
  - [Device Trees](#device-trees)
- [License](#license)
- [Sources](#sources)

<!-- /code_chunk_output -->

## What is this?


## Getting Started


### Generating the Hardware Design


### Baremetal Platform Test

### PetaLinux Flow
The PetaLinux flow is extensively documented in the reference guide [UG1144][amd-doc]. The steps for
a custom hardware platform are summarized in the table below.

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

5. Build the entire system image:

       petalinux-build

6. Device tree customization:
      
       nano components/plnx_workspace/device-tree/device-tree/pl.dtsi 

   Copy amba_pl node description into /project-spec/meta-user/recipes-bsp/device-tree/files/system-user.dtsi and inside system-user.dtsi change interrupts line from
     
       interrupts = <0 29 4>;

   into
   
       interrupts = <0 29 1>;

   in order to change the sensitivity to rising edge.
   
   Rebuild the device tree and the entire system:

        petalinux-build -c device-tree
        petalinux-build

7. Create the custom kernel module:

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

8. Using fdisk, as suggested in [UG1144][amd-doc] partition the sd card into ~ 200 MB for the FAT32 bootable partition and the remaining space as EXT4 root file system partition


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

9. Insert SD card into the board. Create a serial connection through PuTTY with Baud Rate 115200. A command prompt appears, enter both user and password as root.



### The algorithm


## The Driver




## License

Code: [GNU GPLv2](https://choosealicense.com/licenses/gpl-2.0/)

Documentation: [CC BY-NC 4.0](http://creativecommons.org/licenses/by-nc/4.0/)


## Sources

* SHA-256: [Hash Functions][hash-functions], [Secure Hash Standard][sh-standard]

* [SHA-256 hardware implementation][sha256-core]

* [AMD Documentation Portal][amd-doc]

* Theory: 
  
    * [OS Concepts][os-concepts]

    * Device drivers: [Introduction to Linux Device Drivers][amd-pltfm], [The platform device API][pltfm-api],
      [Platform devices and device trees][pltfm-dev] and [Kernel, drivers and embedded Linux development][dev-trees]

[//]: # "Source definitions"
[hash-functions]: https://csrc.nist.gov/projects/hash-functions "Hash Functions by NIST CSRC" 
