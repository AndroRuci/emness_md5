//md5 Core ZYNQ APSoC © 2024 by Filippo Borghetto, Alex Turiani, Andro Ruci is licensed under CC BY-NC 4.0 

/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xil_io.h"
#include "xscugic.h"
#include "ip_cryptocore.h"
#include "xparameters.h"
#include "xil_exception.h"


XScuGic IntcInstance;
static void ISR_read();


int main()
{
    init_platform();


    XScuGic_Config *IntcConfig;
    u32 status;
    xil_printf("Hello World \n");

    /* XScuGic_Config *XScuGic_LookupConfig(u16 DeviceId); */
    IntcConfig = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);


    /* XScuGic_CfgInitialize(XScuGic *InstancePtr, XScuGic_Config *ConfigPtr,
        							u32 EffectiveAddr);  */
    status = XScuGic_CfgInitialize(&IntcInstance, IntcConfig,
    		IntcConfig->CpuBaseAddress);


    /*Check if the initialize has success*/
    if(status != XST_SUCCESS) {
    	xil_printf("Interrupt controller initialization failed \n");
    	return -1;
    }
    xil_printf("Interrupt controller initialization success \n");

    /*
    XScuGic_SetPriorityTriggerType(XScuGic *InstancePtr, u32 Int_Id,
    					u8 *Priority, u8 *Trigger);
    XPAR_FABRIC_IP_CRYPTOCORE_0_IRQ_INTR
	*/
    XScuGic_SetPriorityTriggerType(&IntcInstance, XPAR_PS7_SCUGIC_0_DEVICE_ID,
        					0x0, 3);

    /* s32  XScuGic_Connect(XScuGic *InstancePtr, u32 Int_Id,
			Xil_InterruptHandler Handler, void *CallBackRef); */
    status = XScuGic_Connect(&IntcInstance, XPAR_PS7_SCUGIC_0_DEVICE_ID,
    			(Xil_InterruptHandler) ISR_read, 0);

    if(status != XST_SUCCESS) {
        	xil_printf("Interrupt connection failed \n");
        	return -1;
    }
    xil_printf("Interrupt connection success \n");

    //void XScuGic_Enable(XScuGic *InstancePtr, u32 Int_Id);
    XScuGic_Enable(&IntcInstance, XPAR_PS7_SCUGIC_0_DEVICE_ID);


    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,(Xil_ExceptionHandler) XScuGic_InterruptHandler, (void *)&IntcInstance);
    Xil_ExceptionEnable();


    u32 input_to_crypto;
    u32 status_register;


    input_to_crypto = 0x00008061;
    IP_CRYPTOCORE_mWriteReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR , IP_CRYPTOCORE_S00_AXI_SLV_REG0_OFFSET, input_to_crypto);

    input_to_crypto = 0x00000000;
    IP_CRYPTOCORE_mWriteReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR , IP_CRYPTOCORE_S00_AXI_SLV_REG1_OFFSET, input_to_crypto);
    input_to_crypto = 0x00000000;
    IP_CRYPTOCORE_mWriteReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR , IP_CRYPTOCORE_S00_AXI_SLV_REG2_OFFSET, input_to_crypto);
    input_to_crypto = 0x00000000;
    IP_CRYPTOCORE_mWriteReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR , IP_CRYPTOCORE_S00_AXI_SLV_REG3_OFFSET, input_to_crypto);
    input_to_crypto = 0x00000000;
    IP_CRYPTOCORE_mWriteReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR , IP_CRYPTOCORE_S00_AXI_SLV_REG4_OFFSET, input_to_crypto);
    input_to_crypto = 0x00000000;
    IP_CRYPTOCORE_mWriteReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR , IP_CRYPTOCORE_S00_AXI_SLV_REG5_OFFSET, input_to_crypto);
    input_to_crypto = 0x00000000;
    IP_CRYPTOCORE_mWriteReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR , IP_CRYPTOCORE_S00_AXI_SLV_REG6_OFFSET, input_to_crypto);
    input_to_crypto = 0x00000000;
    IP_CRYPTOCORE_mWriteReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR , IP_CRYPTOCORE_S00_AXI_SLV_REG7_OFFSET, input_to_crypto);
    input_to_crypto = 0x00000000;
    IP_CRYPTOCORE_mWriteReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR , IP_CRYPTOCORE_S00_AXI_SLV_REG8_OFFSET, input_to_crypto);
    input_to_crypto = 0x00000000;
    IP_CRYPTOCORE_mWriteReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR , IP_CRYPTOCORE_S00_AXI_SLV_REG9_OFFSET, input_to_crypto);
    input_to_crypto = 0x00000000;
    IP_CRYPTOCORE_mWriteReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR , IP_CRYPTOCORE_S00_AXI_SLV_REG10_OFFSET, input_to_crypto);
    input_to_crypto = 0x00000000;
    IP_CRYPTOCORE_mWriteReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR , IP_CRYPTOCORE_S00_AXI_SLV_REG11_OFFSET, input_to_crypto);
    input_to_crypto = 0x00000000;
    IP_CRYPTOCORE_mWriteReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR , IP_CRYPTOCORE_S00_AXI_SLV_REG12_OFFSET, input_to_crypto);
    input_to_crypto = 0x00000000;
    IP_CRYPTOCORE_mWriteReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR , IP_CRYPTOCORE_S00_AXI_SLV_REG13_OFFSET, input_to_crypto);
    input_to_crypto = 0x00000008;
    IP_CRYPTOCORE_mWriteReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR , IP_CRYPTOCORE_S00_AXI_SLV_REG14_OFFSET, input_to_crypto);
    input_to_crypto = 0x00000000;
    IP_CRYPTOCORE_mWriteReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR , IP_CRYPTOCORE_S00_AXI_SLV_REG15_OFFSET, input_to_crypto);


    status_register = IP_CRYPTOCORE_mReadReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR , IP_CRYPTOCORE_S00_AXI_SLV_REG16_OFFSET) | 0x80000000;
    IP_CRYPTOCORE_mWriteReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR , IP_CRYPTOCORE_S00_AXI_SLV_REG16_OFFSET, status_register);
    xil_printf("\n Registers written \n\r");

    cleanup_platform();
    return 0;
}



static void ISR_read() {

	XScuGic_Disable(&IntcInstance, XPAR_PS7_SCUGIC_0_DEVICE_ID);

	u32 crypto_status;

	// IP_CRYPTOCORE_mReadReg((BaseAddress) + (RegOffset))
	crypto_status = IP_CRYPTOCORE_mReadReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR , IP_CRYPTOCORE_S00_AXI_SLV_REG16_OFFSET);

	//Mask the status to put at zero the interrupt bit
	crypto_status = crypto_status & 0xFFFFFFFB;

	//IP_CRYPTOCORE_mWriteReg((BaseAddress) + (RegOffset), (u32)(Data))
	IP_CRYPTOCORE_mWriteReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR, IP_CRYPTOCORE_S00_AXI_SLV_REG16_OFFSET, crypto_status);

	u32 hash_0;
	u32 hash_1;
	u32 hash_2;
	u32 hash_3;

	hash_0 = IP_CRYPTOCORE_mReadReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR , IP_CRYPTOCORE_S00_AXI_SLV_REG0_OFFSET);
	hash_1 = IP_CRYPTOCORE_mReadReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR , IP_CRYPTOCORE_S00_AXI_SLV_REG1_OFFSET);
	hash_2 = IP_CRYPTOCORE_mReadReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR , IP_CRYPTOCORE_S00_AXI_SLV_REG2_OFFSET);
	hash_3 = IP_CRYPTOCORE_mReadReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR , IP_CRYPTOCORE_S00_AXI_SLV_REG3_OFFSET);

	xil_printf("The hash is: %d , %d, %d, %d \n", hash_0, hash_1, hash_2, hash_3);


	crypto_status = IP_CRYPTOCORE_mReadReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR , IP_CRYPTOCORE_S00_AXI_SLV_REG16_OFFSET) & 0x7FFFFFFF;
	IP_CRYPTOCORE_mWriteReg(XPAR_IP_CRYPTOCORE_0_S00_AXI_BASEADDR , IP_CRYPTOCORE_S00_AXI_SLV_REG16_OFFSET, crypto_status);


	XScuGic_Enable(&IntcInstance, XPAR_PS7_SCUGIC_0_DEVICE_ID);
}
