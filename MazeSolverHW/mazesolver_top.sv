/*---------------------------------------------------------------------------
  --      lab7.sv                                                          --
  --      Christine Chen                                                   --
  --      10/23/2013                                                       --
  --      modified by Zuofu Cheng                                          --
  --      For use with ECE 385                                             --
  --      UIUC ECE Department                                              --
  ---------------------------------------------------------------------------*/
// Top-level module that integrates the Nios II system with the rest of the hardware

module mazesolver_top(
    input	      CLOCK_50, 
    input  [1:0]  KEY,
    output [7:0]  LEDG,
    output [12:0] DRAM_ADDR,
    output [1:0]  DRAM_BA,
    output        DRAM_CAS_N,
    output		   DRAM_CKE,
    output		   DRAM_CS_N,
    inout  [31:0] DRAM_DQ,
    output [3:0]  DRAM_DQM,
    output		   DRAM_RAS_N,
    output		   DRAM_WE_N,
    output		   DRAM_CLK
);

mazesolver_soc m_mazesolver_soc(
    .clk_clk(CLOCK_50),
    .reset_reset_n(KEY[0]), 
    .led_wire_export(LEDG),
	 .key_1_wire_export(KEY[1]),
    .sdram_wire_addr(DRAM_ADDR),    //  sdram_wire.addr
    .sdram_wire_ba(DRAM_BA),      	//  .ba
    .sdram_wire_cas_n(DRAM_CAS_N),    //  .cas_n
    .sdram_wire_cke(DRAM_CKE),     	//  .cke
    .sdram_wire_cs_n(DRAM_CS_N),      //  .cs_n
    .sdram_wire_dq(DRAM_DQ),      	//  .dq
    .sdram_wire_dqm(DRAM_DQM),     	//  .dqm
    .sdram_wire_ras_n(DRAM_RAS_N),    //  .ras_n
    .sdram_wire_we_n(DRAM_WE_N),      //  .we_n
    .sdram_clk_clk(DRAM_CLK)			//  clock out to SDRAM from other PLL port
);

//Instantiate additional FPGA fabric modules as needed

endmodule
