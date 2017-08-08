----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 05/11/2017 02:41:40 PM
-- Design Name: 
-- Module Name: stack_tb - Behavioral
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
use IEEE.MATH_REAL.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity stack_tb is
--  Port ( );
end stack_tb;

architecture Behavioral of stack_tb is

component block_ram_stack is
    Generic( M : in integer);
    Port ( addra : in STD_LOGIC_VECTOR (6 downto 0);
           clka : in STD_LOGIC;
           dina : in STD_LOGIC_VECTOR (15 downto 0);
           douta : out STD_LOGIC_VECTOR (15 downto 0);
           ena : in STD_LOGIC;
           wea : in STD_LOGIC);
end component;


signal addra: std_logic_vector(6 downto 0) := (others => '0');
signal dina: std_logic_vector(15 downto 0);
signal douta : std_logic_vector(15 downto 0);
signal ena : std_logic;
signal wea: std_logic;

-- Clock period
constant aclk_period : time := 8 ns;        -- 125 MHz clock
signal aclk : STD_LOGIC := '0';

signal rand_num : integer := 0;

begin

stack : component block_ram_stack
    Generic Map (M => 64)
    Port Map (
       addra => addra,
       clka => aclk,
       dina => dina,
       douta => douta,
       ena => ena,
       wea => wea  
    );
    
-- Clock process
    clk_proc: process
        variable seed1, seed2: positive;
        variable rand: real;
    begin
        aclk <= not(aclk);
        wait for aclk_period/2;
    end process clk_proc;
    
  -- insert data into RAM
  ram_proc: process
    variable seed1, seed2: positive;
    variable rand: real;
  begin
    uniform(seed1, seed2, rand);
    rand_num <= integer(rand*1000.0);
    ena <= '1';
    wea <= '1';
    addra <= b"0000001";
    dina <= std_logic_vector(to_unsigned(rand_num, dina'length));
    wait for aclk_period;
  end process ram_proc;
    




end Behavioral;
