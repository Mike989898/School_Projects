----------------------------------------------------------------------------------
-- Company: ENGS 128 17S
-- Engineer: Josh Feiber, Mike Kilgore
-- 
-- Create Date: 04/30/2017 01:52:47 PM
-- Design Name: 
-- Module Name: CIC - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: CIC filter with variable input/outputs.
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

entity CIC is
    Generic (R : integer := 16;
             DATA_IN_WIDTH : integer := 16;
             DATA_OUT_WIDTH : integer := 16;
             DATA_OUT_SIGNAL_WIDTH : integer := 16;
             MAX_REG_LENGTH : integer := 28);
    Port ( data_in : in STD_LOGIC_VECTOR (DATA_IN_WIDTH-1 downto 0);
           data_in_tvalid : in STD_LOGIC;
           data_out : out STD_LOGIC_VECTOR (DATA_OUT_SIGNAL_WIDTH-1 downto 0);
           data_out_tvalid : out STD_LOGIC;
           clk125mhz : in STD_LOGIC;
           clk960khz : in STD_LOGIC;
           en960R_out    : out STD_LOGIC;
           clk_start_conversion : out STD_LOGIC);
end CIC;

architecture Behavioral of CIC is

component comb_filter is
    Generic (DATA_WIDTH : integer);
    Port ( clk : in STD_LOGIC;
           en : in STD_LOGIC;
           data_in : in STD_LOGIC_VECTOR (DATA_WIDTH-1 downto 0);
           data_out : out STD_LOGIC_VECTOR (DATA_WIDTH-1 downto 0));
end component;

component integrator is
    Generic (DATA_WIDTH : integer);
    Port ( data_in : in STD_LOGIC_VECTOR (DATA_WIDTH-1 downto 0);
           en : in STD_LOGIC;
           clk : in STD_LOGIC;
           data_out : out STD_LOGIC_VECTOR (DATA_WIDTH-1 downto 0));
end component;

-- Input reg
signal data_in_reg : std_logic_vector(MAX_REG_LENGTH-1 downto 0) := (others => '0');

-- Output reg
signal data_out_reg : std_logic_vector(DATA_OUT_SIGNAL_WIDTH-1 downto 0) := (others => '0');

-- Integrator intermediate signals
signal integrator_1_out : std_logic_vector(MAX_REG_LENGTH-1 downto 0);
signal integrator_2_out : std_logic_vector(MAX_REG_LENGTH-1 downto 0);
signal integrator_3_out : std_logic_vector(MAX_REG_LENGTH-1 downto 0);

-- Comb filter intermediate signals
signal comb_filter_1_out : std_logic_vector(MAX_REG_LENGTH-1 downto 0);
signal comb_filter_2_out : std_logic_vector(MAX_REG_LENGTH-1 downto 0);
signal comb_filter_3_out : std_logic_vector(MAX_REG_LENGTH-1 downto 0);

-- Down sampler register
signal down_sampler_reg : std_logic_vector(MAX_REG_LENGTH-1 downto 0) := (others => '0');

--Signals for clock divider
constant CLOCK_DIVIDER_VALUE : integer := R/2;
signal clkdiv: integer := 0;		-- the clock divider counter
signal clk_start_conversion_bufg_in : std_logic := '0';    -- 960Khz/R clock signal that goes into BUFG
signal clk960R : std_logic := '0'; -- 960Khz/R clock signal that comes out of the BUFG

-- Signals for the en960R tick counter 
constant EN_TICK_TERMINAL_VALUE : integer := R;
signal en_tick_counter : integer := 0; -- enable tick counter
signal en960R : std_logic := '0'; -- 960Khz/R enable tick for the down sampler and comb filter registers

--state machine
type controller_state is (ST0, ST1, ST2);
signal CURR_STATE, NEXT_STATE : controller_state;

begin

-- The BUFG component puts the 960Khz/R clock onto the FPGA clocking network
nine_sixty_clk_buf: BUFG
      port map (I => clk_start_conversion_bufg_in,
                O => clk960R );


integrator_1 : integrator
    generic map(
        DATA_WIDTH => MAX_REG_LENGTH
    )
    port map (
        data_in => data_in_reg,
        data_out => integrator_1_out,
        clk => clk960khz,
        en => '1'
    );

integrator_2 : integrator
    generic map(
        DATA_WIDTH => MAX_REG_LENGTH
    )
    port map (
        data_in => integrator_1_out,
        data_out => integrator_2_out,
        clk => clk960khz,
        en => '1'
    );
    
integrator_3 : integrator
        generic map(
            DATA_WIDTH => MAX_REG_LENGTH
        )
        port map (
            data_in => integrator_2_out,
            data_out => integrator_3_out,
            clk => clk960khz,
            en => '1'
        );
        
comb_filter_1 : comb_filter
                generic map(
                    DATA_WIDTH => MAX_REG_LENGTH
                )
                port map (
                    data_in => down_sampler_reg,
                    data_out => comb_filter_1_out,
                    clk => clk125mhz,
                    en => en960R
                );

comb_filter_2 : comb_filter
                generic map(
                    DATA_WIDTH => MAX_REG_LENGTH
                )
                port map (
                    data_in => comb_filter_1_out,
                    data_out => comb_filter_2_out,
                    clk => clk125mhz,
                    en => en960R
                );

comb_filter_3 : comb_filter
                generic map(
                    DATA_WIDTH => MAX_REG_LENGTH
                )
                port map (
                    data_in => comb_filter_2_out,
                    data_out => comb_filter_3_out,
                    clk => clk125mhz,
                    en => en960R
                );
          
clk_start_conversion <= clk960R;
en960R_out <= en960R;

output_reg : process(clk960khz) 
begin
    if rising_edge(clk960khz) then
        data_out_reg(DATA_OUT_WIDTH-1 downto 0) <= comb_filter_3_out(MAX_REG_LENGTH-1 downto MAX_REG_LENGTH-DATA_OUT_WIDTH);
    end if;
end process;
data_out <= data_out_reg;
data_out_tvalid <= '1';

-- Divide the master clock down to 960Khz/R/2, then
-- toggling the clk_start_conversion_bufg_in signal at that frequency gives a 960KHz/R clock with 50% duty cycle
Clock_divider: process(clk960khz)
begin
    if rising_edge(clk960khz) then
        if clkdiv = CLOCK_DIVIDER_VALUE-1 then 
            clk_start_conversion_bufg_in <= NOT(clk_start_conversion_bufg_in);        
            clkdiv <= 0;
        else
            clkdiv <= clkdiv + 1;
        end if;
    end if;
end process Clock_divider;

en_tick_counter_proc : process(clk960khz)
begin
    if rising_edge(clk960khz) then
        if en_tick_counter = EN_TICK_TERMINAL_VALUE-1 then
            en_tick_counter <= 0;
        else
            en_tick_counter <= en_tick_counter +1;
        end if;
    end if;
end process en_tick_counter_proc;


--flip flop for monopluser state machine
synch_mono_pulse: process (clk125mhz)
begin
    if rising_edge(clk125mhz) then
        CURR_STATE <= NEXT_STATE;
    end if;
end process synch_mono_pulse;

--update the states
state_machine_proc: process(CURR_STATE, en_tick_counter)
begin
    NEXT_STATE <= CURR_STATE;
    en960R <= '0';
    case CURR_STATE is
        when ST0 =>
            if en_tick_counter = EN_TICK_TERMINAL_VALUE-1 then
                NEXT_STATE <= ST1;
            end if;
        when ST1 =>
            en960R <= '1';
            NEXT_STATE <= ST2;
        when ST2 =>
            if en_tick_counter = 0 then
                NEXT_STATE <= ST0;
            end if;
        when others =>
        end case;
end process state_machine_proc;
              

-- Process for the downsampler reg
down_sampler: process(clk125mhz)
begin
    if rising_edge(clk125mhz) AND en960R = '1' then
        down_sampler_reg <= integrator_3_out;
    end if;
end process down_sampler;

-- Process for the input reg
latch_input : process(clk960khz)
begin
    if rising_edge(clk960khz) AND data_in_tvalid = '1' then
        data_in_reg <= std_logic_vector(resize(signed(data_in), MAX_REG_LENGTH));
    end if;
end process latch_input;
    


end Behavioral;
