----------------------------------------------------------------------------------
-- Company: ENGS 128 17S
-- Engineer: Josh Feiber, Mike Kilgore
-- 
-- Create Date: 05/11/2017 02:41:40 PM
-- Design Name: 
-- Module Name: stack_tb - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: Testbench for the FIR filter.
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
library UNISIM;
use UNISIM.VComponents.all;

entity fir_tb is
--  Port ( );
end fir_tb;

architecture Behavioral of fir_tb is

component FIR is
    Generic (DATA_IN_WIDTH: integer;
             DATA_OUT_SIGNAL_WIDTH : integer;
             DATA_OUT_WIDTH : integer;
             ACCUM_REG_WIDTH : integer);
    Port ( next_sample_en : STD_LOGIC;
           x_in : in STD_LOGIC_VECTOR(DATA_IN_WIDTH-1 downto 0);
           x_out: out STD_LOGIC_VECTOR(DATA_OUT_SIGNAL_WIDTH-1 downto 0);
           clk_in : in STD_LOGIC);
end component;

constant DATA_IN_WIDTH : integer := 16;
constant DATA_OUT_SIGNAL_WIDTH : integer := 16;

signal x_in : std_logic_vector(DATA_IN_WIDTH-1 downto 0) := (others => '0');
signal x_out : std_logic_vector(DATA_OUT_SIGNAL_WIDTH-1 downto 0) := (others => '0');

-- Clock period
constant aclk_period : time := 8 ns;        -- 125 MHz clock
signal aclk : STD_LOGIC := '0';

constant en_tick_period : time := 16667 ns;
signal en_tick : STD_LOGIC := '0';

signal rand_num : integer := 0;

signal have_sent_data : boolean := false;

signal data_in : std_logic_vector(DATA_IN_WIDTH-1 downto 0) := (others => '0');

constant FSAMP : real := 960000.0;              -- Sampling rate (Hz)
constant TSAMP : real := 1.0/FSAMP;             -- Sampling period (sec)

constant samp_period : time := 1.042 us;		-- 960 kHz

-- Sine wave definitions
signal analog_sin : std_logic_vector(x_in'length-1 downto 0) := (others => '0');
constant SIN_FREQ : real := 1000.0; 				-- Hz
constant SIN_AMPL : real := real(2**(x_in'length-1)-1);		-- 2^(N-1) - 1

begin


uut: component FIR
    Generic Map (DATA_IN_WIDTH => DATA_IN_WIDTH,
                 DATA_OUT_SIGNAL_WIDTH => DATA_OUT_SIGNAL_WIDTH,
                 DATA_OUT_WIDTH => 16,
                 ACCUM_REG_WIDTH => 32)
    Port Map (
        next_sample_en => en_tick,
        x_in => x_in,
        clk_in => aclk,
        x_out => x_out
    );
    
-- Clock process
    clk_proc: process
    begin
        aclk <= not(aclk);
        wait for aclk_period/2;
    end process clk_proc;
    
    -- 960/R tick process
    en_tick_proc: process
    begin
        wait for en_tick_period; 
        en_tick <= '1';
        if not have_sent_data then
            data_in <= b"0000000000000001";
            have_sent_data <= true;
        else
            data_in <= (others => '0');
        end if;
        wait for aclk_period;
        en_tick <= '0';
    end process en_tick_proc;
    
       -- Stimulus process
    stim_proc: process
         variable t : real := 0.0;
    begin             
     loop                            -- Loop forever    
         analog_sin <= std_logic_vector(to_signed(integer( SIN_AMPL*sin(math_2_pi*SIN_FREQ*t) ), x_in'length));    
            wait for samp_period/2;
            wait for samp_period/2;                                    
         t := t + TSAMP;
     end loop;
    end process;
    
    -- Pick the TB input
    -- x_in <= analog_sin;    
      x_in <= data_in;    
    

end Behavioral;
