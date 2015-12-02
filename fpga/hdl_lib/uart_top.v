// Complete UART

module uart_top
  (
  bus_in,
  bus_out,

  // Serial

  uart_rx,
  uart_tx
  );

parameter ADDR = 0;
parameter IRQ_ADDR = 0;

`include "bus_params.v"

input [BUS_IN_WIDTH-1:0] bus_in;
output [BUS_OUT_WIDTH-1:0] bus_out;

input uart_rx;
output uart_tx;

`include "bus_decl.v"

wire diag_loop;
wire line_loop;

// The actual UART

wire [11:0] baud_rate;
wire [7:0] tx_fifo_rd_data;
wire tx_fifo_re;
wire tx_fifo_ne;

wire [7:0] rx_fifo_wr_data;
wire rx_fifo_we;

wire uart_frame_error;
wire rx_en;

wire uart_tx_internal;

assign uart_tx = line_loop ? uart_rx : uart_tx_internal;

uart raw_uart
  (
  .reset_l (reset_l),
  .clk (bus_clk),

  // Baud rate divisor
  .baud_rate (baud_rate),

  // Tx side
  .tx_fifo_rd_data (tx_fifo_rd_data),
  .tx_fifo_re (tx_fifo_re),
  .tx_fifo_ne (tx_fifo_ne),

  .uart_tx (uart_tx_internal),

  // Rx side
  .rx_fifo_wr_data (rx_fifo_wr_data),
  .rx_fifo_we (rx_fifo_we),
  .uart_frame_error (uart_frame_error),

  .rx_en (rx_en),
  .uart_rx (diag_loop ? uart_tx_internal : uart_rx)
  );

// Tx FIFO

wire tx_fifo_full;
wire tx_fifo_ovf;
wire tx_fifo_we;
wire [7:0] tx_fifo_wr_data;

fifo_sync_late #(.DATAWIDTH(8)) tx_fifo
  (
  .reset_l (reset_l),
  .clk (bus_clk),
  .we (tx_fifo_we),
  .wr_data (tx_fifo_wr_data),
  .full (tx_fifo_full),
  .ovf (tx_fifo_ovf),

  .re (tx_fifo_re),
  .rd_data (tx_fifo_rd_data),
  .ne (tx_fifo_ne)
  );

// Rx FIFO

wire rx_fifo_full;
wire rx_fifo_ovf;
wire rx_fifo_re;
wire [7:0] rx_fifo_rd_data_1;
wire rx_fifo_ne;

fifo_sync_late #(.DATAWIDTH(8)) rx_fifo
  (
  .reset_l (reset_l),
  .clk (bus_clk),
  .we (rx_fifo_we),
  .wr_data (rx_fifo_wr_data),
  .full (rx_fifo_full),
  .ovf (rx_fifo_ovf),

  .re (rx_fifo_re),
  .rd_data (rx_fifo_rd_data_1),
  .ne (rx_fifo_ne)
  );

// Interrupt sources

reg tx_fifo_ne_old;

always @(posedge bus_clk or negedge reset_l)
  if(!reset_l)
    tx_fifo_ne_old <= 0;
  else
    tx_fifo_ne_old <= tx_fifo_ne;

wire [BUS_OUT_WIDTH-1:0] irq_sources_out;
assign bus_out = irq_sources_out;
bus_irq_pair #(.ADDR(IRQ_ADDR), .OFFSET(0), .DATAWIDTH(5)) irq_sources
  (
  .bus_in (bus_in), .bus_out (irq_sources_out),

  .clk (bus_clk),
  .trig ({ rx_fifo_ne, uart_frame_error, tx_fifo_ovf, rx_fifo_ovf, (!tx_fifo_ne && tx_fifo_ne_old) })
  );

// Programmable registers

wire [2:0] dummy1;
wire [13:0] dummy2;

wire [BUS_OUT_WIDTH-1:0] uart_ctrl_out;
assign bus_out = uart_ctrl_out;
bus_reg #(.ADDR(ADDR), .IZ(589)) uart_ctrl
  (
  .bus_in (bus_in), .bus_out (uart_ctrl_out),

  .in ({ tx_fifo_ne, tx_fifo_full, rx_fifo_ne, rx_en, diag_loop, line_loop, 14'd0, baud_rate }),
  .out ({ dummy1, rx_en, diag_loop, line_loop, dummy2, baud_rate })
  );

wire [7:0] rx_fifo_rd_data = rx_fifo_ne ? rx_fifo_rd_data_1 : 8'h00;

wire [BUS_OUT_WIDTH-1:0] uart_data_out;
assign bus_out = uart_data_out;
bus_reg #(.ADDR(ADDR+4), .DATAWIDTH(8)) uart_data
  (
  .bus_in (bus_in), .bus_out (uart_data_out),

  .in (rx_fifo_rd_data),
  .out (tx_fifo_wr_data),
  .read (rx_fifo_re),
  .write (tx_fifo_we)
  );

endmodule
