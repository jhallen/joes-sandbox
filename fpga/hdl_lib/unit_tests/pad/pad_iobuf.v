module pad_iobuf
  (
  pin,
  i,
  o,
  oe
  );

parameter TYPE = "3.0-V LVTTL";

inout pin;
output i;
input o;
input oe;

alt_iobuf #(.io_standard(TYPE)) iobuf
  (
  .io (pin),
  .i (o),
  .o (i),
  .oe (oe)
  );

endmodule
