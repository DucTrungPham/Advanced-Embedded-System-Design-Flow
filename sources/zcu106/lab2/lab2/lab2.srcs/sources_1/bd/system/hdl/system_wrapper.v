//Copyright 1986-2018 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2018.2 (win64) Build 2258646 Thu Jun 14 20:03:12 MDT 2018
//Date        : Sun Aug 20 08:00:23 2023
//Host        : LAPTOP-TRUNG running 64-bit major release  (build 9200)
//Command     : generate_target system_wrapper.bd
//Design      : system_wrapper
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module system_wrapper
   (dip_switches_8bits_tri_i,
    leds,
    push_buttons_5bits_tri_i);
  input [7:0]dip_switches_8bits_tri_i;
  output [7:0]leds;
  input [4:0]push_buttons_5bits_tri_i;

  wire [7:0]dip_switches_8bits_tri_i;
  wire [7:0]leds;
  wire [4:0]push_buttons_5bits_tri_i;

  system system_i
       (.dip_switches_8bits_tri_i(dip_switches_8bits_tri_i),
        .leds(leds),
        .push_buttons_5bits_tri_i(push_buttons_5bits_tri_i));
endmodule
