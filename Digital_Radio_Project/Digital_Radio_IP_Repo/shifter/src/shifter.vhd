----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 05/24/2017 09:16:35 PM
-- Design Name: 
-- Module Name: shifter - Behavioral
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

entity shifter is
    Generic (DATA_WIDTH : integer := 16;
             SHIFT      : integer := 5);
    Port ( data_in : in STD_LOGIC_VECTOR(DATA_WIDTH-1 downto 0);
           data_out : out STD_LOGIC_VECTOR(DATA_WIDTH-1 downto 0));
end shifter;

architecture Behavioral of shifter is

begin

--data_out <= std_logic_vector(SHIFT_LEFT(unsigned(data_in), SHIFT));
data_out <= data_in(6 downto 0) & "000000000";



end Behavioral;
