----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 05/24/2017 09:23:12 PM
-- Design Name: 
-- Module Name: shifter_tb - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
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
use IEEE.NUMERIC_STD.ALL;
use IEEE.MATH_REAL.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
library UNISIM;
use UNISIM.VComponents.all;

entity shifter_tb is
--  Port ( );
end shifter_tb;

architecture Behavioral of shifter_tb is

component shifter is
    Generic (DATA_WIDTH : integer := 16;
             SHIFT      : integer := 5);
    Port ( data_in : in STD_LOGIC_VECTOR(DATA_WIDTH-1 downto 0);
           data_out : out STD_LOGIC_VECTOR(DATA_WIDTH-1 downto 0));
end component;


constant FSAMP : real := 960000.0;              -- Sampling rate (Hz)
constant TSAMP : real := 1.0/FSAMP;             -- Sampling period (sec)

constant samp_period : time := 1.042 us;		-- 960 kHz

-- Sine wave definitions
signal analog_sin : std_logic_vector(15 downto 0) := (others => '0');
constant SIN_FREQ : real := 6000.0; 				-- Hz
constant SIN_AMPL : real := 62.0;		-- 2^(N-1) - 1

signal data_out : std_logic_vector(15 downto 0);

begin

uut: component shifter
    Port Map (
        data_in => analog_sin,
        data_out => data_out);
        
       -- Stimulus process
     stim_proc: process
          variable t : real := 0.0;
     begin             
      loop                            -- Loop forever    
          analog_sin <= std_logic_vector(to_signed(integer( SIN_AMPL*sin(math_2_pi*SIN_FREQ*t) ), analog_sin'length));    
             wait for samp_period/2;
             wait for samp_period/2;                                    
          t := t + TSAMP;
      end loop;
     end process;


end Behavioral;
