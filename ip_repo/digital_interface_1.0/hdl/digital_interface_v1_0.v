
`timescale 1 ns / 1 ps

module digital_interface_v1_0 #
  (
   // Users to add parameters here

   // User parameters ends
   // Do not modify the parameters beyond this line


   // Parameters of Axi Slave Bus Interface S00_AXI
   parameter integer C_S00_AXI_DATA_WIDTH	= 32,
   parameter integer C_S00_AXI_ADDR_WIDTH	= 4
   )
   (
    // Users to add ports here

    (* dont_touch = "true" *) input wire[7:0] DATA0_I,
    (* dont_touch = "true" *) input wire [7:0] DATA1_I,
    (* dont_touch = "true" *) output wire [7:0] DATA0_O,
    (* dont_touch = "true" *) output wire [7:0] DATA1_O,
    (* dont_touch = "true" *) output wire [7:0] DATA0_T, // Data enable
    (* dont_touch = "true" *) output wire [7:0] DATA1_T,
    output wire[7:0] CTL0,
    output wire[7:0] CTL1,

    input wire AMIGA_LTS_OUT,
    input wire AMIGA_CLOCK_OUT,
    input wire AMIGA_TX,
    output wire AMIGA_RX,
         
    input wire RD_TRIG,
    input wire RD_MOSI,
    input wire RD_SCK,
    input wire RD_CE,
    output wire RD_XFR_CLK,
    output wire RD_SER_DATA0,
    output wire RD_SER_DATA1,
    output wire RD_MISO,

    output wire DBG1,
    output wire DBG2,
    output wire DBG3,

    // User ports ends
    // Do not modify the ports beyond this line


    // Ports of Axi Slave Bus Interface S00_AXI
    input wire  s00_axi_aclk,
    input wire  s00_axi_aresetn,
    input wire [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_awaddr,
    input wire [2 : 0] s00_axi_awprot,
    input wire  s00_axi_awvalid,
    output wire  s00_axi_awready,
    input wire [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_wdata,
    input wire [(C_S00_AXI_DATA_WIDTH/8)-1 : 0] s00_axi_wstrb,
    input wire  s00_axi_wvalid,
    output wire  s00_axi_wready,
    output wire [1 : 0] s00_axi_bresp,
    output wire  s00_axi_bvalid,
    input wire  s00_axi_bready,
    input wire [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_araddr,
    input wire [2 : 0] s00_axi_arprot,
    input wire  s00_axi_arvalid,
    output wire  s00_axi_arready,
    output wire [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_rdata,
    output wire [1 : 0] s00_axi_rresp,
    output wire  s00_axi_rvalid,
    input wire  s00_axi_rready
    );

   
   // Instantiation of Axi Bus Interface S00_AXI
   digital_interface_v1_0_S00_AXI # ( 
		                      .C_S_AXI_DATA_WIDTH(C_S00_AXI_DATA_WIDTH),
		                      .C_S_AXI_ADDR_WIDTH(C_S00_AXI_ADDR_WIDTH)
	                              ) digital_interface_v1_0_S00_AXI_inst
     (
      .S_AXI_ACLK(s00_axi_aclk),
      .S_AXI_ARESETN(s00_axi_aresetn),
      .S_AXI_AWADDR(s00_axi_awaddr),
      .S_AXI_AWPROT(s00_axi_awprot),
      .S_AXI_AWVALID(s00_axi_awvalid),
      .S_AXI_AWREADY(s00_axi_awready),
      .S_AXI_WDATA(s00_axi_wdata),
      .S_AXI_WSTRB(s00_axi_wstrb),
      .S_AXI_WVALID(s00_axi_wvalid),
      .S_AXI_WREADY(s00_axi_wready),
      .S_AXI_BRESP(s00_axi_bresp),
      .S_AXI_BVALID(s00_axi_bvalid),
      .S_AXI_BREADY(s00_axi_bready),
      .S_AXI_ARADDR(s00_axi_araddr),
      .S_AXI_ARPROT(s00_axi_arprot),
      .S_AXI_ARVALID(s00_axi_arvalid),
      .S_AXI_ARREADY(s00_axi_arready),
      .S_AXI_RDATA(s00_axi_rdata),
      .S_AXI_RRESP(s00_axi_rresp),
      .S_AXI_RVALID(s00_axi_rvalid),
      .S_AXI_RREADY(s00_axi_rready),
      .DATA0_I(DATA0_I),
      .DATA1_I(DATA1_I),
      .DATA0_O(DATA0_O),
      .DATA1_O(DATA1_O),
      .DATA0_T(DATA0_T),
      .DATA1_T(DATA1_T),
      .CTL0(CTL0),
      .CTL1(CTL1),
      .AMIGA_LTS_OUT(AMIGA_LTS_OUT),
      .AMIGA_CLOCK_OUT(AMIGA_CLOCK_OUT),
      .AMIGA_TX(AMIGA_TX),
      .AMIGA_RX(AMIGA_RX),
      .RD_TRIG(RD_TRIG),
      .RD_MISO(RD_MISO),
      .RD_SCK(RD_SCK),
      .RD_CE(RD_CE),
      .RD_XFR_CLK(RD_XFR_CLK),
      .RD_SER_DATA0(RD_SER_DATA0),
      .RD_SER_DATA1(RD_SER_DATA1),
      .RD_MOSI(RD_MOSI),
      .DBG1(DBG1),
      .DBG2(DBG2),
      .DBG3(DBG3)
      );

   // Add user logic here

endmodule
