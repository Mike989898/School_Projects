--Copyright 1986-2016 Xilinx, Inc. All Rights Reserved.
----------------------------------------------------------------------------------
--Tool Version: Vivado v.2016.1 (win64) Build 1538259 Fri Apr  8 15:45:27 MDT 2016
--Date        : Mon May 29 17:16:04 2017
--Host        : mecha-3 running 64-bit Service Pack 1  (build 7601)
--Command     : generate_target lab2_wrapper.bd
--Design      : lab2_wrapper
--Purpose     : IP block netlist
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity lab2_wrapper is
  port (
    Clk125 : in STD_LOGIC;
    DDR_addr : inout STD_LOGIC_VECTOR ( 14 downto 0 );
    DDR_ba : inout STD_LOGIC_VECTOR ( 2 downto 0 );
    DDR_cas_n : inout STD_LOGIC;
    DDR_ck_n : inout STD_LOGIC;
    DDR_ck_p : inout STD_LOGIC;
    DDR_cke : inout STD_LOGIC;
    DDR_cs_n : inout STD_LOGIC;
    DDR_dm : inout STD_LOGIC_VECTOR ( 3 downto 0 );
    DDR_dq : inout STD_LOGIC_VECTOR ( 31 downto 0 );
    DDR_dqs_n : inout STD_LOGIC_VECTOR ( 3 downto 0 );
    DDR_dqs_p : inout STD_LOGIC_VECTOR ( 3 downto 0 );
    DDR_odt : inout STD_LOGIC;
    DDR_ras_n : inout STD_LOGIC;
    DDR_reset_n : inout STD_LOGIC;
    DDR_we_n : inout STD_LOGIC;
    FIXED_IO_ddr_vrn : inout STD_LOGIC;
    FIXED_IO_ddr_vrp : inout STD_LOGIC;
    FIXED_IO_mio : inout STD_LOGIC_VECTOR ( 53 downto 0 );
    FIXED_IO_ps_clk : inout STD_LOGIC;
    FIXED_IO_ps_porb : inout STD_LOGIC;
    FIXED_IO_ps_srstb : inout STD_LOGIC;
    ad1_spi_miso1 : in STD_LOGIC;
    ad1_spi_miso2 : in STD_LOGIC;
    ad1_spi_sclk : out STD_LOGIC;
    ad1_spi_syncL : out STD_LOGIC;
    btns_4bits_tri_i : in STD_LOGIC_VECTOR ( 3 downto 0 );
    clp_control_tri_o : out STD_LOGIC_VECTOR ( 3 downto 0 );
    clp_data_tri_io : inout STD_LOGIC_VECTOR ( 7 downto 0 );
    da2_spi_mosi1 : out STD_LOGIC;
    da2_spi_mosi2 : out STD_LOGIC;
    da2_spi_sclk : out STD_LOGIC;
    da2_spi_syncL : out STD_LOGIC;
    sws_4bits_tri_i : in STD_LOGIC_VECTOR ( 3 downto 0 )
  );
end lab2_wrapper;

architecture STRUCTURE of lab2_wrapper is
  component lab2 is
  port (
    CLP_CONTROL_tri_o : out STD_LOGIC_VECTOR ( 3 downto 0 );
    CLP_DATA_tri_i : in STD_LOGIC_VECTOR ( 7 downto 0 );
    CLP_DATA_tri_o : out STD_LOGIC_VECTOR ( 7 downto 0 );
    CLP_DATA_tri_t : out STD_LOGIC_VECTOR ( 7 downto 0 );
    btns_4bits_tri_i : in STD_LOGIC_VECTOR ( 3 downto 0 );
    sws_4bits_tri_i : in STD_LOGIC_VECTOR ( 3 downto 0 );
    Clk125 : in STD_LOGIC;
    da2_spi_sclk : out STD_LOGIC;
    da2_spi_syncL : out STD_LOGIC;
    da2_spi_mosi1 : out STD_LOGIC;
    da2_spi_mosi2 : out STD_LOGIC;
    ad1_spi_miso2 : in STD_LOGIC;
    ad1_spi_miso1 : in STD_LOGIC;
    ad1_spi_sclk : out STD_LOGIC;
    ad1_spi_syncL : out STD_LOGIC;
    DDR_cas_n : inout STD_LOGIC;
    DDR_cke : inout STD_LOGIC;
    DDR_ck_n : inout STD_LOGIC;
    DDR_ck_p : inout STD_LOGIC;
    DDR_cs_n : inout STD_LOGIC;
    DDR_reset_n : inout STD_LOGIC;
    DDR_odt : inout STD_LOGIC;
    DDR_ras_n : inout STD_LOGIC;
    DDR_we_n : inout STD_LOGIC;
    DDR_ba : inout STD_LOGIC_VECTOR ( 2 downto 0 );
    DDR_addr : inout STD_LOGIC_VECTOR ( 14 downto 0 );
    DDR_dm : inout STD_LOGIC_VECTOR ( 3 downto 0 );
    DDR_dq : inout STD_LOGIC_VECTOR ( 31 downto 0 );
    DDR_dqs_n : inout STD_LOGIC_VECTOR ( 3 downto 0 );
    DDR_dqs_p : inout STD_LOGIC_VECTOR ( 3 downto 0 );
    FIXED_IO_mio : inout STD_LOGIC_VECTOR ( 53 downto 0 );
    FIXED_IO_ddr_vrn : inout STD_LOGIC;
    FIXED_IO_ddr_vrp : inout STD_LOGIC;
    FIXED_IO_ps_srstb : inout STD_LOGIC;
    FIXED_IO_ps_clk : inout STD_LOGIC;
    FIXED_IO_ps_porb : inout STD_LOGIC
  );
  end component lab2;
  component IOBUF is
  port (
    I : in STD_LOGIC;
    O : out STD_LOGIC;
    T : in STD_LOGIC;
    IO : inout STD_LOGIC
  );
  end component IOBUF;
  signal clp_data_tri_i_0 : STD_LOGIC_VECTOR ( 0 to 0 );
  signal clp_data_tri_i_1 : STD_LOGIC_VECTOR ( 1 to 1 );
  signal clp_data_tri_i_2 : STD_LOGIC_VECTOR ( 2 to 2 );
  signal clp_data_tri_i_3 : STD_LOGIC_VECTOR ( 3 to 3 );
  signal clp_data_tri_i_4 : STD_LOGIC_VECTOR ( 4 to 4 );
  signal clp_data_tri_i_5 : STD_LOGIC_VECTOR ( 5 to 5 );
  signal clp_data_tri_i_6 : STD_LOGIC_VECTOR ( 6 to 6 );
  signal clp_data_tri_i_7 : STD_LOGIC_VECTOR ( 7 to 7 );
  signal clp_data_tri_io_0 : STD_LOGIC_VECTOR ( 0 to 0 );
  signal clp_data_tri_io_1 : STD_LOGIC_VECTOR ( 1 to 1 );
  signal clp_data_tri_io_2 : STD_LOGIC_VECTOR ( 2 to 2 );
  signal clp_data_tri_io_3 : STD_LOGIC_VECTOR ( 3 to 3 );
  signal clp_data_tri_io_4 : STD_LOGIC_VECTOR ( 4 to 4 );
  signal clp_data_tri_io_5 : STD_LOGIC_VECTOR ( 5 to 5 );
  signal clp_data_tri_io_6 : STD_LOGIC_VECTOR ( 6 to 6 );
  signal clp_data_tri_io_7 : STD_LOGIC_VECTOR ( 7 to 7 );
  signal clp_data_tri_o_0 : STD_LOGIC_VECTOR ( 0 to 0 );
  signal clp_data_tri_o_1 : STD_LOGIC_VECTOR ( 1 to 1 );
  signal clp_data_tri_o_2 : STD_LOGIC_VECTOR ( 2 to 2 );
  signal clp_data_tri_o_3 : STD_LOGIC_VECTOR ( 3 to 3 );
  signal clp_data_tri_o_4 : STD_LOGIC_VECTOR ( 4 to 4 );
  signal clp_data_tri_o_5 : STD_LOGIC_VECTOR ( 5 to 5 );
  signal clp_data_tri_o_6 : STD_LOGIC_VECTOR ( 6 to 6 );
  signal clp_data_tri_o_7 : STD_LOGIC_VECTOR ( 7 to 7 );
  signal clp_data_tri_t_0 : STD_LOGIC_VECTOR ( 0 to 0 );
  signal clp_data_tri_t_1 : STD_LOGIC_VECTOR ( 1 to 1 );
  signal clp_data_tri_t_2 : STD_LOGIC_VECTOR ( 2 to 2 );
  signal clp_data_tri_t_3 : STD_LOGIC_VECTOR ( 3 to 3 );
  signal clp_data_tri_t_4 : STD_LOGIC_VECTOR ( 4 to 4 );
  signal clp_data_tri_t_5 : STD_LOGIC_VECTOR ( 5 to 5 );
  signal clp_data_tri_t_6 : STD_LOGIC_VECTOR ( 6 to 6 );
  signal clp_data_tri_t_7 : STD_LOGIC_VECTOR ( 7 to 7 );
begin
clp_data_tri_iobuf_0: component IOBUF
     port map (
      I => clp_data_tri_o_0(0),
      IO => clp_data_tri_io(0),
      O => clp_data_tri_i_0(0),
      T => clp_data_tri_t_0(0)
    );
clp_data_tri_iobuf_1: component IOBUF
     port map (
      I => clp_data_tri_o_1(1),
      IO => clp_data_tri_io(1),
      O => clp_data_tri_i_1(1),
      T => clp_data_tri_t_1(1)
    );
clp_data_tri_iobuf_2: component IOBUF
     port map (
      I => clp_data_tri_o_2(2),
      IO => clp_data_tri_io(2),
      O => clp_data_tri_i_2(2),
      T => clp_data_tri_t_2(2)
    );
clp_data_tri_iobuf_3: component IOBUF
     port map (
      I => clp_data_tri_o_3(3),
      IO => clp_data_tri_io(3),
      O => clp_data_tri_i_3(3),
      T => clp_data_tri_t_3(3)
    );
clp_data_tri_iobuf_4: component IOBUF
     port map (
      I => clp_data_tri_o_4(4),
      IO => clp_data_tri_io(4),
      O => clp_data_tri_i_4(4),
      T => clp_data_tri_t_4(4)
    );
clp_data_tri_iobuf_5: component IOBUF
     port map (
      I => clp_data_tri_o_5(5),
      IO => clp_data_tri_io(5),
      O => clp_data_tri_i_5(5),
      T => clp_data_tri_t_5(5)
    );
clp_data_tri_iobuf_6: component IOBUF
     port map (
      I => clp_data_tri_o_6(6),
      IO => clp_data_tri_io(6),
      O => clp_data_tri_i_6(6),
      T => clp_data_tri_t_6(6)
    );
clp_data_tri_iobuf_7: component IOBUF
     port map (
      I => clp_data_tri_o_7(7),
      IO => clp_data_tri_io(7),
      O => clp_data_tri_i_7(7),
      T => clp_data_tri_t_7(7)
    );
lab2_i: component lab2
     port map (
      CLP_CONTROL_tri_o(3 downto 0) => clp_control_tri_o(3 downto 0),
      CLP_DATA_tri_i(7) => clp_data_tri_i_7(7),
      CLP_DATA_tri_i(6) => clp_data_tri_i_6(6),
      CLP_DATA_tri_i(5) => clp_data_tri_i_5(5),
      CLP_DATA_tri_i(4) => clp_data_tri_i_4(4),
      CLP_DATA_tri_i(3) => clp_data_tri_i_3(3),
      CLP_DATA_tri_i(2) => clp_data_tri_i_2(2),
      CLP_DATA_tri_i(1) => clp_data_tri_i_1(1),
      CLP_DATA_tri_i(0) => clp_data_tri_i_0(0),
      CLP_DATA_tri_o(7) => clp_data_tri_o_7(7),
      CLP_DATA_tri_o(6) => clp_data_tri_o_6(6),
      CLP_DATA_tri_o(5) => clp_data_tri_o_5(5),
      CLP_DATA_tri_o(4) => clp_data_tri_o_4(4),
      CLP_DATA_tri_o(3) => clp_data_tri_o_3(3),
      CLP_DATA_tri_o(2) => clp_data_tri_o_2(2),
      CLP_DATA_tri_o(1) => clp_data_tri_o_1(1),
      CLP_DATA_tri_o(0) => clp_data_tri_o_0(0),
      CLP_DATA_tri_t(7) => clp_data_tri_t_7(7),
      CLP_DATA_tri_t(6) => clp_data_tri_t_6(6),
      CLP_DATA_tri_t(5) => clp_data_tri_t_5(5),
      CLP_DATA_tri_t(4) => clp_data_tri_t_4(4),
      CLP_DATA_tri_t(3) => clp_data_tri_t_3(3),
      CLP_DATA_tri_t(2) => clp_data_tri_t_2(2),
      CLP_DATA_tri_t(1) => clp_data_tri_t_1(1),
      CLP_DATA_tri_t(0) => clp_data_tri_t_0(0),
      Clk125 => Clk125,
      DDR_addr(14 downto 0) => DDR_addr(14 downto 0),
      DDR_ba(2 downto 0) => DDR_ba(2 downto 0),
      DDR_cas_n => DDR_cas_n,
      DDR_ck_n => DDR_ck_n,
      DDR_ck_p => DDR_ck_p,
      DDR_cke => DDR_cke,
      DDR_cs_n => DDR_cs_n,
      DDR_dm(3 downto 0) => DDR_dm(3 downto 0),
      DDR_dq(31 downto 0) => DDR_dq(31 downto 0),
      DDR_dqs_n(3 downto 0) => DDR_dqs_n(3 downto 0),
      DDR_dqs_p(3 downto 0) => DDR_dqs_p(3 downto 0),
      DDR_odt => DDR_odt,
      DDR_ras_n => DDR_ras_n,
      DDR_reset_n => DDR_reset_n,
      DDR_we_n => DDR_we_n,
      FIXED_IO_ddr_vrn => FIXED_IO_ddr_vrn,
      FIXED_IO_ddr_vrp => FIXED_IO_ddr_vrp,
      FIXED_IO_mio(53 downto 0) => FIXED_IO_mio(53 downto 0),
      FIXED_IO_ps_clk => FIXED_IO_ps_clk,
      FIXED_IO_ps_porb => FIXED_IO_ps_porb,
      FIXED_IO_ps_srstb => FIXED_IO_ps_srstb,
      ad1_spi_miso1 => ad1_spi_miso1,
      ad1_spi_miso2 => ad1_spi_miso2,
      ad1_spi_sclk => ad1_spi_sclk,
      ad1_spi_syncL => ad1_spi_syncL,
      btns_4bits_tri_i(3 downto 0) => btns_4bits_tri_i(3 downto 0),
      da2_spi_mosi1 => da2_spi_mosi1,
      da2_spi_mosi2 => da2_spi_mosi2,
      da2_spi_sclk => da2_spi_sclk,
      da2_spi_syncL => da2_spi_syncL,
      sws_4bits_tri_i(3 downto 0) => sws_4bits_tri_i(3 downto 0)
    );
end STRUCTURE;
