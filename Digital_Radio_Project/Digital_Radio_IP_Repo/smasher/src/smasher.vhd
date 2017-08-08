----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 05/03/2017 01:49:08 PM
-- Design Name: 
-- Module Name: smasher - Behavioral
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
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity smasher is
    Generic ( INPUT_WIDTH : integer);
    Port ( low : in STD_LOGIC_VECTOR(INPUT_WIDTH-1 downto 0);
           high : in STD_LOGIC_VECTOR(INPUT_WIDTH-1 downto 0);
           y : out STD_LOGIC_VECTOR(2*INPUT_WIDTH-1 downto 0));
end smasher;

architecture Behavioral of smasher is

begin

y <= high & low;


end Behavioral;
