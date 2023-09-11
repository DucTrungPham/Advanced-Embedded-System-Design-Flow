/*
 * devcfg.c
 *
 *  Created on: Aug 15, 2012
 *      Author: ckohn
 */

#include <stdlib.h>
#include "devcfg.h"
#include "xil_printf.h"


/*
 * SLCR registers
 */
#define XPAR_XDCFG_0_DEVICE_ID 0U
#define XPAR_XDCFG_NUM_INSTANCES 1U
#define XPAR_XDCFG_0_BASEADDR 0xF8007000U
#define SLCR_LOCK	   0xF8000004 /**< SLCR Write Protection Lock */
#define SLCR_UNLOCK	   0xF8000008 /**< SLCR Write Protection Unlock */
#define FPGA_RST_CTRL  0xF8000240 /**< FPGA Software Reset Control */
#define LVL_SHFTR_EN   0xF8000900 /**< FPGA Level Shifters Enable */

#define SLCR_LOCK_VAL	0x767B
#define SLCR_UNLOCK_VAL	0xDF0D


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

void XDcfg_InitiateDma(XDcfg *InstancePtr, u32 SourcePtr, u32 DestPtr,
				u32 SrcWordLength, u32 DestWordLength)
{

	XDcfg_WriteReg(InstancePtr->Config.BaseAddr,
				XDCFG_DMA_SRC_ADDR_OFFSET,
				SourcePtr);

	XDcfg_WriteReg(InstancePtr->Config.BaseAddr,
				XDCFG_DMA_DEST_ADDR_OFFSET,
				DestPtr);

	XDcfg_WriteReg(InstancePtr->Config.BaseAddr,
				XDCFG_DMA_SRC_LEN_OFFSET,
				SrcWordLength);

	XDcfg_WriteReg(InstancePtr->Config.BaseAddr,
				XDCFG_DMA_DEST_LEN_OFFSET,
				DestWordLength);
}

void XDcfg_IntrEnable(XDcfg *InstancePtr, u32 Mask)
{
	u32 RegValue;

	/*
	 * Assert the arguments.
	 */
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	/*
	 * Enable the specified interrupts in the Interrupt Mask Register.
	 */
	RegValue = XDcfg_ReadReg(InstancePtr->Config.BaseAddr,
				    XDCFG_INT_MASK_OFFSET);
	RegValue &= ~(Mask & XDCFG_IXR_ALL_MASK);
	XDcfg_WriteReg(InstancePtr->Config.BaseAddr,
				XDCFG_INT_MASK_OFFSET,
				RegValue);
}

void XDcfg_IntrDisable(XDcfg *InstancePtr, u32 Mask)
{
	u32 RegValue;

	/*
	 * Assert the arguments.
	 */
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	/*
	 * Disable the specified interrupts in the Interrupt Mask Register.
	 */
	RegValue = XDcfg_ReadReg(InstancePtr->Config.BaseAddr,
				    XDCFG_INT_MASK_OFFSET);
	RegValue |= (Mask & XDCFG_IXR_ALL_MASK);
	XDcfg_WriteReg(InstancePtr->Config.BaseAddr,
				XDCFG_INT_MASK_OFFSET,
				RegValue);
}


XDcfg *XDcfg_Initialize(u16 DeviceId)
{
	XDcfg *Instance = malloc(sizeof *Instance);
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
	XDcfg_Config *Config = XDcfg_LookupConfig(DeviceId);
	XDcfg_CfgInitialize(Instance, Config, Config->BaseAddr);

	return Instance;
}

XDcfg_Config XDcfg_ConfigTable[1] = {
	{
		XPAR_XDCFG_0_DEVICE_ID,
		XPAR_XDCFG_0_BASEADDR,
	}
};

u32 XDcfg_IntrGetEnabled(XDcfg *InstancePtr)
{
	/*
	 * Assert the arguments.
	 */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	/*
	 * Return the value read from the Interrupt Mask Register.
	 */
	return (~ XDcfg_ReadReg(InstancePtr->Config.BaseAddr,
				XDCFG_INT_MASK_OFFSET));
}

static u32 XDcfg_PcapReadback(XDcfg *InstancePtr, u32 SourcePtr,
				u32 SrcWordLength, u32 DestPtr,
				u32 DestWordLength)
{
	u32 IntrReg;

	/*
	 * Send READ Frame command to FPGA
	 */
	XDcfg_InitiateDma(InstancePtr, SourcePtr, XDCFG_DMA_INVALID_ADDRESS,
				SrcWordLength, 0);

	/*
	 * Store the enabled interrupts to enable before the actual read
	 * transfer is initiated and Disable all the interrupts temporarily.
	 */
	IntrReg = XDcfg_IntrGetEnabled(InstancePtr);
	XDcfg_IntrDisable(InstancePtr, XDCFG_IXR_ALL_MASK);

	/*
	 * Wait till you get the DMA done for the read command sent
	 */
	 while ((XDcfg_ReadReg(InstancePtr->Config.BaseAddr,
			XDCFG_INT_STS_OFFSET) &
			XDCFG_IXR_D_P_DONE_MASK) !=
			XDCFG_IXR_D_P_DONE_MASK);
	/*
	 * Enable the previously stored Interrupts .
	 */
	XDcfg_IntrEnable(InstancePtr, IntrReg);

	/*
	 * Initiate the DMA write command.
	 */
	XDcfg_InitiateDma(InstancePtr, XDCFG_DMA_INVALID_ADDRESS, (u32)DestPtr,
				0, DestWordLength);

	return XST_SUCCESS;
}

int XDcfg_TransferBitfile(XDcfg *Instance, u32 StartAddress, u32 WordLength)
{
	int Status;
	volatile u32 IntrStsReg = 0;

	// TODO : not working although suggested procedure per TRM
	// Disable AXI Interface and Output Level Shifters (Input Level Shifters are still enabled)
//	Xil_Out32(SLCR_UNLOCK, SLCR_UNLOCK_VAL);
//	Xil_Out32(FPGA_RST_CTRL, 0xFFFFFFFF);
//	Xil_Out32(LVL_SHFTR_EN, 0xA);
//	Xil_Out32(SLCR_LOCK, SLCR_LOCK_VAL);

	void XDcfg_SetControlRegister(XDcfg *InstancePtr, u32 Mask)
	{
		u32 CtrlReg;
		/*
		 * Assert the arguments.
		 */
		Xil_AssertVoid(InstancePtr != NULL);
		Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);


		CtrlReg = XDcfg_ReadReg(InstancePtr->Config.BaseAddr,
						XDCFG_CTRL_OFFSET);

		XDcfg_WriteReg(InstancePtr->Config.BaseAddr, XDCFG_CTRL_OFFSET,
				(CtrlReg | Mask));

	}

	// Clear DMA and PCAP Done Interrupts
	void XDcfg_IntrClear(XDcfg *InstancePtr, u32 Mask)
	{
		/*
		 * Assert the arguments.
		 */
		Xil_AssertVoid(InstancePtr != NULL);
		Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

		XDcfg_WriteReg(InstancePtr->Config.BaseAddr,
					XDCFG_INT_STS_OFFSET,
					Mask);

	}
	XDcfg_IntrClear(Instance, (XDCFG_IXR_DMA_DONE_MASK | XDCFG_IXR_D_P_DONE_MASK));

	// Transfer bitstream from DDR into fabric in non secure mode
	u32 XDcfg_Transfer(XDcfg *InstancePtr,
				void *SourcePtr, u32 SrcWordLength,
				void *DestPtr, u32 DestWordLength,
				u32 TransferType)
	{

		u32 CtrlReg;

		Xil_AssertNonvoid(InstancePtr != NULL);
		Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);


		u32 XDcfg_IsDmaBusy(XDcfg *InstancePtr)
		{

			u32 RegData;

			Xil_AssertNonvoid(InstancePtr != NULL);
			Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

			/* Read the PCAP status register for DMA status */
			RegData = XDcfg_ReadReg(InstancePtr->Config.BaseAddr,
							XDCFG_STATUS_OFFSET);

			if ((RegData & XDCFG_STATUS_DMA_CMD_Q_F_MASK) ==
						XDCFG_STATUS_DMA_CMD_Q_F_MASK){
				return XST_SUCCESS;
			}

			return XST_FAILURE;
		}
		if (XDcfg_IsDmaBusy(InstancePtr) == XST_SUCCESS) {
			return XST_DEVICE_BUSY;
		}

		/*
		 * Check whether the fabric is in initialized state
		 */
		if ((XDcfg_ReadReg(InstancePtr->Config.BaseAddr, XDCFG_STATUS_OFFSET)
				& XDCFG_STATUS_PCFG_INIT_MASK) == 0) {
			/*
			 * We don't need to check PCFG_INIT to be high for
			 * non-encrypted loopback transfers.
			 */
			if (TransferType != XDCFG_CONCURRENT_NONSEC_READ_WRITE) {
				return XST_FAILURE;
			}
		}

		if ((TransferType == XDCFG_SECURE_PCAP_WRITE) ||
			(TransferType == XDCFG_NON_SECURE_PCAP_WRITE)) {

			/* Check for valid source pointer and length */
			if ((!SourcePtr) || (SrcWordLength == 0)) {
				return XST_INVALID_PARAM;
			}

			/* Clear internal PCAP loopback */
			CtrlReg = XDcfg_ReadReg(InstancePtr->Config.BaseAddr,
						XDCFG_MCTRL_OFFSET);
			XDcfg_WriteReg(InstancePtr->Config.BaseAddr,
					XDCFG_MCTRL_OFFSET, (CtrlReg &
					~(XDCFG_MCTRL_PCAP_LPBK_MASK)));

			if (TransferType == XDCFG_NON_SECURE_PCAP_WRITE) {
				/*
				 * Clear QUARTER_PCAP_RATE_EN bit
				 * so that the PCAP data is transmitted every clock
				 */
				CtrlReg = XDcfg_ReadReg(InstancePtr->Config.BaseAddr,
							XDCFG_CTRL_OFFSET);

				XDcfg_WriteReg(InstancePtr->Config.BaseAddr,
						XDCFG_CTRL_OFFSET, (CtrlReg &
						  ~XDCFG_CTRL_PCAP_RATE_EN_MASK));

			}
			if (TransferType == XDCFG_SECURE_PCAP_WRITE) {
				/*
				 * AES engine handles only 8 bit data every clock cycle.
				 * Hence, Encrypted PCAP data which is 32 bit data can
				 * only be sent in every 4 clock cycles. Set the control
				 * register QUARTER_PCAP_RATE_EN bit to achieve this
				 * operation.
				 */

				XDcfg_SetControlRegister(InstancePtr,
							XDCFG_CTRL_PCAP_RATE_EN_MASK);
			}

			XDcfg_InitiateDma(InstancePtr, (u32)SourcePtr,
					(u32)DestPtr, SrcWordLength, DestWordLength);

		}

		if (TransferType == XDCFG_PCAP_READBACK) {

			if ((!DestPtr) || (DestWordLength == 0)) {

				return XST_INVALID_PARAM;
			}

			/* Clear internal PCAP loopback */
			CtrlReg = XDcfg_ReadReg(InstancePtr->Config.BaseAddr,
						XDCFG_MCTRL_OFFSET);
			XDcfg_WriteReg(InstancePtr->Config.BaseAddr,
					XDCFG_MCTRL_OFFSET, (CtrlReg &
					~(XDCFG_MCTRL_PCAP_LPBK_MASK)));

			/*
			 * For PCAP readback of FPGA configuration register or memory,
			 * the read command is first sent (written) to the FPGA fabric
			 * which responds by returning the required read data. Read data
			 * from the FPGA is captured if pcap_radata_v is active.A DMA
			 * read transfer is required to obtain the readback command,
			 * which is then sent to the FPGA, followed by a DMA write
			 * transfer to support this mode of operation.
			 */
			return XDcfg_PcapReadback(InstancePtr,
						 (u32)SourcePtr, SrcWordLength,
						 (u32)DestPtr, 	 DestWordLength);
		}


		if ((TransferType == XDCFG_CONCURRENT_SECURE_READ_WRITE) ||
			(TransferType == XDCFG_CONCURRENT_NONSEC_READ_WRITE)) {

			if ((!SourcePtr) || (SrcWordLength == 0) ||
				(!DestPtr) || (DestWordLength == 0)) {
				return XST_INVALID_PARAM;
			}

			if (TransferType == XDCFG_CONCURRENT_NONSEC_READ_WRITE) {
				/* Enable internal PCAP loopback */
				CtrlReg = XDcfg_ReadReg(InstancePtr->Config.BaseAddr,
						XDCFG_MCTRL_OFFSET);
				XDcfg_WriteReg(InstancePtr->Config.BaseAddr,
						XDCFG_MCTRL_OFFSET, (CtrlReg |
						XDCFG_MCTRL_PCAP_LPBK_MASK));

				/*
				 * Clear QUARTER_PCAP_RATE_EN bit
				 * so that the PCAP data is transmitted every clock
				 */
				CtrlReg = XDcfg_ReadReg(InstancePtr->Config.BaseAddr,
							XDCFG_CTRL_OFFSET);

				XDcfg_WriteReg(InstancePtr->Config.BaseAddr,
						XDCFG_CTRL_OFFSET, (CtrlReg &
						  ~XDCFG_CTRL_PCAP_RATE_EN_MASK));

			}
			if (TransferType == XDCFG_CONCURRENT_SECURE_READ_WRITE) {
				/* Clear internal PCAP loopback */
				CtrlReg = XDcfg_ReadReg(InstancePtr->Config.BaseAddr,
							XDCFG_MCTRL_OFFSET);
				XDcfg_WriteReg(InstancePtr->Config.BaseAddr,
						XDCFG_MCTRL_OFFSET, (CtrlReg &
						~(XDCFG_MCTRL_PCAP_LPBK_MASK)));

				/*
				 * Set the QUARTER_PCAP_RATE_EN bit
				 * so that the PCAP data is transmitted every 4 clock
				 * cycles, this is required for encrypted data.
				 */
				XDcfg_SetControlRegister(InstancePtr,
						XDCFG_CTRL_PCAP_RATE_EN_MASK);
			}

			XDcfg_InitiateDma(InstancePtr, (u32)SourcePtr,
					(u32)DestPtr, SrcWordLength, DestWordLength);
		}

		return XST_SUCCESS;
	}
	Status = XDcfg_Transfer(Instance, (u32 *) StartAddress, WordLength,	(u32 *) XDCFG_DMA_INVALID_ADDRESS, 0, XDCFG_NON_SECURE_PCAP_WRITE);
	if (Status != XST_SUCCESS)
		return Status;

	u32 XDcfg_IntrGetStatus(XDcfg *InstancePtr)
	{
		/*
		 * Assert the arguments.
		 */
		Xil_AssertNonvoid(InstancePtr != NULL);
		Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

		/*
		 * Return the value read from the Interrupt Status register.
		 */
		return XDcfg_ReadReg(InstancePtr->Config.BaseAddr,
					XDCFG_INT_STS_OFFSET);
	}
	// Poll DMA Done Interrupt
	while ((IntrStsReg & XDCFG_IXR_DMA_DONE_MASK) != XDCFG_IXR_DMA_DONE_MASK)
		IntrStsReg = XDcfg_IntrGetStatus(Instance);

	// Poll PCAP Done Interrupt
	while ((IntrStsReg & XDCFG_IXR_D_P_DONE_MASK) != XDCFG_IXR_D_P_DONE_MASK)
		IntrStsReg = XDcfg_IntrGetStatus(Instance);

	// Enable AXI Interface and Input/Output Level Shifters
//	Xil_Out32(SLCR_UNLOCK, SLCR_UNLOCK_VAL);
//	Xil_Out32(LVL_SHFTR_EN, 0xF);
//	Xil_Out32(FPGA_RST_CTRL, 0x0);
//	Xil_Out32(SLCR_LOCK, SLCR_LOCK_VAL);

	return XST_SUCCESS;
}
