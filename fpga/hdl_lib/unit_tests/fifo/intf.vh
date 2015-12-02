interface bus;
  wire clk;
  wire reset_l;
  wire we;
  wire re;
  wire [3:0] wr_data;
  wire [3:0] addr;
  wire [3:0] rd_data;
  modport source (output clk, reset_l, we, re, wr_data, addr, input rd_data);
  modport sink (input clk, reset_l, we, re, wr_data, addr, output rd_data);
endinterface
