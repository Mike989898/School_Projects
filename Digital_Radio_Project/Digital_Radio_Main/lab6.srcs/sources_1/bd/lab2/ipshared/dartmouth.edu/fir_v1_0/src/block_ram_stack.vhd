----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 05/10/2017 09:57:57 PM
-- Design Name: 
-- Module Name: block_ram - Behavioral
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
library UNISIM;
use UNISIM.VComponents.all;

entity block_ram_stack is
    Generic( M : in integer := 64);
    Port ( addra : in STD_LOGIC_VECTOR (6 downto 0);
           clka : in STD_LOGIC;
           dina : in STD_LOGIC_VECTOR (15 downto 0);
           douta : out STD_LOGIC_VECTOR (15 downto 0);
           ena : in STD_LOGIC;
           wea : in STD_LOGIC);
end block_ram_stack;

architecture Behavioral of block_ram_stack is
type delay_line_type is array (M-1 downto 0) of STD_LOGIC_VECTOR (15 downto 0);
signal delay_line : delay_line_type := (others => (others => '0'));

begin

douta <= delay_line(to_integer(unsigned(addra)));

main_delay_line_proc: process (clka)
begin
    if rising_edge(clka) then
        if ena = '1' then
            if wea = '1' then
                for i in 0 to M-2 loop
                    delay_line(i+1) <= delay_line(i);
                end loop;
                delay_line(0) <= dina;
            end if;
        end if;
    end if;
end process main_delay_line_proc;

end Behavioral;
