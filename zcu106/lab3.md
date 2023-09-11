# Extending Memory Space with Block RAM

## Objectives

After completing this lab, you will be able to:

* Add BRAM and connect it to the processing system&#39;s AXI master port
* Execute the software application having data section in the BRAM

## Steps
## Open the Project        
### Open the Vivado program. Open the _lab1_ project you created earlier or use the _lab1_ project from the labsolution directory, and save the project as _lab3_.
1. Start Vivado if necessary and open either the _lab1_ project (lab1.xpr) you created earlier or the lab1 project in the _labsolutions_ directory using the **Open Project** link in the Getting Started page.
2. Select **File &gt; Project &gt; Save As …** to open the _Save Project As_ dialog box. Enter **lab3** as the project name.  Make sure that the _Create Project Subdirectory_ and _Import All Files to the New Project_ options are checked, the project directory path is **{labs}** and click **OK**.

    This will create the _lab3_ directory and save the project and associated directory with lab3 name.

## Configure the Processor to Enable M\_AXI\_HPM0\_FPD and M\_AXI\_HPM1\_FPD 
1. Open the Block Design and enable the M\_AXI\_HPM0\_FPD and M\_AXI\_HPM1\_FPD interfaces.
2. Click **Open Block Design** in the _Flow Navigator_ pane
3. Double-click on the _Zynq UltraScale+ MPSoC_ instance to open its configuration form.
4. Select _PS-PL Configuration_ in the Page Navigator window in the left pane, expand _PS-PL Interfaces&gt;Master Interface_, enable **AXI HPM0 FPD** and **AXI HPM1 FPD**, then disable **AXI HPM0 LPD**.
5. Select _Clock Configuration_ in the Page Navigator window in the left pane, click on _Output Clocks_ tab, expand _PL Fabric Clocks_ on the right, and click on the check-box of the **PL1** to enable it.
6. Enter the_Requested Frequency_for the **PL1** as **140.00000** MHz.

    <p align="center">
    <img src ="./images/lab3/1.png" width="60%" height="80%"/>
    </p>
    <p align = "center">
    <i>Output Clocks Configuration</i>
    </p>

7. Click **OK** to accept the settings and close the configuration form.

    <p align="center">
    <img src ="./images/lab3/Fig2.png" width="60%" height="80%"/>
    </p>
    <p align = "center">
    <i></i>
    </p>

## Extend with BRAM
### Add an AXI BRAM Controller instance with BRAM.
1. Click the ![alt tag](./images/add_ip.png) button and search for **BRAM** in the catalog.
2. Double-click the **AXI BRAM Controller** to add an instance to the design.
3. Click on **Run Connection Automation** , and select **axi\_bram\_ctrl\_0**
4. Click on **BRAM\_PORTA** and **BRAM\_PORTB** check boxes.
5. Click **S\_AXI** , and change the _Master_ option to **/zynq\_ultra\_ps\_e\_0/M\_AXI\_HPM1\_FPD**, change the Clock source for driving interconnect IP, Clock source for Master interface,_ and _Clock source for Salve interface_ to **/zynq\_ultra\_ps\_e\_0/pl\_clk1 (140 MHz)** as they all run in the same clock domain, and click **OK**

    <p align="center">
    <img src ="./images/lab3/Fig3.png" width="60%" height="80%"/>
    </p>
    <p align = "center">
    <i>Connecting AXI BRAM Controller to M_AXI_HPM1_FPD to run at faster clock speed</i>
    </p>

    Notice that an instance of AXI SmartConnect and Processor System Reset are added, and the maxihpm1\_fpd\_aclk is connected to pl\_clk1.

    <p align="center">
    <img src ="./images/lab3/Fig4.png" width="60%" height="80%"/>
    </p>
    <p align = "center">
    <i>Clocking network connections</i>
    </p>
6. Double-click on the **axi\_bram\_ctrl\_0** instance to open the configuration form.
7. Set the _Data Width_ to **64**.

    <p align="center">
    <img src ="./images/lab3/Fig5.png" width="60%" height="80%"/>
    </p>
    <p align = "center">
    <i>Setting the BRAM controller data width to 64</i>
    </p>
8. Click **OK**.
### Using the Address Editor tab, set the BRAM controller size to 64KB.  Validate the design.
1. Select the **Address Editor** tab and notice that the BRAM controller memory space is **8K**.
2. Click in the _Range_ column of the _axi\_bram\_ctrl\_0_ instance and set the size as **64K**.

    <p align="center">
    <img src ="./images/lab3/Fig6.png" width="60%" height="80%"/>
    </p>
    <p align = "center">
    <i>AXI BRAM space assignment</i>
    </p>

3. Select the _Diagram_ tab, and click on the ![alt tag](./images/validate.png) (Validate Design) button to make sure that there are no errors.
## Generate the Bitstream        
1. Click on the **Generate Bitstream** to run the synthesis, implementation, and bit generation processes.
2. Click **Save** if prompted to save the project, and **Yes** to run the processes. Click **OK** to launch the runs.
3. When the bitstream generation process has completed successfully, click **Cancel**.
## Generate Applications in the SDK        
### Export the implemented design, and start SDK
1. Export the hardware configuration by clicking **File &gt; Export &gt; Export Hardware…**
2. Click the box to _Include Bitstream_and click **OK** (Click _Yes_ if prompted to overwrite the previous module)
3. Launch SDK by clicking **File &gt; Launch SDK** and click **OK**
4. Right-click on the **lab1** and **standalone\_bsp\_0** and **system\_wrapper\_hw\_platfrom\_0** projects in the Project Explorer view and select **close project**.
### Create an empty application project, named lab3, and import the provided lab3.c file.
1. Select **File &gt; New** &gt; **Application Project.**
2. In the _Project Name_ field, enter **lab3** as the project name.
3. Use the default settings to create a new BSP and click **Next.**
4. Select the **Empty Application** template and click **Finish.**

    The lab3 and lab3\_bsp projects will be created in the Project Explorer window of SDK.

5. Select **lab3 &gt; src** directoryin the project view, right-click, and select **Import.**
6. Expand the **General** category and double-click on **File System.**
7. Browse to **{sources}\lab3** folder.
8. Select **lab3.c** and click **Finish.**

    A snippet of the source code is shown in the following figure. It shows that we write a pattern to the LED port and execute a software delay loop. Repeat for 16 times. It also shows the code (greyed) which will be used in Lab5.

    <p align="center">
    <img src ="./images/lab3/Fig7.png" width="60%" height="80%"/>
    </p>
    <p align = "center">
    <i>Source Code</i>
    </p>

## Test in Hardware   
### Connect and power up the board. Establish the serial communication using the SDK Terminal tab.  Program the FPGA.
1. Connect and power up the board.
2. In SDK, select **Xilinx &gt; Program FPGA** and click the **Program** button to program the FPGA.
3. Select the ![alt tag](./images/terminal.png) tab.  If it is not visible then select **Window &gt; Show view &gt; Terminal**.
4. Click on ![alt tag](./images/connect.png) to initiate the serial connection and select the appropriate COM port (depending on your computer). Configure it with 115200 baud rate.
### Run the lab3 application.
1. Select the **lab3** project in _Project Explorer_, right-click and select **Run As &gt; Launch on Hardware (System Debugger).** Click **Yes** to terminate the previous run.

    The application (lab3.elf) will be downloaded into the target device, execute psu\_init, and execute.

2. You should see the on-board LEDs changing patterns at roughly a one second delay rate.
### Modify the linker scipt to use the psu\_ddr\_0 for the code and data sections, and the BRAM for the Heap and Stack segments. Change the loop limit from 99999999 to 999999. Execute the program.
1. Select the **lab3** application in the _Project Explorer_ view.
1. Right-click and select **Generate Linker Script**.
1. Change the _code_ and _Data_ sections to **psu\_ddr\_0** and the _Heap and Stack_ segment memory to **axi\_bram\_ctrl\_0\_Mem0.**
1. Click the **Generate** button.
1. Click the **Yes** button to overwrite.
1. Change the loop limit from 99999999 to **9999999**. Save changes so the program recompiles.
1. Select the **lab3** project in _Project Explorer_, right-click and select **Run As &gt; Launch on Hardware (System Debugger).**

    Click OK to terminate the exisiting run and relaunch if shown.

1. You should see the on-board LEDs changing patterns very slowly (about 5 seconds).
1. Change the loop limit from 9999999 to **999999**. Save changes so the program recompiles.
1. Select the **lab3** project in _Project Explorer_, right-click and select **Run As &gt; Launch on Hardware (System Debugger).**

    Click Yes to terminate the existing run.

1. You should see the on-board LEDs changing patterns relatively faster (about 1 seconds).
1. Close the SDK program by selecting **File &gt; Exit**.
1. Close the Vivado program by selecting **File &gt; Exit.**
1. Turn OFF the power on the board.

## Conclusion

This lab led you through adding BRAM memory in the PL section thereby extending the total memory space available to the PS. You have verified the functionality by creating an application, targeting the stack and heap sections to the added BRAM, and executing the application.
