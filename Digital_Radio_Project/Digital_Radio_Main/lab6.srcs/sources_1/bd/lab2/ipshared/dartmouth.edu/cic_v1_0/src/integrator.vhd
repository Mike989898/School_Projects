----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 04/30/2017 01:19:57 PM
-- Design Name: 
-- Module Name: integrator - Behavioral
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

entity integrator is
    Generic (DATA_WIDTH : integer);
    Port ( data_in : in STD_LOGIC_VECTOR (DATA_WIDTH-1 downto 0);
           en : in STD_LOGIC;
           clk : in STD_LOGIC;
           data_out : out STD_LOGIC_VECTOR (DATA_WIDTH-1 downto 0));
end integrator;

architecture Behavioral of integrator is

signal integrator_reg : signed(DATA_WIDTH-1 downto 0) := (others => '0');

begin

process (clk)
begin
    if rising_edge(clk) AND en = '1' then
        integrator_reg <= signed(data_in) + integrator_reg;
    end if;
end process;

data_out <= std_logic_vector(integrator_reg);


end Behavioral;
