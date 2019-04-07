# G1_Xilinx_Video_Mixer_Example

This is an example project created to demonstrate the use of the Xilinx Video Mixer IP.

This project uses the Nexys Video FPGA and outputs the alpha blending of two sample test
patterns through HDMI.

## 1.0 Project Structure

```
G1_Xilinx_Video_Mixer_Example
  ┣ proj                                    # Vivado project
  ┃ ┣ Nexys-Video-HDMI-master.cache         # Vivado cached files
  ┃ ┣ Nexys-Video-HDMI-master.hw            # Vivado hardware files
  ┃ ┣ Nexys-Video-HDMI-master.ip_user_files # Vivado IPs
  ┃ ┣ Nexys-Video-HDMI-master.runs          # Vivado runs
  ┃ ┣ Nexys-Video-HDMI-master.sim           # Vivado simulation
  ┃ ┣ Nexys-Video-HDMI-master.srcs          # Vivado Verilog source files
  ┃ ┣ Nexys-Video-HDMI-master.sdk           # Vivado SDK
  ┃ ┃ ┣ RemoteSystemsTempFiles			
  ┃ ┃ ┣ mixer                               # Main SDK source
  ┃ ┃ ┃ ┣ src
  ┃ ┃ ┃ ┃ ┣ DisplayControl                  # Display control API
  ┃ ┃ ┃ ┃ ┣ DynamicClockControl             # Dynamic clock control API
  ┃ ┃ ┃ ┃ ┣ InterruptControl                # Interrupt control API
  ┃ ┃ ┃ ┃ ┣ TimerControl                    # Timer control API
  ┃ ┃ ┃ ┃ ┣ lscript.ld                      # Linker file
  ┃ ┃ ┃ ┃ ┣ main.c                          # Main – entry point
  ┃ ┃ ┃ ┗ ┗ main.h                          # Main includes
  ┃ ┃ ┣ mixer_bsp                          # System board support file
  ┃ ┃ ┃ ┣ Makefile                          # SDK Makefile
  ┃ ┃ ┃ ┗ system.mss                        # System microprocessor software spec
  ┃ ┃ ┣ top_hw_platform_0					
  ┃ ┃ ┗ top.hdf                             # System hardware description file
  ┃ ┣ Nexys-Video-HDMI-master.tmp           # Vivado temporary files
  ┃ ┗ Nexys-Video-HDMI-master.xpr           # Vivado project file
  ┣ src                                     # Nexys Video Constraint file
  ┗ repo                                    # Digilent IP source files

  ```

## 2.0 Acknowledgement 
```
  This project contains various source files from Xilinx and Digilent.  Please acknowledge 
  the copyright agreement for academic workplaces only.
```
