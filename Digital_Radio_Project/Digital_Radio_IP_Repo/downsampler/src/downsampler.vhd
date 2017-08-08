----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 05/12/2017 09:37:22 PM
-- Design Name: 
-- Module Name: downsampler - Behavioral
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

entity downsampler is
    Generic (DATA_WIDTH : integer := 16;
             DATA_OUT_WIDTH : integer := 12);
    Port ( data_in : in STD_LOGIC_VECTOR(DATA_WIDTH-1 downto 0);
           data_out_full : out STD_LOGIC_VECTOR(DATA_WIDTH-1 downto 0);
           data_out : out STD_LOGIC_VECTOR(DATA_WIDTH-1 downto 0);
           clk960 : in STD_LOGIC;
           clk125mhz : in STD_LOGIC;
           clk12khz : out STD_LOGIC;
           en12khz_out   : out STD_LOGIC);
end downsampler;

architecture Behavioral of downsampler is

--Signals for clock divider
constant CLOCK_DIVIDER_VALUE : integer := 40; -- this will generate a clock of 960R/5 with a 50% duty cycle
signal clkdiv: integer := 0;		-- the clock divider counter
signal slow_clock : std_logic := '0'; -- 960Khz/R/5 clock signal that comes out of the BUFG

signal EN_TICK_TC : integer := 2*CLOCK_DIVIDER_VALUE;
signal en_tick_count : integer := 0;
signal en12khz : std_logic := '0';

signal data_out_reg : std_logic_vector(DATA_WIDTH-1 downto 0) := (others => '0');
signal data_out_reg_full : std_logic_vector(DATA_WIDTH-1 downto 0) := (others => '0');

signal clk12khz_internal : std_logic := '0';

--state machine
type controller_state is (ST0, ST1, ST2);
signal CURR_STATE, NEXT_STATE : controller_state;

begin

en12khz_out <= en12khz;
clk12khz <= clk12khz_internal;

-- The BUFG component puts the 960Khz/R clock onto the FPGA clocking network
twelve_clk_buf: BUFG
      port map (I => slow_clock,
                O => clk12khz_internal );
                
                
-- Divide the master clock down to 960/R/10, then
 -- toggling the slow_clock signal at that frequency gives a 960KHz/R/5 clock with 50% duty cycle
Clock_divider: process(clk960)
begin
    if rising_edge(clk960) then
        if clkdiv = CLOCK_DIVIDER_VALUE-1 then 
            slow_clock <= NOT(slow_clock);        
            clkdiv <= 0;
        else
            clkdiv <= clkdiv + 1;
        end if;
    end if;
end process Clock_divider;

-- Counter for the 960R/5 en tick
en_tick : process(clk960)
begin
    if rising_edge(clk960) then
        if en_tick_count = EN_TICK_TC - 1 then
            en_tick_count <= 0;
        else
            en_tick_count <= en_tick_count + 1;
        end if;
    end if;
end process en_tick;

--flip flop for monopluser state machine
synch_mono_pulse: process (clk125mhz)
begin
    if rising_edge(clk125mhz) then
        CURR_STATE <= NEXT_STATE;
    end if;
end process synch_mono_pulse;


--update the states
state_machine_proc: process(CURR_STATE, en_tick_count)
begin
    en12khz <= '0';
    NEXT_STATE <= CURR_STATE;
    case CURR_STATE is
        when ST0 =>
            if en_tick_count = EN_TICK_TC-1 then
                NEXT_STATE <= ST1;
            end if;
        when ST1 =>
            en12khz <= '1';
            NEXT_STATE <= ST2;
        when ST2 =>
            if en_tick_count = 0 then
                NEXT_STATE <= ST0;
            end if;
        end case;
end process state_machine_proc;

-- Register the output
output_clock : process(clk12khz_internal)
begin
    if rising_edge(clk12khz_internal) then
        data_out_reg_full <= data_in;
        data_out_reg(DATA_OUT_WIDTH-1 downto 0) <= data_in(data_in'length-1 downto data_in'length-DATA_OUT_WIDTH);
    end if;
end process output_clock;
data_out_full <= data_out_reg_full;
data_out <= data_out_reg;


end Behavioral;
