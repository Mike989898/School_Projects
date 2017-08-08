----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 05/10/2017 07:54:54 PM
-- Design Name: 
-- Module Name: FIR_control - Behavioral
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

entity FIR is
    Generic (DATA_IN_WIDTH: integer := 16;
             DATA_OUT_SIGNAL_WIDTH : integer := 16;
             DATA_OUT_WIDTH : integer := 12;
             ACCUM_REG_WIDTH : integer := 32);
    Port ( next_sample_en : in STD_LOGIC;
           x_in : in STD_LOGIC_VECTOR (DATA_IN_WIDTH-1 downto 0);
           clk_in : in STD_LOGIC;
           x_out : out STD_LOGIC_VECTOR(DATA_OUT_SIGNAL_WIDTH-1 downto 0)
       );
end FIR;

architecture Behavioral of FIR is

COMPONENT blk_mem_gen_0
  PORT (
    clka : IN STD_LOGIC;
    ena : IN STD_LOGIC;
    wea : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    addra : IN STD_LOGIC_VECTOR(6 DOWNTO 0);
    dina : IN STD_LOGIC_VECTOR(15 DOWNTO 0);
    douta : OUT STD_LOGIC_VECTOR(15 DOWNTO 0)
  );
END COMPONENT;

component block_ram_stack is
    Generic( M : in integer);
    Port ( addra : in STD_LOGIC_VECTOR (6 downto 0);
           clka : in STD_LOGIC;
           dina : in STD_LOGIC_VECTOR (15 downto 0);
           douta : out STD_LOGIC_VECTOR (15 downto 0);
           ena : in STD_LOGIC;
           wea : in STD_LOGIC);
end component;

--signals for x_in block ram
signal x_in_addra : STD_LOGIC_VECTOR (6 downto 0) := (others => '0');
signal x_in_dina : STD_LOGIC_VECTOR (15 downto 0) := (others => '0');
signal x_in_enable : STD_LOGIC := '0';
signal x_in_douta : STD_LOGIC_VECTOR (15 downto 0) := (others => '0');
signal x_in_wea : STD_LOGIC := '0';

--signals for const block ram
signal const_addra : STD_LOGIC_VECTOR (6 downto 0) := (others => '0');
signal const_dina : STD_LOGIC_VECTOR (15 downto 0) := (others => '0');
signal const_douta : STD_LOGIC_VECTOR (15 downto 0) := (others => '0');
signal const_wea : STD_LOGIC_VECTOR(0 downto 0) := (others => '0');
signal const_enable : STD_LOGIC := '0';

--state machine
type controller_state is (ST0, ST1, ST2, ST3, ST4, ST5, ST6);
signal CURR_STATE, NEXT_STATE : controller_state;

--output reg
signal output_reg : STD_LOGIC_VECTOR (DATA_OUT_SIGNAL_WIDTH-1 downto 0) := (others => '0');
signal output_reg_en : STD_LOGIC := '0';

--input reg
signal input_reg : STD_LOGIC_VECTOR(DATA_IN_WIDTH-1 downto 0) := (others => '0');

--accumulator reg
signal accum_reg : signed ( ACCUM_REG_WIDTH-1 downto 0) := (others => '0');
signal accum_reg_en : STD_LOGIC := '0';
signal accum_clear : STD_LOGIC := '0';

--address shift adder
signal addr_to_fetch : unsigned (6 downto 0) := (others => '0');
signal addr_to_fetch_en : std_logic := '0';
signal addr_to_fetch_rst : std_logic := '0';

-- Number of coefficients
constant M : integer := 71;

begin

x_in_block: component block_ram_stack
    Generic map (M => M)
     port map (
      addra => std_logic_vector(addr_to_fetch),
      clka => clk_in,
      dina => input_reg,
      douta => x_in_douta,
      ena => x_in_enable,
      wea => x_in_wea
    );

const_block: component blk_mem_gen_0
     port map (
      addra => std_logic_vector(addr_to_fetch),
      clka => clk_in,
      dina => const_dina,
      douta => const_douta,
      ena => const_enable,
      wea => const_wea
    );
    
const_dina <= (others => '0');

--output register
output_reg_update: process(clk_in) 
begin
    if rising_edge(clk_in) AND output_reg_en = '1' then
        output_reg(DATA_OUT_WIDTH-1 downto 0) <= std_logic_vector(accum_reg(accum_reg'length-1 downto accum_reg'length - DATA_OUT_WIDTH));
    end if;
end process output_reg_update;
x_out <= output_reg;

--input register
input_reg_proc : process(clk_in)
begin
    if rising_edge(clk_in) AND next_sample_en = '1' then
        input_reg <= x_in;
    end if;
end process input_reg_proc;

--accumulator register
accum_reg_update: process(clk_in, accum_clear)
begin
 if rising_edge(clk_in) then
    if accum_reg_en = '1' then 
        accum_reg <= (signed(const_douta)*signed(x_in_douta)) + signed(accum_reg);
    elsif accum_clear = '1' then
        accum_reg <= (others => '0');
    end if;
 end if;
end process accum_reg_update;

--next address adder
addr_to_fetch_proc: process(clk_in)
begin
    if rising_edge(clk_in) then
        if addr_to_fetch_en = '1' then 
            addr_to_fetch <= addr_to_fetch + 1;
        elsif addr_to_fetch_rst = '1' then
            addr_to_fetch <= (others => '0');
        end if;
    end if;
end process addr_to_fetch_proc;

--state machine reg
state_machine_reg: process(clk_in)
begin 
    if rising_edge(clk_in) then
        CURR_STATE <= NEXT_STATE;
    end if;
end process state_machine_reg;

--state machine logic
state_machine_proc: process(CURR_STATE, next_sample_en)
begin
    --SIGNAL DEFAULTS
    NEXT_STATE <= CURR_STATE;
    addr_to_fetch_en <= '0';
    addr_to_fetch_rst <= '0';
    accum_reg_en <= '0';
    accum_clear <= '0';
    const_enable <= '0';
    x_in_wea <= '0';
    x_in_enable <= '0';
    output_reg_en <= '0';
    case CURR_STATE is
    when ST0 =>
        if next_sample_en = '1' then
            NEXT_STATE <=ST1;
        end if;
    when ST1 =>
        accum_clear <= '1';
        x_in_enable <= '1';
        x_in_wea <= '1';
        NEXT_STATE <= ST2;
    when ST2 =>
        x_in_enable <= '1';
        const_enable <= '1';
        NEXT_STATE <= ST3;
    when ST3 =>
       -- Data takes 2 clock cycles to come out of the block ram, this state just ensures
       -- that the data is ready.
       x_in_enable <= '1';
       const_enable <= '1';
       NEXT_STATE <= ST4;
    when ST4 =>
        x_in_enable <= '1';
        const_enable <= '1';
        accum_reg_en <= '1';
        if addr_to_fetch < M-1 then
            NEXT_STATE <= ST5;
        else
            NEXT_STATE <= ST6;
        end if;
    when ST5 =>
        addr_to_fetch_en <= '1';
        NEXT_STATE <= ST2;
    when ST6 =>
        addr_to_fetch_rst <= '1';
        output_reg_en <= '1';
        NEXT_STATE <= ST0;
    end case;

end process state_machine_proc;

end Behavioral;
