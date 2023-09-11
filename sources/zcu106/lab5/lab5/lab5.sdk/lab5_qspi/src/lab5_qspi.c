#include "xparameters.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xil_cache.h"
#include "xdevcfg.h"
#include "xil_io.h"
#include "xil_types.h" 

// Read function for STDIN
extern char inbyte(void);

#define XPAR_XDCFG_0_DEVICE_ID 0U
#define XPAR_XDCFG_NUM_INSTANCES 1U
#define XPAR_XDCFG_0_BASEADDR 0xF8007000U
#define PS_RST_CTRL_REG			(XPS_SYS_CTRL_BASEADDR + 0x200)
#define PS_RST_MASK			0x1	/* PS software reset */
#define SLCR_UNLOCK_OFFSET		0x8

int XDcfg_CfgInitialize(XDcfg *InstancePtr,
			 XDcfg_Config *ConfigPtr, u32 EffectiveAddress)
{
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(ConfigPtr != NULL);

	/*
	 * If the device is started, disallow the initialize and return a
	 * status indicating it is started. This allows the user to stop the
	 * device and reinitialize, but prevents a user from inadvertently
	 * initializing.
	 */
	if (InstancePtr->IsStarted == XIL_COMPONENT_IS_STARTED) {
		return XST_DEVICE_IS_STARTED;
	}

	/*
	 * Copy configuration into instance.
	 */
	InstancePtr->Config.DeviceId = ConfigPtr->DeviceId;

	/*
	 * Save the base address pointer such that the registers of the block
	 * can be accessed and indicate it has not been started yet.
	 */
	InstancePtr->Config.BaseAddr = EffectiveAddress;
	InstancePtr->IsStarted = 0;


	/* Unlock the Device Configuration Interface */
	XDcfg_Unlock(InstancePtr);

	/*
	 * Indicate the instance is ready to use, successfully initialized.
	 */
	InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

	return XST_SUCCESS;
}

XDcfg_Config *XDcfg_LookupConfig(u16 DeviceId)
	{
		extern XDcfg_Config XDcfg_ConfigTable[];
		XDcfg_Config *CfgPtr = NULL;
		u32 Index;

		for (Index = 0U; Index < XPAR_XDCFG_NUM_INSTANCES; Index++) {
			if (XDcfg_ConfigTable[Index].DeviceId == DeviceId) {
				CfgPtr = &XDcfg_ConfigTable[Index];
				break;
			}
		}

		return (CfgPtr);
	}

XDcfg_Config XDcfg_ConfigTable[1] = {
	{
		XPAR_XDCFG_0_DEVICE_ID,
		XPAR_XDCFG_0_BASEADDR,
	}
};

int main()
{
	u32 MultiBootReg = 0;
	XDcfg XDcfg_0;

	// Initialize Device Configuration Interface
	XDcfg_Config *Config = XDcfg_LookupConfig(XPAR_XDCFG_0_DEVICE_ID);
	XDcfg_CfgInitialize(&XDcfg_0, Config, Config->BaseAddr);

	// Display Menu
    int Exit = 0;
    int OptionCurr;
    int OptionNext = 1; // start-up default
	while(Exit != 1) {
		do {
			print("    1: Lab1\n\r");
			print("    2: Lab3\n\r");
			print("    0: Exit\n\r");
			print("> ");

			OptionCurr = OptionNext;
			OptionNext = inbyte();
			if (isalpha(OptionNext)) {
				OptionNext = toupper(OptionNext);
			}

			xil_printf("%c\n\r", OptionNext);
		} while (!isdigit(OptionNext));

		if (OptionCurr == OptionNext)
			continue;

		switch (OptionNext) {
			case '0':
				Exit = 1;
				break;
			case '1':
				  print("Loading Lab1 project\r\n");
				  MultiBootReg = 0x2000000/0x40000; // 0x00400000 is the base address where lab1.bin is stored
				  Xil_Out32(0xF8000000 + SLCR_UNLOCK_OFFSET, 0xDF0DDF0D); // unlock SLCR
				  XDcfg_WriteReg(XDcfg_0.Config.BaseAddr, XDCFG_MULTIBOOT_ADDR_OFFSET, MultiBootReg); // write to multiboot reg
				  // synchronize
				  __asm__(
						"dsb sy\n\t"
						"isb"
					);
				  // Generate soft reset
				  Xil_Out32(PS_RST_CTRL_REG, PS_RST_MASK);
				break;
			case '2':
				  print("Loading Lab3 project\r\n");
				  MultiBootReg = 0x4000000/0x40000; // 0x00800000 is the base address where lab3.bin is stored
				  Xil_Out32(0xF8000000 + SLCR_UNLOCK_OFFSET, 0xDF0DDF0D); // unlock SLCR
				  XDcfg_WriteReg(XDcfg_0.Config.BaseAddr, XDCFG_MULTIBOOT_ADDR_OFFSET, MultiBootReg); // write to multiboot reg
				  // synchronize
					__asm__(
						"dsb sy\n\t"
						"isb"
					);
				  // Generate soft reset
			      Xil_Out32(PS_RST_CTRL_REG, PS_RST_MASK);
				break;

				break;
			default:
				break;
		}
	}

    return 0;
}

