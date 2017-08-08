----------------------------------------------------------------------------------
-- Company: 			Engs 128 17S
-- Engineer: 			Mike & Josh
-- 
-- Create Date:    	 
-- Design Name: 		
-- Module Name:    		Lab2 
-- Project Name: 		DDSClocking
-- Target Devices: 		Zybo
-- Tool versions: 		Vivado 2016.1
-- Description: 		Takes a 125Mhz clock input and gives a 250Mhz clock, a 19.2Mhz clock, and a 960Khz clock output.
--				
--
----------------------------------------------------------------------------------
-- Boilerplate: don't change this
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.ALL;		-- needed for arithmetic

library UNISIM;					-- needed for the BUFG component
use UNISIM.Vcomponents.ALL;


-- Entity block

entity DDSclocking is
port ( Clk125 	    : in std_logic;
       spi_sclk, DDS_aclk, Clk250, clk125mhz, SPIclock_locked	: out std_logic );
end DDSclocking; 

architecture Behavioral of DDSclocking is

component clk_wiz_0
port
 (-- Clock in ports
  clk_in1           : in     std_logic;
  -- Clock out ports
  spiclk          : out    std_logic;
  fast_clock          : out    std_logic;
  clk125mhz          : out    std_logic;
  -- Status and control signals
  locked            : out    std_logic
 );
end component;
constant CLOCK_DIVIDER_VALUE: integer := 10;      -- (19.2Mhz/960Khz)/2
signal clkdiv: integer := 0;		-- the clock divider counter
signal clk_en: std_logic := '0';    -- 960Khz clock signal that goes into BUFG
signal clk: std_logic;			    -- the 960Khz clock

signal nineteen_two_mhz : std_logic := '0';
signal test : std_logic := '0';

begin

clock_wiz : clk_wiz_0
   port map ( 
   -- Clock in ports
    clk_in1 => Clk125,
  -- Clock out ports  
   spiclk => nineteen_two_mhz,
   fast_clock => Clk250,
   locked => SPIclock_locked ,
   clk125mhz => clk125mhz           
 );

spi_sclk <= nineteen_two_mhz;
DDS_aclk <= clk;

-- The BUFG component puts the 960Khz clock onto the FPGA clocking network
Slow_clock_buffer: BUFG
      port map (I => clk_en,
                O => clk );

-- Divide the master clock down to 19.2MHz, then
-- toggling the clk_en signal at 19.2MHz gives a 960 KHz clock with 50% duty cycle
Clock_divider: process(nineteen_two_mhz)
begin
	if rising_edge(nineteen_two_mhz) then
	   	if clkdiv = CLOCK_DIVIDER_VALUE-1 then 
	   		clk_en <= NOT(clk_en);		
			clkdiv <= 0;
		else
			clkdiv <= clkdiv + 1;
		end if;
	end if;
end process Clock_divider;



end Behavioral; 