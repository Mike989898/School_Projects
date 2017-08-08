----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 05/23/2017 09:36:54 PM
-- Design Name: 
-- Module Name: demodulator - Behavioral
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

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity demodulator is
    Generic (DATA_IN_WIDTH : integer := 16;
             DATA_OUT_WIDTH : integer := 12;
             DATA_OUT_SIGNAL_WIDTH : integer := 16);
   Port (clk_in : in std_logic;
         I : in std_logic_vector(DATA_IN_WIDTH-1 downto 0);
         Q : in std_logic_vector(DATA_IN_WIDTH-1 downto 0);
         data_out : out std_logic_vector(DATA_OUT_SIGNAL_WIDTH-1 downto 0));
end demodulator;

architecture Behavioral of demodulator is

signal max_reg : std_logic_vector(DATA_IN_WIDTH-1 downto 0) := (others => '0');
signal min_reg : std_logic_vector(DATA_IN_WIDTH-1 downto 0) := (others => '0');
signal output_reg : std_logic_vector(DATA_OUT_SIGNAL_WIDTH-1 downto 0) := (others => '0');

signal y : std_logic_vector(DATA_OUT_SIGNAL_WIDTH-1 downto 0) := (others => '0');


begin

input_proc : process(clk_in)
begin
    if rising_edge(clk_in) then
        if abs(signed(I)) > abs(signed(Q)) then
            max_reg <= I;
            min_reg <= Q;
        else
            max_reg <= Q;
            min_reg <= I;
        end if;
    end if;
end process input_proc;

y <= std_logic_vector(abs(signed(max_reg)) + abs(signed(min_reg(min_reg'length-1 downto 2))));
data_out <= output_reg;

output_proc : process(clk_in)
begin
    if rising_edge(clk_in) then
        output_reg(DATA_OUT_WIDTH-1 downto 0) <= y(y'length-1 downto y'length-DATA_OUT_WIDTH);
    end if;
end process output_proc;


end Behavioral;
