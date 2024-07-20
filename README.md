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
The algorithm is the SHA-256 (Secure Hash Algorithm 256), one of the cryptographic hash functions designed
by the United States National Security Agency (NSA). All the SHA algorithms work in a similar way: they
take as input a message of indefinite length (for example, a file content) and they deliver as output a
digest (or hash) on a fixed number of bytes. The input message is divided into blocks and each block is then
divided into words. SHA-256 requires blocks of 512 bits (64 bytes), divided into 16 32-bit words. The message
length (on 64 bits) is appended at the end of the message (thus the maximum length of the message is equal to
$2^{64}$ bits, which is a really big size, unreachable from a practical point of view). Between the message and
the length a ‘1’ bit is added followed by as many zeros as needed to have an overall message with a length equal
to a multiple of 512 bits.

The hash is on 256 bits (32 bytes), divided into 8 32-bit words. One message block at a time is sent to the
algorithm: the hash computed for each block is used as the starting point to compute the hash for the following
block. The hash for the first block is computed starting from an initial hash, defined by the algorithm standard
(it is obtained by taking the first 32 bits of the fractional parts of the square root of the first 8 prime
numbers). After last block is read, the algorithm provides the final hash.

Cryptographic functions like the SHA-256 can be used for a lot of applications, for example to verify the integrity
of a message or file, to verify the correctness of a password or to reliably identify a file. They are called
secure because, for a given algorithm, it is computationally infeasible:
 * to find a message that corresponds to a given message digest, or
 * to find two different messages that produce the same message digest. 

To learn more about how the algorithm is implemented it is suggested to have a look
to the [algorithm standard](http://dx.doi.org/10.6028/NIST.FIPS.180-4).

<p align="center">
<img src="doc/hash_diagram.png" width="800">  
</p>

### The Programmer's Model

If programming in a bare metal environment, the accelerator must be accessed through the following
memory-mapped registers: CSR is the control & status register, WXX registers are used to store
the 512-bit block, while HX registers store the computed digest.

| Register Name | Bit Field and Description          |
|---------------|------------------------------------|
| CSR           | <table><thead><tr><th>31</th><th>30</th><th>29</th><th>10</th><th>9</th><th>8</th><th>7:4</th><th>3:2</th><th>1</th><th>0</th></tr></thead><tbody><tr><td>ERR</td><td>DONE</td><td>BLOCKREAD</td><td>IENABLE</td><td>WR</td><td>LAST</td><td>LASTWORD</td><td>VALIDBYTE</td><td>NEWHASH</td><td>COREENABLE</td></tr></tbody></table>ERR: read-only bit raised by the crypto core if some error occurs.<br/>DONE: read-only bit raised by the crypto core when the hash computation of the entire message has terminated.<br/>BLOCKREAD: read-only bit raised by the crypto core when an entire block has been read (it is not raised if the block is the last one); it is automatically cleared when writing '1' on WR.<br/>IENABLE: this bit must be set by the programmer if he wants to use interrupts.<br/>WR: this bit must be set to '1' to tell the core that a new block is available in the registers; it is automatically cleared after all the words have been read by the core.<br/>LAST: this bit must be set by the programmer before writing '1' on WR to indicate that the block the core is going to read is the last one; it is automatically cleared when writing '1' on NEWHASH.<br/>LASTWORD: 4 bits to indicate $N_{words}-1$, where $N_{words}$ is the number of valid words inside the block.<br/>VALIDBYTE: 2 bits to indicate how many bytes are part of the message in last word ("00" if all bytes are valid).<br/>NEWHASH: this bit must be set before starting the computation of a new hash to reset the crypto core; it is automatically cleared after one clock cycle.<br/>COREENABLE: this bit must be set to enable the crypto core.
| WXX           | W0: most significant word of the 512-bit block <br/>W15: least significant word of the 512 bit block. 
| HX            | H7: most significant word of the 256-bit hash <br/>H0: least significant word of the 256-bit hash

If programming in a Linux environment, there is no need of knowing the registers:
the driver provides all the functionalities to compute the hash of a message or
a concatenation of more messages. Like all device drivers in Linux, it is sufficient
to use some system calls:
* open(): it enables the core.
* close(): it disables the core.
* ioctl(): to choose between concatenated (writing 1) or non-concatenated mode.
 (writing 0). Please remember to always disable concatenation mode when it is no more required.
* lseek(): allows to reset the core (if final position == 0) or to add some bytes filled
 with 0s (if final position > current position).
* write(): to send the message from which the hash must be computed. Writing a number
 of bytes equal to 0 allows some special functionalities (summarized in the graph).
* read(): to read the computed hash; if this system call is invoked while the core is still
 computing the hash, it waits until the result is available (unless O_NONBLOCK was set while
 opening the file).

The following graph summarize how to use the system calls to send the message to the core, in both
concatenated and non-concatenated mode.

<p align="center">
<img src="doc/Instruction_scheme.png" width="900">
</p>


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
[sh-standard]: http://dx.doi.org/10.6028/NIST.FIPS.180-4 "FIPS PUB 180-4 Secure Hash Standard"
[sha256-core]: https://opencores.org/projects/sha256_hash_core "SHA-256 HASH CORE"
[amd-doc]: https://docs.xilinx.com/ "AMD Documentation Portal"
[os-concepts]: https://www.os-book.com/OS10/ "Operating System Concepts"
[pltfm-api]: https://lwn.net/Articles/448499/ "The platform device API"
[pltfm-dev]: https://lwn.net/Articles/448502/ "Platform devices and device trees"
[dev-trees]: https://elinux.org/images/f/f9/Petazzoni-device-tree-dummies_0.pdf "Kernel, drivers and embedded Linux development"
[amd-pltfm]: https://www.xilinx.com/video/soc/linux-device-drivers-part-2-platform-character-drivers.html "Introduction to Linux Device Drivers"
