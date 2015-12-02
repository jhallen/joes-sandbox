// Capture a packet

module capture
  (
  reset_l,
  clk,

  sample_data_in,	// Data
  sample_sot_in,	// Start of packet
  sample_eot_in,	// End of packet

  trig,			// Arm on rising edge.  Cancel on falling edge.
  armed,		// High if armed.  Cleared after capture.
  ram_wr_addr,		// Last address written to + 1

  rd_addr,
  rd_data
  );

parameter DATAWIDTH=16;
parameter ADDRWIDTH=5;

input reset_l;
input clk;
input [DATAWIDTH-1:0] sample_data_in;
input sample_sot_in;
input sample_eot_in;

input trig;

output armed;
reg armed;

input [ADDRWIDTH-1:0] rd_addr;

output [DATAWIDTH-1:0] rd_data;

output [ADDRWIDTH-1:0] ram_wr_addr;
reg [ADDRWIDTH-1:0] ram_wr_addr;

reg running;

wire first = (armed && sample_sot_in);

ram_dp #(.DATAWIDTH(DATAWIDTH), .ADDRWIDTH(ADDRWIDTH)) capture_ram
  (
  .clk (clk),
  .wr_addr (first ? { ADDRWIDTH { 1'd0 } } : ram_wr_addr),
  .wr_data (sample_data_in),
  .we (first || running),
  .rd_addr (rd_addr),
  .rd_data (rd_data)
  );

reg trig_old;

always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      armed <= 0;
      running <= 0;
      trig_old <= 0;
      ram_wr_addr <= 0;
    end
  else
    begin
      trig_old <= trig;
      if (trig && !trig_old)
        armed <= 1;
      if (!trig && trig_old)
        armed <= 0;
      if (running)
        begin
          ram_wr_addr <= ram_wr_addr + 1'd1;
          if (sample_eot_in || ram_wr_addr == { ADDRWIDTH { 1'd1 }})
            begin
              running <= 0;
              armed <= 0;
            end
        end
      else if (first)
        begin
          running <= 1;
          ram_wr_addr <= 1;
          if (sample_eot_in)
            begin
              running <= 0;
              armed <= 0;
            end
        end
    end

endmodule
