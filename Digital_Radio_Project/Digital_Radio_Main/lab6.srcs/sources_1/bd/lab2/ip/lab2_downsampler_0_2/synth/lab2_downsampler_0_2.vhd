-- (c) Copyright 1995-2017 Xilinx, Inc. All rights reserved.
-- 
-- This file contains confidential and proprietary information
-- of Xilinx, Inc. and is protected under U.S. and
-- international copyright and other intellectual property
-- laws.
-- 
-- DISCLAIMER
-- This disclaimer is not a license and does not grant any
-- rights to the materials distributed herewith. Except as
-- otherwise provided in a valid license issued to you by
-- Xilinx, and to the maximum extent permitted by applicable
-- law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND
-- WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES
-- AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
-- BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
-- INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
-- (2) Xilinx shall not be liable (whether in contract or tort,
-- including negligence, or under any other theory of
-- liability) for any loss or damage of any kind or nature
-- related to, arising under or in connection with these
-- materials, including for any direct, or any indirect,
-- special, incidental, or consequential loss or damage
-- (including loss of data, profits, goodwill, or any type of
-- loss or damage suffered as a result of any action brought
-- by a third party) even if such damage or loss was
-- reasonably foreseeable or Xilinx had been advised of the
-- possibility of the same.
-- 
-- CRITICAL APPLICATIONS
-- Xilinx products are not designed or intended to be fail-
-- safe, or for use in any application requiring fail-safe
-- performance, such as life-support or safety devices or
-- systems, Class III medical devices, nuclear facilities,
-- applications related to the deployment of airbags, or any
-- other applications that could lead to death, personal
-- injury, or severe property or environmental damage
-- (individually and collectively, "Critical
-- Applications"). Customer assumes the sole risk and
-- liability of any use of Xilinx products in Critical
-- Applications, subject only to applicable laws and
-- regulations governing limitations on product liability.
-- 
-- THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
-- PART OF THIS FILE AT ALL TIMES.
-- 
-- DO NOT MODIFY THIS FILE.

-- IP VLNV: dartmouth.edu:user:downsampler:1.0
-- IP Revision: 2

LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;

ENTITY lab2_downsampler_0_2 IS
  PORT (
    data_in : IN STD_LOGIC_VECTOR(15 DOWNTO 0);
    data_out_full : OUT STD_LOGIC_VECTOR(15 DOWNTO 0);
    data_out : OUT STD_LOGIC_VECTOR(15 DOWNTO 0);
    clk960 : IN STD_LOGIC;
    clk125mhz : IN STD_LOGIC;
    clk12khz : OUT STD_LOGIC;
    en12khz_out : OUT STD_LOGIC
  );
END lab2_downsampler_0_2;

ARCHITECTURE lab2_downsampler_0_2_arch OF lab2_downsampler_0_2 IS
  ATTRIBUTE DowngradeIPIdentifiedWarnings : STRING;
  ATTRIBUTE DowngradeIPIdentifiedWarnings OF lab2_downsampler_0_2_arch: ARCHITECTURE IS "yes";
  COMPONENT downsampler IS
    GENERIC (
      DATA_WIDTH : INTEGER;
      DATA_OUT_WIDTH : INTEGER
    );
    PORT (
      data_in : IN STD_LOGIC_VECTOR(15 DOWNTO 0);
      data_out_full : OUT STD_LOGIC_VECTOR(15 DOWNTO 0);
      data_out : OUT STD_LOGIC_VECTOR(15 DOWNTO 0);
      clk960 : IN STD_LOGIC;
      clk125mhz : IN STD_LOGIC;
      clk12khz : OUT STD_LOGIC;
      en12khz_out : OUT STD_LOGIC
    );
  END COMPONENT downsampler;
  ATTRIBUTE X_CORE_INFO : STRING;
  ATTRIBUTE X_CORE_INFO OF lab2_downsampler_0_2_arch: ARCHITECTURE IS "downsampler,Vivado 2016.1";
  ATTRIBUTE CHECK_LICENSE_TYPE : STRING;
  ATTRIBUTE CHECK_LICENSE_TYPE OF lab2_downsampler_0_2_arch : ARCHITECTURE IS "lab2_downsampler_0_2,downsampler,{}";
BEGIN
  U0 : downsampler
    GENERIC MAP (
      DATA_WIDTH => 16,
      DATA_OUT_WIDTH => 12
    )
    PORT MAP (
      data_in => data_in,
      data_out_full => data_out_full,
      data_out => data_out,
      clk960 => clk960,
      clk125mhz => clk125mhz,
      clk12khz => clk12khz,
      en12khz_out => en12khz_out
    );
END lab2_downsampler_0_2_arch;
