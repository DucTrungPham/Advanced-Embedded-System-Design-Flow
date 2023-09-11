#include "xparameters.h"
#include "xgpio.h"
#ifdef MULTIBOOT
#include "xdevcfg.h"
#endif

//====================================================
#define XPAR_XDCFG_0_DEVICE_ID 0U
#define XPAR_XDCFG_NUM_INSTANCES 1U
#define XPAR_XDCFG_0_BASEADDR 0xF8007000U

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

int main (void) 
{

      XGpio sws, leds, btns;
	  int i, sws_check, btns_check;
	
	  xil_printf("-- Start of the Program --\r\n");

	  // AXI GPIO switches Initialization
	  XGpio_Initialize(&sws, XPAR_SWITCHES_DEVICE_ID);
	  XGpio_SetDataDirection(&sws, 1, 0xffffffff);		// input
	  // AXI GPIO leds Initialization
	  XGpio_Initialize(&leds, XPAR_LEDS_DEVICE_ID);
	  XGpio_SetDataDirection(&leds, 1, 0);				// output
	  // AXI GPIO buttons Initialization
	  XGpio_Initialize(&btns, XPAR_BUTTONS_DEVICE_ID);
	  XGpio_SetDataDirection(&btns, 1, 0xffffffff);		// input

	  xil_printf("-- Press any of BTN0-BTN3 to see corresponding output on LEDs --\r\n");
	  xil_printf("-- Set slide switches to 0x03 to exit the program --\r\n");

	  while (1)
	  {
		  btns_check = XGpio_DiscreteRead(&btns, 1);
		  XGpio_DiscreteWrite(&leds, 1, btns_check);
	      sws_check = XGpio_DiscreteRead(&sws,1);
          if((sws_check & 0x03)==0x03)
        	  break;
		  for (i=0; i<9999999; i++); // delay loop
	   }
	  xil_printf("-- End of Program --\r\n");
#ifdef MULTIBOOT
	  // Driver Instantiations
	  XDcfg XDcfg_0;
	  u32 MultiBootReg = 0;
	  #define PS_RST_CTRL_REG	(XPS_SYS_CTRL_BASEADDR + 0x200)
	  #define PS_RST_MASK	0x1	/* PS software reset */
	  #define SLCR_UNLOCK_OFFSET 0x08

	  // Initialize Device Configuration Interface
	  XDcfg_Config *Config = XDcfg_LookupConfig(XPAR_XDCFG_0_DEVICE_ID);
	  XDcfg_CfgInitialize(&XDcfg_0, Config, Config->BaseAddr);

	  MultiBootReg = 0; // Once done, boot the master image stored at 0xfc00_0000
	  Xil_Out32(0xF8000000 + SLCR_UNLOCK_OFFSET, 0xDF0DDF0D); // unlock SLCR
	  XDcfg_WriteReg(XDcfg_0.Config.BaseAddr, XDCFG_MULTIBOOT_ADDR_OFFSET, MultiBootReg); // write to multiboot reg
	  // synchronize
		__asm__(
			"dsb sy\n\t"
			"isb"
		);
      Xil_Out32(PS_RST_CTRL_REG, PS_RST_MASK);
#endif
	  return 0;
}
 
