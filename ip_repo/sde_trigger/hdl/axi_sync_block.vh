// axi_sync.vh
// 
// This is the code portion of the sde_trigger that is does the
// synchronization with the AXI bus.  
// 
// 22-Nov-2016 DFN Split off from sde_trigger_code.vh because that filee
//                 was getting too large.


   always @( posedge S_AXI_ACLK )
     begin
        if (slv_reg_wren &&
            ( axi_awaddr[ADDR_LSB+OPT_MEM_ADDR_BITS:ADDR_LSB] 
              == `SHWR_BUF_CONTROL_ADDR ))
          AXI_SHWR_CONTROL_WRITTEN <= 1;
        else
          AXI_SHWR_CONTROL_WRITTEN <= 0;
        if (slv_reg_wren &&
            ( axi_awaddr[ADDR_LSB+OPT_MEM_ADDR_BITS:ADDR_LSB] 
              == `MUON_BUF_CONTROL_ADDR ))
          AXI_MUON_CONTROL_WRITTEN <= 1;
        else
          AXI_MUON_CONTROL_WRITTEN <= 0;
        AXI_REG_WRITE <= slv_reg_wren;
	P6X[3] <= AXI_SHWR_CONTROL_WRITTEN;
     end

   // Synchronization with AXI registers for cases where glitches would be
   // problematic.  This should not be necessary for semi-static control registers.

   synchronizer_32bit compatibility_global_control_sync
     (.ASYNC_IN(COMPATIBILITY_GLOBAL_CONTROL),
      .CLK(CLK120),
      .SYNC_OUT(LCL_COMPATIBILITY_GLOBAL_CONTROL));
   synchronizer_32bit shwr_buf_control_sync(.ASYNC_IN(SHWR_BUF_CONTROL),
                                            .CLK(CLK120),
                                            .SYNC_OUT(LCL_SHWR_BUF_CONTROL));
   synchronizer_32bit led__control_sync(.ASYNC_IN(LED_CONTROL),
                                            .CLK(CLK120),
                                            .SYNC_OUT(LCL_LED_CONTROL));
   synchronizer_1bit control_written_sync(.ASYNC_IN(AXI_SHWR_CONTROL_WRITTEN),
                                          .CLK(CLK120),
                                          .SYNC_OUT(LCL_SHWR_CONTROL_WRITTEN));

   synchronizer_32bit shwr_trigid_sync
     (.ASYNC_IN(LCL_SHWR_BUF_TRIG_ID),
      .CLK(S_AXI_ACLK),.SYNC_OUT(SHWR_BUF_TRIG_ID));
   synchronizer_32bit shwr_buf_status_sync
     (.ASYNC_IN(LCL_SHWR_BUF_STATUS),
      .CLK(S_AXI_ACLK),.SYNC_OUT(SHWR_BUF_STATUS));
   synchronizer_32bit shwr_buf_start_sync
     (.ASYNC_IN(LCL_SHWR_BUF_START),
      .CLK(S_AXI_ACLK),.SYNC_OUT(SHWR_BUF_START));

// Peak, area, & baseline
   synchronizer_32bit shwr_peak_area0
     (.ASYNC_IN(LCL_SHWR_PEAK_AREA0[SHWR_BUF_RNUM]),
      .CLK(S_AXI_ACLK),.SYNC_OUT(SHWR_PEAK_AREA0));
   synchronizer_32bit shwr_peak_area1
     (.ASYNC_IN(LCL_SHWR_PEAK_AREA1[SHWR_BUF_RNUM]),
      .CLK(S_AXI_ACLK),.SYNC_OUT(SHWR_PEAK_AREA1));
   synchronizer_32bit shwr_peak_area2
     (.ASYNC_IN(LCL_SHWR_PEAK_AREA2[SHWR_BUF_RNUM]),
      .CLK(S_AXI_ACLK),.SYNC_OUT(SHWR_PEAK_AREA2));
   synchronizer_32bit shwr_peak_area3
     (.ASYNC_IN(LCL_SHWR_PEAK_AREA3[SHWR_BUF_RNUM]),
      .CLK(S_AXI_ACLK),.SYNC_OUT(SHWR_PEAK_AREA3));
   synchronizer_32bit shwr_peak_area4
     (.ASYNC_IN(LCL_SHWR_PEAK_AREA4[SHWR_BUF_RNUM]),
      .CLK(S_AXI_ACLK),.SYNC_OUT(SHWR_PEAK_AREA4));
   synchronizer_32bit shwr_peak_area5
     (.ASYNC_IN(LCL_SHWR_PEAK_AREA5[SHWR_BUF_RNUM]),
      .CLK(S_AXI_ACLK),.SYNC_OUT(SHWR_PEAK_AREA5));
   synchronizer_32bit shwr_peak_area6
     (.ASYNC_IN(LCL_SHWR_PEAK_AREA6[SHWR_BUF_RNUM]),
      .CLK(S_AXI_ACLK),.SYNC_OUT(SHWR_PEAK_AREA6));
   synchronizer_32bit shwr_peak_area7
     (.ASYNC_IN(LCL_SHWR_PEAK_AREA7[SHWR_BUF_RNUM]),
      .CLK(S_AXI_ACLK),.SYNC_OUT(SHWR_PEAK_AREA7));
   synchronizer_32bit shwr_peak_area8
     (.ASYNC_IN(LCL_SHWR_PEAK_AREA8[SHWR_BUF_RNUM]),
      .CLK(S_AXI_ACLK),.SYNC_OUT(SHWR_PEAK_AREA8));
   synchronizer_32bit shwr_peak_area9
     (.ASYNC_IN(LCL_SHWR_PEAK_AREA9[SHWR_BUF_RNUM]),
      .CLK(S_AXI_ACLK),.SYNC_OUT(SHWR_PEAK_AREA9));

   synchronizer_32bit shwr_baseline0
     (.ASYNC_IN(LCL_SHWR_BASELINE0[SHWR_BUF_RNUM]),
      .CLK(S_AXI_ACLK),.SYNC_OUT(SHWR_BASELINE0));
   synchronizer_32bit shwr_baseline1
     (.ASYNC_IN(LCL_SHWR_BASELINE1[SHWR_BUF_RNUM]),
      .CLK(S_AXI_ACLK),.SYNC_OUT(SHWR_BASELINE1));
   synchronizer_32bit shwr_baseline2
     (.ASYNC_IN(LCL_SHWR_BASELINE2[SHWR_BUF_RNUM]),
      .CLK(S_AXI_ACLK),.SYNC_OUT(SHWR_BASELINE2));
   synchronizer_32bit shwr_baseline3
     (.ASYNC_IN(LCL_SHWR_BASELINE3[SHWR_BUF_RNUM]),
      .CLK(S_AXI_ACLK),.SYNC_OUT(SHWR_BASELINE3));
   synchronizer_32bit shwr_baseline4
     (.ASYNC_IN(LCL_SHWR_BASELINE4[SHWR_BUF_RNUM]),
      .CLK(S_AXI_ACLK),.SYNC_OUT(SHWR_BASELINE4));

   // Test ADC outputs
   synchronizer_32bit adc0_test_sync
     (.ASYNC_IN({4'b0000,ADC0[2*`ADC_WIDTH-1:`ADC_WIDTH],
                 4'b0000,ADC0[`ADC_WIDTH-1:0]}),
      .CLK(S_AXI_ACLK),.SYNC_OUT(ADC0_TEST));
   synchronizer_32bit adc1_test_sync
     (.ASYNC_IN({4'b0000,ADC1[2*`ADC_WIDTH-1:`ADC_WIDTH],
                 4'b0000,ADC1[`ADC_WIDTH-1:0]}),
      .CLK(S_AXI_ACLK),.SYNC_OUT(ADC1_TEST));
   synchronizer_32bit adc2_test_sync
     (.ASYNC_IN({4'b0000,ADC2[2*`ADC_WIDTH-1:`ADC_WIDTH],
                 4'b0000,ADC2[`ADC_WIDTH-1:0]}),
      .CLK(S_AXI_ACLK),.SYNC_OUT(ADC2_TEST));
   synchronizer_32bit adc3_test_sync
     (.ASYNC_IN({4'b0000,ADC3[2*`ADC_WIDTH-1:`ADC_WIDTH],
                 4'b0000,ADC3[`ADC_WIDTH-1:0]}),
      .CLK(S_AXI_ACLK),.SYNC_OUT(ADC3_TEST));
   synchronizer_32bit adc4_test_sync
     (.ASYNC_IN({4'b0000,ADC4[2*`ADC_WIDTH-1:`ADC_WIDTH],
                 4'b0000,ADC4[`ADC_WIDTH-1:0]}),
      .CLK(S_AXI_ACLK),.SYNC_OUT(ADC4_TEST));

   // Test filtered ADC outputs
   synchronizer_32bit filt_pmt0_test_sync
     (.ASYNC_IN({2'b00,FILTD_PMT0,4'b0000,FILTB_PMT0}),
      .CLK(S_AXI_ACLK),.SYNC_OUT(FILT_PMT0_TEST));
   synchronizer_32bit filt_pmt1_test_sync
     (.ASYNC_IN({2'b00,FILTD_PMT1,4'b0000,FILTB_PMT1}),
      .CLK(S_AXI_ACLK),.SYNC_OUT(FILT_PMT1_TEST));
   synchronizer_32bit filt_pmt2_test_sync
     (.ASYNC_IN({2'b00,FILTD_PMT2,4'b0000,FILTB_PMT2}),
      .CLK(S_AXI_ACLK),.SYNC_OUT(FILT_PMT2_TEST));

   // These are mostly redundant, but may be useful for debugging
   // synchronizer_32bit shwr_buf_start0_sync
   //   (.ASYNC_IN(LCL_SHWR_BUF_START0),
   //    .CLK(S_AXI_ACLK),.SYNC_OUT(`SHWR_BUF_START0));
   // synchronizer_32bit shwr_buf_start1_sync
   //   (.ASYNC_IN(LCL_SHWR_BUF_START1),
   //    .CLK(S_AXI_ACLK),.SYNC_OUT(`SHWR_BUF_START1));
   // synchronizer_32bit shwr_buf_start2_sync
   //   (.ASYNC_IN(LCL_SHWR_BUF_START2),
   //    .CLK(S_AXI_ACLK),.SYNC_OUT(`SHWR_BUF_START2));
   // synchronizer_32bit shwr_buf_start3_sync
   //   (.ASYNC_IN(LCL_SHWR_BUF_START3),
   //    .CLK(S_AXI_ACLK),.SYNC_OUT(`SHWR_BUF_START3));

