----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 04/30/2017 01:29:55 PM
-- Design Name: 
-- Module Name: comb_filter - Behavioral
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

entity comb_filter is
    Generic (DATA_WIDTH : integer);
    Port ( clk : in STD_LOGIC;
           en : in STD_LOGIC;
           data_in : in STD_LOGIC_VECTOR (DATA_WIDTH-1 downto 0);
           data_out : out STD_LOGIC_VECTOR (DATA_WIDTH-1 downto 0));
end comb_filter;

architecture Behavioral of comb_filter is

signal comb_filter_reg : signed(DATA_WIDTH-1 downto 0) := (others => '0');
signal output_reg : signed(DATA_WIDTH-1 downto 0) := (others => '0');

begin

process(clk)
begin
    if rising_edge(clk) then
        if en = '1' then
            comb_filter_reg <= signed(data_in);
            output_reg <= signed(data_in) - comb_filter_reg;
        end if;
    end if;
end process;

data_out <= std_logic_vector(output_reg);


end Behavioral;
