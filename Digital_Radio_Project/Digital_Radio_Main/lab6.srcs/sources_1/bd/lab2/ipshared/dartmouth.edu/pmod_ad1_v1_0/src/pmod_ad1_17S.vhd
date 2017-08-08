----------------------------------------------------------------------------------
-- Company:         Engs 128 17S
-- Engineer:        Eric Hansen
-- 
-- Create Date:     07/21/2016 05:21:56 PM
-- Design Name:     Course prep
-- Module Name:     pmod_ad1 - Behavioral
-- Project Name:    Zybo_ADDApassthru
-- Target Devices:  Zynq, PmodAD1
-- Tool Versions:   Vivado 2016.1
-- Description:     SPI bus interface module to Digilent Pmod AD1 A/D converter
-- 
-- Dependencies:    Uses 20 MHz SPI SCLK
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

library UNISIM;
use UNISIM.VComponents.all;

entity pmod_ad1 is
    Port ( 
    --  System side (AXI-stream) interface 
           spi_aclk  : in STD_LOGIC; 
           resetn : in std_logic;                            
 --          m_axis_ad1_tready : in STD_LOGIC;                      -- unused
           m_axis_ad1_tvalid : out std_logic;
           m_axis_ad1_tdata : out STD_LOGIC_VECTOR (31 downto 0);   -- 2 ADCs on the Pmod
    -- System side non-AXI interface
           ad1_sample : in std_logic;
    -- ADC side (SPI) interface
           spi_sclk  : out STD_LOGIC;
           spi_syncL : out STD_LOGIC;
           spi_miso1 : in STD_LOGIC;
           spi_miso2 : in STD_LOGIC );
end pmod_ad1;

architecture Behavioral of pmod_ad1 is
    signal sresetn : std_logic := '0';
    signal ser_data_reg1, ser_data_reg2 : std_logic_vector(15 downto 0) := x"0000";
    signal shift_en : std_logic := '0';
    signal ad_data_ld : std_logic := '0';
    
    signal shift_count : unsigned(4 downto 0) := "00000";
    signal shift_count_clr : std_logic := '0';
    signal shift_count_tc : std_logic := '0';
    
    type state_type is (idle, shift, valid, finish);
    signal curr_state, next_state : state_type;
        
begin

-- Pass the clock through to the SPI bus
spi_sclk <= spi_aclk;                

Sync_reset: process(spi_aclk)                   -- synch for the reset signal
begin
    if rising_edge(spi_aclk) then
        sresetn <= resetn;
    end if;
end process Sync_reset;

-- Shift the serial data in from the SPI bus
spi_shift_register: process(spi_aclk)
begin
    if rising_edge(spi_aclk) then
        if shift_en = '1' then
            ser_data_reg1 <= ser_data_reg1(14 downto 0) & spi_miso1;
            ser_data_reg2 <= ser_data_reg2(14 downto 0) & spi_miso2;
        end if;
    end if;
end process spi_shift_register;

-- Store the received serial data in the AXI-stream output register
-- Flip the MSB to convert from offset binary to twos complement, extend the sign
Parallel_output_register: process(spi_aclk)      
begin
    if rising_edge(spi_aclk) then
        if sresetn = '0' then
            m_axis_ad1_tdata <= (others => '0');
        elsif ad_data_ld = '1' then
            m_axis_ad1_tdata <= 
                std_logic_vector( resize( signed( not(ser_data_reg2(11)) & ser_data_reg2(10 downto 0) ), 16) ) &
                std_logic_vector( resize( signed( not(ser_data_reg1(11)) & ser_data_reg1(10 downto 0) ), 16) );
        end if;
    end if;
end process Parallel_output_register;

-- Count the shifts of the serial register
spi_shift_counter: process(spi_aclk, shift_count)
begin
    if rising_edge(spi_aclk) then
        if shift_count_clr = '1' then
            shift_count <= (others => '0');
        elsif shift_en = '1' then
            shift_count <= shift_count + 1;
        end if;
    end if;
    
    if shift_count = 15 then        -- 14 or 15, not sure why yet. See ADCS7476 data sheet, Fig. 1
        shift_count_tc <= '1';
    else
        shift_count_tc <= '0';
    end if;
end process spi_shift_counter;

-- Controller
state_update: process(spi_aclk)
begin
    if rising_edge(spi_aclk) then
        if sresetn = '0' then
            curr_state <= idle;
        else
            curr_state <= next_state;
        end if;
    end if;
end process state_update;

FSM_logic: process(curr_state, ad1_sample, shift_count_tc) --, m_axis_ad1_tready)
begin
    next_state <= curr_state;
    shift_en <= '0';
    shift_count_clr <= '0';
    spi_syncL <= '1';
--    ad1_busy <= '0';
    ad_data_ld <= '0';
    m_axis_ad1_tvalid <= '0';
    
    -- TVALID is asserted from when the new data is loaded for output until the next conversion
    -- cycle starts.
    case curr_state is  -- (idle, shift, valid, finish)
        when idle => shift_count_clr <= '1'; m_axis_ad1_tvalid <= '1';
            if ad1_sample = '1' then            -- Assumed to have 50% duty cycle, so check for low at end
                next_state <= shift;
            end if;
            
        when shift => spi_syncL <= '0';  shift_en <= '1';
--                      ad1_busy <= '1'; 
            if shift_count_tc = '1' then
                next_state <= valid;
            end if;
        
        when valid => ad_data_ld <= '1';        -- New data to stream
--                       ad1_busy <= '1';
            next_state <= finish;
            
        when finish => m_axis_ad1_tvalid <= '1';
                       shift_count_clr <= '1';
            if ad1_sample = '0' then     
                next_state <= idle;
            end if;
        -- Assume that stream transfer occurs normally and TREADY can be ignored.
        -- With tight timing of ad1_sample and spi_sclk, don't really need to check
        -- ad1_sample here, but if ad1_sample were slower, we'd have to.
    end case;
end process FSM_logic;

end Behavioral;
