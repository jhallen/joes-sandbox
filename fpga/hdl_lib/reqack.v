// Make request/ack sequence look like a FIFO access

module reqack
  (
  input reset_l,
  input clk,
  input req,		// Flip for request
  output reg ack,	// Flip for ack
  output reg ne,	// (not empty) A request is pending
  input re		// (read enable) Pulse to acknowledge request
  );

wire req_synced;

sync req_syncer
  (
  .reset_l (reset_l),
  .clk (clk),
  .i (req),
  .o (req_synced)
  );

always @(posedge clk or negedge reset_l)
  if(!reset_l)
    begin
      ack <= 0;
      ne <= 0;
    end
  else
    begin
      ne <= req_synced != ack;
      if(re) ack <= !ack;
    end

endmodule
