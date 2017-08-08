----------------------------------------------------------------------------------
-- Company:         Engs 128 17S
-- Engineer:        Josh & Mike
-- 
-- Create Date:     04/04/2017 03:34:26 PM
-- Design Name:     
-- Module Name:     dds_clocking_tb - Behavioral
-- Project Name:    Lab 2
-- Target Devices: 
-- Tool Versions: 
-- Description:     Testbench for the DDS clocking module
-- 
-- Dependencies:    
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity dds_clocking_tb is
--  Port ( );
end dds_clocking_tb;

architecture Behavioral of dds_clocking_tb is

-- Declare unit under test
component DDSclocking is
port ( Clk125 	    : in std_logic;
       spi_sclk, DDS_aclk, Clk250, SPIclock_locked, clk125mhz	: out std_logic );
end component; 

-- Port map signals
signal spi_sclk, DDS_aclk, Clk250, SPIclock_locked, clk125mhz : STD_LOGIC := '0';
signal aclk : STD_LOGIC := '0';

-- Clock period
constant aclk_period : time := 8 ns;        -- 125 MHz clock

begin

-- Instantiate unit under test
uut: DDSclocking port map (
    Clk125 => aclk,
    spi_sclk => spi_sclk,
    DDS_aclk => DDS_aclk,
	Clk250 => Clk250,
	SPIclock_locked => SPIclock_locked,
	clk125mhz => clk125mhz
  );

-- Clock process
clk_proc: process
begin
    aclk <= not(aclk);
    wait for aclk_period/2;
end process clk_proc;

-- Stimulus process
-- There is no stimulus process
end Behavioral;
