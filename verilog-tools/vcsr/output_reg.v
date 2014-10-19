module output_reg
  (
  out
  );

output [15:0] out;

parameter REG = 0; // Mark this module as a register
parameter ADDR = 0; // Address of register
parameter INV = 0; // Bit to invert

endmodule
