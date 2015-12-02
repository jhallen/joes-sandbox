// Inject a packet

module inject
  (
  clk,
  reset_l,

  wr_addr,		// Load inject RAM
  wr_data,
  we,

  trig,			// Transmit on rising edge

  inject_data_out,	// injects from address 0 to wr_addr inclusive
  inject_sop,
  inject_eop,
  inject_valid
  );

parameter ADDRWIDTH=5;
parameter DATAWIDTH=16;

input clk;
input reset_l;

input [ADDRWIDTH-1:0] wr_addr;
input [DATAWIDTH-1:0] wr_data;
input we;

input trig;

output [DATAWIDTH-1:0] inject_data_out;
output inject_sop;
reg inject_sop;
output inject_eop;
reg inject_eop;
output inject_valid;
reg inject_valid;

reg [ADDRWIDTH-1:0] ram_rd_addr;

wire [DATAWIDTH-1:0] ram_rd_data;
assign inject_data_out = inject_valid ? ram_rd_data : { DATAWIDTH { 1'd0 } };

ram_dp #(.DATAWIDTH(DATAWIDTH), .ADDRWIDTH(ADDRWIDTH)) inject_ram
  (
  .clk (clk),
  .wr_addr (wr_addr),
  .wr_data (wr_data),
  .we (we),
  .rd_addr (ram_rd_addr),
  .rd_data (ram_rd_data)
  );

reg trig_old;
reg running;
reg inject_sop_1;
reg inject_eop_1;
reg inject_valid_1;

always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      ram_rd_addr <= 0;
      trig_old <= 0;
      running <= 0;
      inject_sop <= 0;
      inject_sop_1 <= 0;
      inject_eop <= 0;
      inject_eop_1 <= 0;
      inject_valid <= 0;
      inject_valid_1 <= 0;
    end
  else
    begin
      trig_old <= trig;
      inject_sop <= inject_sop_1;
      inject_eop <= inject_eop_1;
      inject_valid <= inject_valid_1;
      inject_sop_1 <= 0;
      inject_eop_1 <= 0;
      inject_valid_1 <= 0;
      if (running)
        begin
          inject_valid_1 <= 1;
          ram_rd_addr <= ram_rd_addr + 1'd1;
          if (ram_rd_addr + 2'd2 == wr_addr)
            begin
              running <= 0;
              inject_eop_1 <= 1;
            end
        end
      else if (trig && !trig_old)
        begin
          ram_rd_addr <= 0;
          inject_sop_1 <= 1;
          inject_valid_1 <= 1;
          running <= 1;
          if (wr_addr == 1)
            begin
              running <= 0;
              inject_eop_1 <= 1;
            end
        end
    end

endmodule
