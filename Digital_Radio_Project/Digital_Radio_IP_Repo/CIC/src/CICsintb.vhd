--------------------------------------------------------------------------------
-- Company: 		Engs 128 17S
-- Engineer:		E.W. Hansen
--
-- Create Date:   	04/27/2017
-- Design Name:   
-- Module Name:   	CICsintb.vhd
-- Project Name:	
-- Target Device:  	
-- Tool versions:  	Vivado 2016.1
-- Description:   
-- 
-- Dependencies:
-- 
-- Revision:
--  0.01 - File Created originally by ISE
--	1.0 -- Modified for 17S
-- Additional Comments:
--
-------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;
USE ieee.math_real.ALL;
 
ENTITY CICsintb IS
END CICsintb;
 
ARCHITECTURE behavior OF CICsintb IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
    -- Modify this for your particular component's entity
--    COMPONENT cic3s32
--    PORT(
--         clk : IN  std_logic;		-- master clock
--         cic_nd : IN  std_logic;	-- clock integrator registers
--         x_in : IN  std_logic_vector(15 downto 0);
--         cic_rdy : OUT  std_logic;	-- enable comb registers
--         y_out : OUT  std_logic_vector(15 downto 0)
--        );
--    END COMPONENT;    

component CIC is
    Generic (R : integer;
             DATA_IN_WIDTH : integer;
             DATA_OUT_WIDTH : integer;
             MAX_REG_LENGTH : integer;
             DATA_OUT_SIGNAL_WIDTH : integer);
    Port ( data_in : in STD_LOGIC_VECTOR (DATA_IN_WIDTH-1 downto 0);
           data_in_tvalid : in STD_LOGIC;
           data_out : out STD_LOGIC_VECTOR (DATA_OUT_SIGNAL_WIDTH-1 downto 0);
           clk125mhz : in STD_LOGIC;
           clk960khz : in STD_LOGIC;
           en960R_out    : out STD_LOGIC;
           clk_start_conversion : out STD_LOGIC);
end component;

   --Inputs
   signal clk : std_logic := '0';
   signal cic_nd : std_logic := '0';
   signal reset : std_logic := '0';
   signal x_in : std_logic_vector(15 downto 0) := (others => '0');

 	--Outputs
--   signal cic_rdy : std_logic;
   signal y_out : std_logic_vector(15 downto 0);
   signal en960R_out : std_logic;
   signal clk_start_conversion : std_logic;
   signal integrator_2_out_out : std_logic_vector(27 downto 0);
   signal comb_filter_3_out_out : std_logic_vector(27 downto 0);
   signal fsm_out_next : std_logic_vector(3 downto 0);
   signal fsm_out_curr : std_logic_vector(3 downto 0);
   

   -- Clock period definitions
    constant clk_period : time := 8 ns;				-- 125 MHz
	constant samp_period : time := 1.042 us;		-- 960 kHz

    constant FSAMP : real := 960000.0;              -- Sampling rate (Hz)
    constant TSAMP : real := 1.0/FSAMP;             -- Sampling period (sec)
--    constant samp_period : time := integer(TSAMP * 10.0**9) * 1 ns;	

	 -- Sine wave definitions
 	 signal analog_sin : std_logic_vector(x_in'length-1 downto 0) := (others => '0');
	 constant SIN_FREQ : real := 2000.0; 				-- Hz
	 constant SIN_AMPL : real := real(2**(x_in'length-1)-1);		-- 2^(N-1) - 1
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: CIC 
   Generic Map (
    R => 16,
    DATA_IN_WIDTH => 16,
    DATA_OUT_WIDTH => 12,
    DATA_OUT_SIGNAL_WIDTH => 16,
    MAX_REG_LENGTH => 28
   )
   PORT MAP (
          clk125mhz => clk,
          clk960khz => cic_nd,
          data_in => x_in,
          data_in_tvalid => '1',
          en960R_out => en960R_out,
          clk_start_conversion => clk_start_conversion,
          data_out => y_out
        );
	
   -- Clock process definitions
   clk_process :process
   begin
		clk <= '0';
		wait for clk_period/2;
		clk <= '1';
		wait for clk_period/2;
   end process;
	
   -- Stimulus process
   stim_proc: process
		variable t : real := 0.0;
   begin		
		
    loop							-- Loop forever	
		analog_sin <= std_logic_vector(to_signed(integer( (SIN_AMPL*sin(math_2_pi*SIN_FREQ*t)) ), x_in'length));	
   		cic_nd <= '0';
   		wait for samp_period/2;
   		cic_nd <= '1';
   		wait for samp_period/2;									
		t := t + TSAMP;
	end loop;
   end process;
   x_in <= analog_sin;					-- Set up input to filter

END;
