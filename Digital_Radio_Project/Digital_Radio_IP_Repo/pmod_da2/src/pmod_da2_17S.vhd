----------------------------------------------------------------------------------
-- Company:         Thayer School, Engs 128 17S
-- Engineer:        E.W. Hansen
-- 
-- Create Date:     02/14/2017 08:44:46 AM
-- Design Name:     Engs128_ip_repo
-- Module Name:     pmod_da2_17S - Behavioral
-- Project Name:    
-- Target Devices:  Zync, PmodDA2
-- Tool Versions:   Vivado 2016.1
-- Description:     This code instantiates a simple FSM to latch data and send it 
--                  to the twin DAC121S101s on the Digilent DA2 PMOD.
-- 
-- Dependencies:    Clocked by the SPI bus SCLK, which cannot exceed 20 MHz
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Revision 1.00 (EWH, 1-4 April 2017) --- modify for packaging as an AXI-stream IP block
--
-- Additional Comments:
-- Signal polarity
-- The DAC chip operates from a single-ended power supply, and the input is assumed to 
-- be straight binary ("offset binary"), not twos complement.  The msb (bit 11) of the
-- input data is flipped prior to conversion to shift bipolar data up by x800.  The AXI
-- bus input is two 16-bit words.  The upper four bits of each word are masked out.
--
-- Timing
-- The DAC121S101 has a maximum SPI serial clock frequency of 30 MHz.
-- A single conversion takes 16 SCLK cycles + 20ns.  For example, if clocked at
-- 20 MHz, a single conversion takes 820 ns.  It is therefore possible to convert 
-- at a 1 MHz rate.  But note that the DAC has an 8 us (typ) settling time which 
-- should be taken into account, so that a more practical sampling rate would be 
-- closer to 125 kHz.  Moreover, the output slew rate is limited to 1 V/us, which 
-- limits a full scale sine wave output to 48.2 kHz(about 96 kHz Nyquist sampling 
-- rate).
-- 
-- Operation 
-- To initiate a conversion, put data to tdata, assert tvalid (usually supplied by
-- AXI stream source), assert the dac_start signal.  The AXI bus here has no tready
-- signal, since our application should not need to apply back pressure to the 
-- processing chain.
----------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity pmod_da2 is
  port(
  -- System-side interface
    aclk                : in  std_logic;				        -- clock (identical to SCLK)
    resetn              : in  std_logic;                        -- reset (low true)
    dac_start           : in  std_logic;                        -- start conversion
    s_axis_da2_tvalid   : in  std_logic;                        -- data is streaming
    s_axis_da2_tdata    : in  std_logic_vector(31 downto 0);    -- data to be converted (ch1, ch0, each extended from 12 to 16 bits)
--    s_axis_da2_tready   : out std_logic;				        -- conversion in progress (back pressure to source)
  -- DAC-side (SPI) interface
    spi_sclk            : out std_logic;			             -- serial clock
    spi_syncL           : out std_logic;			             -- low-true sync signal
    spi_mosi1           : out std_logic;			             -- serial data out
    spi_mosi2           : out std_logic
    );
end pmod_da2;

architecture behavioral of pmod_da2 is
  type spi_states is (s0, s1, s2, s3, s4);
  signal spi_cs, spi_ns : spi_states;   	-- for the FSM, current and next states

  signal reg_ld, reg_sh : std_logic := '0';
  signal reg1, reg2     : std_logic_vector(15 downto 0) := x"0000";	-- internal da2_data 
  signal count          : unsigned(3 downto 0) := x"0";
  signal sresetn        : std_logic := '0';
  
begin

spi_sclk <= aclk;                           -- pass sclk to SPI bus

Sync_reset: process(aclk)                   -- synch for the reset signal
begin
    if rising_edge(aclk) then
        sresetn <= resetn;
    end if;
end process Sync_reset;

Parallel_to_serial: process(aclk, reg1, reg2)
begin
	if rising_edge(aclk) then
		if sresetn = '0' then
			reg1 <= x"0000";                  -- clear everything
			reg2 <= x"0000";
			count <= x"0";
		elsif reg_ld = '1' then
			reg1 <= "0000" & not(s_axis_da2_tdata(11)) & s_axis_da2_tdata(10 downto 0);      -- load registers with 12 lsbs 
			reg2 <= "0000" & not(s_axis_da2_tdata(27)) & s_axis_da2_tdata(26 downto 16);     -- of upper and lower words  
			count <= x"0";
		elsif reg_sh = '1' then
			reg1 <= reg1(14 downto 0) & '0'; -- left shift
			reg2 <= reg2(14 downto 0) & '0';   
			count <= count+1;                -- increment shift counter
		end if;
	end if;
	spi_mosi1 <= reg1(15);					 -- output = shift register msb
	spi_mosi2 <= reg2(15);
end process;	
 
SPI_State_Update: process(aclk)
begin		 
	if rising_edge(aclk) then
    	if sresetn = '0' then
      		spi_cs <= s0;
    	else
      		spi_cs <= spi_ns;
		end if;
    end if;
end process;
  
SPI_Next_State: process(spi_cs, s_axis_da2_tvalid, dac_start, count)
begin
    -- defaults here
    spi_syncL   <= '1';				-- SPI bus signals			
--    s_axis_da2_tready  <= '1';				-- status bit
	reg_ld	    <= '0';				-- internal controls
	reg_sh		<= '0';
    spi_ns      <= spi_cs;          -- state vector
     
    case spi_cs is
    -- wait for go
    	when s0 => 	                    
            if (s_axis_da2_tvalid='1') and (dac_start='1')
                then spi_ns <= s1;
            end if;
            
    -- load the register, clear the shift counter           		    
		when s1 => reg_ld <= '1';     
			spi_ns <= s2;

    -- assert sync, busy
    -- shift the register 16 times to send data into the DAC	
		when s2 => spi_syncL <= '0'; reg_sh <= '1'; 
--		           s_axis_da2_tready <= '0';
            if count = 15
                then spi_ns <= s3;
            end if;
    
    -- deassert sync, busy, go back to top
		when s3 => 
            if dac_start='0'        -- so we don't retrigger
                then spi_ns <= s0;
            end if;
			
		when others => 
			spi_ns <= s0;
	end case;
end process;

end behavioral;
