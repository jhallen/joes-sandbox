module example
  (
  );

wire [1:0] a;
wire b;
wire c;

input_reg #(.ADDR(20'h12345)) status_reg
  (
  .in ({ a, b, c })
  );

wire [11:0] x;
wire y_l;
wire z;

input_reg #(.ADDR(16'h0101)) stat_reg
  (
  .in ({ x, !y_l, !z })
  );

wire i;
wire j;
wire k;

output_reg #(.ADDR(16'h55AA), .INV(3'b101)) ctrl_reg
  (
  .out ({ i, j, k })
  );

endmodule
