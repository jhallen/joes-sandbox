module pad_iobuf_reg
  (
  reset_l,
  clk,
  pin,
  i,
  o,
  oe
  );

parameter TYPE = "3.0-V LVTTL";
parameter IZ = 1'd0;

input reset_l;
input clk;
inout pin;
output i;
input o;
input oe;

wire i_r;
reg oe_r /* synthesis altera_attribute = "-name fast_output_enable_register on" */;
reg i /* synthesis altera_attribute = "-name fast_input_register on" */;
reg o_r /* synthesis altera_attribute = "-name fast_output_register on" */;

pad_iobuf #(.TYPE (TYPE)) iobuf
  (
  .pin (pin),
  .oe (oe_r),
  .i (i_r),
  .o (o_r)
  );

always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      oe_r <= 0;
      i <= 0;
      o_r <= IZ;
    end
  else
    begin
      oe_r <= oe;
      o_r <= o;
      i <= i_r;
    end

endmodule
