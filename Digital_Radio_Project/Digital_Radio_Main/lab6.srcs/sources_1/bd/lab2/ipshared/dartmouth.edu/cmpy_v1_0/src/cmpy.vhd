----------------------------------------------------------------------------------
-- Company: ENGS 128 17S
-- Engineer: Josh Feiber, Mike Kilgore
-- 
-- Create Date: 04/29/2017 02:03:20 PM
-- Design Name: 
-- Module Name: cmpy - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: Complex multiplier. Can vary the input/output widths via generics.
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
--library UNISIM;
--use UNISIM.VComponents.all;

entity cmpy is
    Generic (A_WIDTH : integer := 12; 
             B_WIDTH : integer := 12; 
             Y_WIDTH : integer := 25;
             A_B_MULT_WIDTH : integer := 24;
             A_TDATA_WIDTH : integer := 32;
             B_TDATA_WIDTH : integer := 32;
             Y_TDATA_WIDTH : integer := 64);
    Port ( data_clk : in STD_LOGIC;
           aresetn : in STD_LOGIC;
           s_axis_a_tvalid : in STD_LOGIC;
           s_axis_a_tdata : in STD_LOGIC_VECTOR (A_TDATA_WIDTH-1 downto 0);
           s_axis_b_tvalid : in STD_LOGIC;
           s_axis_b_tdata : in STD_LOGIC_VECTOR (B_TDATA_WIDTH-1 downto 0);
           m_axis_y_tvalid : out STD_LOGIC;
           m_axis_y_tdata : out STD_LOGIC_VECTOR (Y_TDATA_WIDTH-1 downto 0));
end cmpy;

architecture Behavioral of cmpy is

constant IP_A_WIDTH : integer := 8 * integer( ceil(real(A_WIDTH)/8.0) ); 
constant IP_B_WIDTH : integer := 8 * integer( ceil(real(B_WIDTH)/8.0) );
constant IP_Y_WIDTH : integer := 8 * integer( ceil(real(Y_WIDTH)/8.0) );

signal y_reg : std_logic_vector(Y_TDATA_WIDTH-1 downto 0) := (others => '0');
signal I : signed(A_B_MULT_WIDTH downto 0) := (others => '0');
signal Q : signed(A_B_MULT_WIDTH downto 0) := (others => '0');

signal a_real : signed(A_WIDTH-1 downto 0) := (others => '0');
signal a_imag : signed(A_WIDTH-1 downto 0) := (others => '0');
signal b_real : signed(B_WIDTH-1 downto 0) := (others => '0');
signal b_imag : signed(B_WIDTH-1 downto 0) := (others => '0');

begin

process(data_clk)
begin
    if rising_edge(data_clk) then
        if aresetn = '0' then
            y_reg <= (others => '0');
            a_real <= (others => '0');
            b_real <= (others => '0');
            a_imag <= (others => '0');
            b_imag <= (others => '0');
        end if;
        if s_axis_a_tvalid = '1' then
            a_real <= signed(s_axis_a_tdata(A_WIDTH-1 downto 0));
            a_imag <= signed(s_axis_a_tdata(IP_A_WIDTH+A_WIDTH-1 downto IP_A_WIDTH));
        else
            a_real <= (others => '0');
            a_imag <= (others => '0');
        end if;
        if s_axis_b_tvalid = '1' then
            b_real <= signed(s_axis_b_tdata(B_WIDTH-1 downto 0));
            b_imag <= signed(s_axis_b_tdata(IP_B_WIDTH+B_WIDTH-1 downto IP_B_WIDTH));
        else
            b_real <= (others => '0');
            b_imag <= (others => '0');
        end if;
       
        y_reg(Y_WIDTH-1 downto 0) <= std_logic_vector(I(I'length-1 downto I'length-Y_WIDTH));
        y_reg(IP_Y_WIDTH+Y_WIDTH-1 downto IP_Y_WIDTH) <= std_logic_vector(Q(Q'length-1 downto Q'length-Y_WIDTH));
    end if;
    
end process;

I <= resize((b_real * a_real) - (a_imag * b_imag), I'length);
Q <= resize((b_real*a_imag) + (b_imag * a_real), Q'length);

m_axis_y_tvalid <= '1';
m_axis_y_tdata <= y_reg;

end Behavioral;
