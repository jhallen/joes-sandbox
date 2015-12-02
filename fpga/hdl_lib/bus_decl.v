// Declare the internal bus structure //

wire [BUS_IN_WIDTH-1:0] bus_in;
wor [BUS_OUT_WIDTH-1:0] bus_out;

// Bus input fields

wire bus_startup = bus_in[BUS_FIELD_STARTUP];	// Startup step
wire bus_reset_l = bus_in[BUS_FIELD_RESET_L];	// Reset
wire bus_clk = bus_in[BUS_FIELD_CLK];		// Clock
wire bus_req = bus_in[BUS_FIELD_REQ];
wire bus_rd_wr_l = bus_in[BUS_FIELD_RD_WR_L];
wire [BUS_DATA_WIDTH-1:0] bus_wr_data = bus_in[BUS_DATA_END-1:BUS_DATA_START];	// Write data
wire [BUS_ADDR_WIDTH-1:0] bus_addr = bus_in[BUS_ADDR_END-1:BUS_ADDR_START];	// Address

// Bus output fields

wire bus_irq = bus_out[BUS_FIELD_IRQ];		// Interrupt request
wire bus_ack = bus_out[BUS_FIELD_ACK];		// Acknowledge
wire [BUS_DATA_WIDTH-1:0] bus_rd_data = bus_out[BUS_DATA_END-1:BUS_DATA_START];
                                                // Read data
assign bus_out = 0;	// In case nobody is driving.
