----------------------------------------------------------------------------------
-- Company: ENGS 128 17S
-- Engineer: Josh Feiber, Mike Kilgore
-- 
-- Create Date: 05/13/2017 01:47:13 PM
-- Design Name: 
-- Module Name: downsampler_tb - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: Testbench for the downsampler.
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

entity downsampler_tb is
--  Port ( );
end downsampler_tb;

architecture Behavioral of downsampler_tb is

component downsampler is
    Generic (DATA_WIDTH : integer;
             DATA_OUT_WIDTH : integer);
    Port ( data_in : in STD_LOGIC_VECTOR(DATA_WIDTH-1 downto 0);
           data_out_full : out STD_LOGIC_VECTOR(DATA_WIDTH-1 downto 0);
           data_out : out STD_LOGIC_VECTOR(DATA_WIDTH-1 downto 0);
           clk960 : in STD_LOGIC;
           clk125mhz : in STD_LOGIC;
           clk12khz : out STD_LOGIC;
           en12khz_out   : out STD_LOGIC);
end component;

constant slow_clk_period : time := 1042 ns;
constant fast_clk_period : time := 8 ns;

constant DATA_WIDTH : integer := 16;
signal data_in : std_logic_vector(DATA_WIDTH-1 downto 0);
signal data_out_full : std_logic_vector(DATA_WIDTH-1 downto 0);
signal data_out : std_logic_vector(DATA_WIDTH-1 downto 0);

signal clk960 : STD_LOGIC := '0';
signal clk125mhz : STD_LOGIC := '0';
signal clk12khz : STD_LOGIC;
signal en12khz : STD_LOGIC;

signal rand_num : integer := 0;

begin

uut : downsampler
    Generic Map (DATA_WIDTH => DATA_WIDTH,
                 DATA_OUT_WIDTH => 12)
    Port Map(
        data_in => data_in,
        data_out_full => data_out_full,
        data_out => data_out,
        clk960 => clk960,
        clk125mhz => clk125mhz,
        clk12khz => clk12khz,
        en12khz_out => en12khz
    );
    
   -- Clock process definitions
    slow_clk_process :process
    begin
         clk960 <= not(clk960);
         wait for slow_clk_period/2;
    end process;
    
    fast_clk_process : process
    begin
        clk125mhz <= not(clk125mhz);
        wait for fast_clk_period/2;
    end process;
    
    generate_rand_data: process
      variable seed1, seed2: positive;
      variable rand: real;
    begin
      uniform(seed1, seed2, rand);
      rand_num <= integer(rand*65500.0);
      wait for slow_clk_period;
    end process generate_rand_data;
    
    data_in <= std_logic_vector(to_signed(rand_num, data_in'length));



end Behavioral;
