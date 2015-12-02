// Simple round-robin arbiter

module round_robin
  (
  req,
  prev,
  gnt,
  any
  );

parameter WIDTH = 11;

input [WIDTH-1:0] req;
input [WIDTH-1:0] prev;
output [WIDTH-1:0] gnt;
output any;

wire [WIDTH-1:0] gnt0 = req & -req;

wire [WIDTH-1:0] req1 = req & ~((prev - 1'd1) | prev);

wire [WIDTH-1:0] gnt1 = req1 & -req1;

assign gnt = |req1 ? gnt1 : gnt0;

assign any = |req;

endmodule
