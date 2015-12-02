// Transfer data from one clock domain to another
// no FIFO is used.. this is useful if you just need to sample recent
// versions of the input data.

module sample
  (
  i_reset_l,
  i_clk,
  i,

  o_clk,
  o_reset_l,
  o_startup,
  o
  );

parameter DATAWIDTH=8;

input i_reset_l;
input i_clk;

input [DATAWIDTH-1:0] i;

input o_clk;
input o_reset_l;
input o_startup;

output [DATAWIDTH-1:0] o;
reg [DATAWIDTH-1:0] o;

reg [DATAWIDTH-1:0] xfer;

reg xfer_req;
wire xfer_req_synced;

edge_det req_edge_det
  (
  .reset_l		(i_reset_l),
  .clk			(i_clk),
  .i			(xfer_req),
  .o			(xfer_req_synced)
  );

reg xfer_done;

always @(posedge i_clk or negedge i_reset_l)
  if(!i_reset_l)
    begin
      xfer_done <= 0;
      xfer <= 0;
    end
  else
    begin
      if(xfer_req_synced)
        begin
          xfer <= i;
          xfer_done <= ~xfer_done;
        end
    end

wire xfer_done_synced;

edge_det done_edge_det
  (
  .reset_l		(o_reset_l),
  .clk			(o_clk),
  .i			(xfer_done),
  .o			(xfer_done_synced)
  );

reg o_startup_reg;

always @(posedge o_clk or negedge o_reset_l)
  if(!o_reset_l)
    begin
      xfer_req <= 0;
      o <= 0;
      o_startup_reg <= 0;
    end
  else
    begin
      o_startup_reg <= o_startup;
      if(xfer_done_synced || (o_startup && !o_startup_reg))
        begin
          o <= xfer;
          xfer_req <= !xfer_req;
        end
    end

endmodule
