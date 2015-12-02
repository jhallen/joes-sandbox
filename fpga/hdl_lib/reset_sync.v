// Reset generator

module reset_sync
  (
  reset_req_l,
  clk,
  reset_l
  );

input reset_req_l; // Async reset request
input clk;
output reset_l; // Async assert, sync de-assert reset out
reg reset_l;

reg [7:0] debounce_counter;
wire debounce_done = debounce_counter[7];
reg reset_l_1;
reg reset_l_2;

always @(posedge clk or negedge reset_req_l)
  if (!reset_req_l)
    begin
      debounce_counter <= 0;
      reset_l_1 <= 0;
      reset_l_2 <= 0;
      reset_l <= 0;
    end
  else
    begin
      reset_l <= reset_l_1;
      reset_l_1 <= reset_l_2;
      reset_l_2 <= debounce_done;
      if (!debounce_done)
        debounce_counter <= debounce_counter + 1'd1;
    end

endmodule
