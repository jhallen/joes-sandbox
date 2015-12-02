module double_register
  #(
   parameter WIDTH_MSB = 0
   )
(
input wire [WIDTH_MSB:0] data_in,
output reg [WIDTH_MSB:0] data_out,

input wire clock,
input wire reset_l

);

reg [WIDTH_MSB:0] data_pipe1;


always @(posedge clock or negedge reset_l)
  if (!reset_l) begin
    data_pipe1 <= 'h0;
    data_out <= 'h0;
  end
  else begin
    data_pipe1 <= data_in;
    data_out   <= data_pipe1;

  end
    
endmodule

