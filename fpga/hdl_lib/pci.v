// Minimal target only PCI interface: no bursts, no bytes, one base address.
// Generates and checks parity.

// Written by: Joe Allen

module pci
  (
  reset_l,		// PCI reset
  bus_startup,		// Startup step
  bus_clk,		// PCI clock (33 or 66 MHz: do not use PLL because
                        // it is legal to stop PCI clock)

  // PCI bus

  ad,
  be_l,
  irdy_l,		// No input FF on this signal! Needs 7ns setup time.
  frame_l,
  idsel,
  trdy_l,
  stop_l,
  parity,
  devsel_l,
  perr_l,
  serr_l,

  // Local bus

  bus_in,
  bus_out,

  // Local bus timeout interrupt

  timeout
  );

parameter ADDRWIDTH = 20;	// No. address bits to decode

parameter
  VENDOR_ID = 16'h1234,
  DEVICE_ID = 16'h5678,
  REVISION_ID = 8'h00;

parameter
  TIMEOUT = 255;

`include "bus_params.v"

// Who Am I

// Local address bus width
// Legal values are 31 to 4 inclusive.

// Width of base address register

parameter
  BAWIDTH = (32 - ADDRWIDTH);

input reset_l;		// PCI reset
input bus_startup;	// Startup step
input bus_clk;		// PCI clock (33 or 66 MHz: do not use PLL because
                        // it is legal to stop PCI clock)

// PCI bus

inout [31:0] ad;
input [3:0] be_l;
input irdy_l;		// No input FF on this signal!  Timing is critical.
input frame_l;
input idsel;
output trdy_l;
output stop_l;
inout parity;
output devsel_l;
output perr_l;
output serr_l;

// Inernal bus

input [BUS_OUT_WIDTH-1:0] bus_out;
output [BUS_IN_WIDTH-1:0] bus_in;

output timeout;
reg timeout;

wire [31:0] bus_rd_data = bus_out[BUS_DATA_END-1:BUS_DATA_START];	// Read data
wire bus_ack = bus_out[BUS_FIELD_ACK] || timeout;		// Acknowledgment

reg bus_req;			// Request
reg bus_rd_wr_l;		// Direction

reg [ADDRWIDTH-1:0] addr;	// Address
reg write_busy;
reg [31:0] write_data;		// Write data

// For multiplexed address/write-data bus
// wire [31:0] bus_wr_data = bus_req ? addr : write_data;

wire [31:0] bus_wr_data = write_data;

// Timeout

reg [7:0] cnt;

always @(posedge bus_clk or negedge reset_l)
  if(!reset_l)
    begin
      cnt <= 0;
      timeout <= 0;
    end
  else
    begin
      timeout <= 0;
      if(bus_out[BUS_FIELD_ACK])
        cnt <= 0;
      else if(bus_req)
        cnt <= TIMEOUT;
      else if(cnt)
        begin
          cnt <= cnt - 1;
          if(cnt==1)
            timeout <= 1;
        end
    end

// This must match bus_params.v
assign bus_in[BUS_DATA_END-1:BUS_DATA_START] = write_data;
assign bus_in[BUS_ADDR_END-1:BUS_ADDR_START] = { { (BUS_ADDR_WIDTH - ADDRWIDTH) { 1'b0 } }, addr };
assign bus_in[BUS_FIELD_REQ] = bus_req;
assign bus_in[BUS_FIELD_RD_WR_L] = bus_rd_wr_l;
assign bus_in[BUS_FIELD_CLK] = bus_clk;
assign bus_in[BUS_FIELD_RESET_L] = reset_l;
assign bus_in[BUS_FIELD_STARTUP] = bus_startup;

// Tri-state drivers and output registers:

// Data
reg [31:0] data_oe /* synthesis syn_preserve=1 xc_props="IOB=TRUE" */;	// output enable
reg data_oe_1 /* synthesis syn_preserve=1 */;
reg [31:0] ad_out_reg /* synthesis syn_preserve=1 xc_props="IOB=TRUE" */;
reg [31:0] p_out_reg /* synthesis syn_preserve=1 */;

assign ad[0] = data_oe[0] ? ad_out_reg[0] : 1'bz;
assign ad[1] = data_oe[1] ? ad_out_reg[1] : 1'bz;
assign ad[2] = data_oe[2] ? ad_out_reg[2] : 1'bz;
assign ad[3] = data_oe[3] ? ad_out_reg[3] : 1'bz;
assign ad[4] = data_oe[4] ? ad_out_reg[4] : 1'bz;
assign ad[5] = data_oe[5] ? ad_out_reg[5] : 1'bz;
assign ad[6] = data_oe[6] ? ad_out_reg[6] : 1'bz;
assign ad[7] = data_oe[7] ? ad_out_reg[7] : 1'bz;
assign ad[8] = data_oe[8] ? ad_out_reg[8] : 1'bz;
assign ad[9] = data_oe[9] ? ad_out_reg[9] : 1'bz;
assign ad[10] = data_oe[10] ? ad_out_reg[10] : 1'bz;
assign ad[11] = data_oe[11] ? ad_out_reg[11] : 1'bz;
assign ad[12] = data_oe[12] ? ad_out_reg[12] : 1'bz;
assign ad[13] = data_oe[13] ? ad_out_reg[13] : 1'bz;
assign ad[14] = data_oe[14] ? ad_out_reg[14] : 1'bz;
assign ad[15] = data_oe[15] ? ad_out_reg[15] : 1'bz;
assign ad[16] = data_oe[16] ? ad_out_reg[16] : 1'bz;
assign ad[17] = data_oe[17] ? ad_out_reg[17] : 1'bz;
assign ad[18] = data_oe[18] ? ad_out_reg[18] : 1'bz;
assign ad[19] = data_oe[19] ? ad_out_reg[19] : 1'bz;
assign ad[20] = data_oe[20] ? ad_out_reg[20] : 1'bz;
assign ad[21] = data_oe[21] ? ad_out_reg[21] : 1'bz;
assign ad[22] = data_oe[22] ? ad_out_reg[22] : 1'bz;
assign ad[23] = data_oe[23] ? ad_out_reg[23] : 1'bz;
assign ad[24] = data_oe[24] ? ad_out_reg[24] : 1'bz;
assign ad[25] = data_oe[25] ? ad_out_reg[25] : 1'bz;
assign ad[26] = data_oe[26] ? ad_out_reg[26] : 1'bz;
assign ad[27] = data_oe[27] ? ad_out_reg[27] : 1'bz;
assign ad[28] = data_oe[28] ? ad_out_reg[28] : 1'bz;
assign ad[29] = data_oe[29] ? ad_out_reg[29] : 1'bz;
assign ad[30] = data_oe[30] ? ad_out_reg[30] : 1'bz;
assign ad[31] = data_oe[31] ? ad_out_reg[31] : 1'bz;

// Parity
reg parity_oe /* synthesis syn_preserve=1 xc_props="IOB=TRUE" */;	// parity output enable
reg parity_reg /* synthesis syn_preserve=1 xc_props="IOB=TRUE" */;
assign parity = parity_oe ? parity_reg : 1'bz;

// Perr_l (for write data parity errors)
reg perr_l_oe /* synthesis syn_preserve=1 xc_props="IOB=TRUE" */;	// data parity error output enable
reg perr_l_reg /* synthesis syn_preserve=1 xc_props="IOB=TRUE" */;	// data parity error signal
assign perr_l = perr_l_oe ? perr_l_reg : 1'bz;

// Serr_l (for address parity errors)
reg serr_l_oe /* synthesis syn_preserve=1 xc_props="IOB=TRUE" */;	// address parity error output enable
assign serr_l = serr_l_oe ? 1'b0 : 1'bz;

// Control
reg [2:0] ctrl_oe /* synthesis syn_preserve=1 xc_props="IOB=TRUE" */;	// Control output enable

reg trdy_l_reg /* synthesis syn_preserve=1 xc_props="IOB=TRUE" */;
reg stop_l_reg /* synthesis syn_preserve=1 xc_props="IOB=TRUE" */;
reg devsel_l_reg /* synthesis syn_preserve=1 xc_props="IOB=TRUE" */;

assign trdy_l = ctrl_oe[2] ? trdy_l_reg : 1'bz;
assign stop_l = ctrl_oe[1] ? stop_l_reg : 1'bz;
assign devsel_l = ctrl_oe[0] ? devsel_l_reg : 1'bz;

// Input registers
reg frame_l_reg /* synthesis syn_maxfan=10000 xc_props="IOB=TRUE" */;
reg idsel_reg /* synthesis syn_maxfan=10000 xc_props="IOB=TRUE" */;
reg [31:0] ad_in_reg /* synthesis syn_maxfan=10000 xc_props="IOB=TRUE" */;
reg [3:0] be_l_reg /* synthesis syn_maxfan=10000 xc_props="IOB=TRUE" */;
reg parity_in_reg /* synthesis syn_maxfan=10000 xc_props="IOB=TRUE" */;

// Configuration space registers
reg conf_rd_req;
reg conf_wr_req;
wire conf_ack = (conf_rd_req || conf_wr_req);
reg [31:0] conf_rd_data;

// Configuration write
reg mem_en;			// Memory space enable (address 4, bit 1)
reg parity_response_en;		// Enable parity error response on serr_l and perr_l
reg serr_l_en;			// Enable serr_l
reg [BAWIDTH-1:0] ba0;		// Base address 0
reg [7:0] int_line;		// Interrupt line field

// Status regs

reg set_bit_serr;
reg set_parity_detected;

reg asserted_serr;
reg detected_parity_error;

always @(posedge bus_clk or negedge reset_l)
  if(!reset_l)
    begin
      mem_en <= 0;
      parity_response_en <= 0;
      serr_l_en <= 0;
      ba0 <= 0;
      asserted_serr <= 0;
      detected_parity_error <= 0;
      int_line <= 0;
    end
  else
    begin
      if(set_bit_serr)
        asserted_serr <= 1;
      if(set_parity_detected)
        detected_parity_error <= 1;
      if(conf_wr_req)
        case(addr[7:2])
          6'h1: // Command regsiter
            begin
              mem_en <= write_data[1];
              parity_response_en <= write_data[6];
              serr_l_en <= write_data[8];
              if(write_data[31])
                detected_parity_error <= 0;
              if(write_data[30])
                asserted_serr <= 0;
            end
          6'h4: // Base address register
            ba0 <= write_data[31:ADDRWIDTH];
          6'hf: // Interrupt line
            int_line <= write_data[7:0];
        endcase
    end

// Configuration read mux

always @(addr or mem_en or ba0 or detected_parity_error or asserted_serr or
         serr_l_en or parity_response_en or mem_en or int_line)
  case(addr[7:2])
    6'h0: // Who am I
      conf_rd_data = { DEVICE_ID[15:0], VENDOR_ID[15:0] };
    6'h1: // status and command registers
      conf_rd_data = { detected_parity_error, // Detected parity error
                       asserted_serr, // Signalled system error
                       1'd0, // Received master abort
                       1'd0, // Received target abort

                       1'd0, // Signalled target abort
                       2'd1, // DEVSEL timing
                       1'd0, // master data parity error

                       1'd0, // fast back-to-back capable
                       1'd0, // User defined featers
                       1'd0, // 66 MHz capable
                       1'd0, // PCI 2.2 capabilities list

                       4'd0, // reserved

                       6'd0, // reserved
                       1'd0, // fast back to back enable
                       serr_l_en, // serr_l enable

                       1'd0, // stepping control
                       parity_response_en, // parity error response
                       1'd0, // VGA snoop
                       1'd0, // memory write and invalidate enable

                       1'd0, // special cycles
                       1'd0, // bus master
                       mem_en, // memory space enable
                       1'd0 // io space enable
                       };
    6'h2: // { Class_code[23:0], Rev[7:0] }
      conf_rd_data = { 24'd0, REVISION_ID[7:0] };
    6'h3: // { Bist[7:0], header_type[7:0], lat_timer[7:0], cache[7:0] }
      conf_rd_data = 0;
    6'h4: // Base address 0
      conf_rd_data = { ba0, { ADDRWIDTH { 1'b0 } } };
    6'h5: // Base address 1
      conf_rd_data = 0;
    6'h6: // Base address 2
      conf_rd_data = 0;
    6'h7: // Base address 3
      conf_rd_data = 0;
    6'h8: // Base address 4
      conf_rd_data = 0;
    6'h9: // Base address 5
      conf_rd_data = 0;
    6'hA: // Cardbus CIS pointer
      conf_rd_data = 0;
    6'hB: // Subsystem_id[15:0], Subsystem_vendor_if[15:0]
      conf_rd_data = 0;
    6'hC: // Expansion ROM base address
      conf_rd_data = 0;
    6'hD: // reserved[23:0], capabilities_ptr[7:0]
      conf_rd_data = 0;
    6'hE: // reserved
      conf_rd_data = 0;
    6'hF: // max_lat[7:0], min_gnt[7:0], int_pin[7:0], int_line[7:0]
      conf_rd_data = { 8'd0, 8'd0, 8'd0, int_line };
    default:
      conf_rd_data = 0;
  endcase

// PCI State machine

reg [2:0] state;

parameter
  IDLE = 0,
  WRITE_WAIT_FOR_CORE = 1,
  WRITE_WAIT_FOR_BUS = 2,
  WRITE_START = 3,
  READ_WAIT_FOR_CORE = 4,
  READ_WAIT_FOR_ACK = 5,
  READ_WAIT_FOR_BUS = 6 ;

reg frame_l_old;		// For frame_l edge det.

reg [31:0] pci_addr;		// Latched pci address
reg [3:0] pci_c;		// Latched pci command

wire ack = (bus_ack || conf_ack);
                                // Memory or configuration complete pulse

reg keep_driving_perr;

reg par;	// Calculated parity
reg par_addr_check;	// Check calculated parity
reg par_data_check;

always @(posedge bus_clk or negedge reset_l)
  if(!reset_l)
    begin
      data_oe <= 0;
      data_oe_1 <= 0;
      ad_out_reg <= 0;
      p_out_reg <= 0;
      ad_in_reg <= 0;
      parity_in_reg <= 0;

      parity_oe <= 0;
      parity_reg <= 0;

      ctrl_oe <= 0;
      trdy_l_reg <= 0;
      stop_l_reg <= 0;
      devsel_l_reg <= 0;

      addr <= 0;
      write_data <= 0;
      bus_req <= 0;
      bus_rd_wr_l <= 0;
      conf_rd_req <= 0;
      conf_wr_req <= 0;

      idsel_reg <= 0;
      frame_l_reg <= 1;
      frame_l_old <= 1;
      be_l_reg <= 0;

      write_busy <= 0;

      pci_addr <= 0;
      pci_c <= 0;

      state <= IDLE;

      serr_l_oe <= 0;

      perr_l_oe <= 0;
      perr_l_reg <= 1;
      keep_driving_perr <= 0;

      set_parity_detected <= 0;
      set_bit_serr <= 0;

      par <= 0;
      par_addr_check <= 0;
      par_data_check <= 0;
    end
  else
    begin
      // Input registers
      ad_in_reg <= ad;
      frame_l_reg <= frame_l;
      frame_l_old <= frame_l_reg;
      idsel_reg <= idsel;
      be_l_reg <= be_l;
      parity_in_reg <= parity;

      // Commands
      bus_req <= 0;
      conf_wr_req <= 0;
      conf_rd_req <= 0;

      // Parity driver (once cycle latency)
      // We always drive parity one cycle after we drive ad[31:0].
      parity_oe <= data_oe_1;

      // Parity is weird: it covers be[3:0] even if we don't
      // drive it: so this is what we're supposed to do:
      // parity_reg <= ^ad_out_reg^^be_l; // PCI parity is even

      // But we only support whole word reads: thus all four be_ls
      // must be low all the time so we don't have to count them
      // for parity and simplify the timing.
      // parity_reg <= ^ad_out_reg;
      parity_reg <= ^p_out_reg;

      // Parity error driver
      perr_l_reg <= 1; // Normally no errors...

      if(keep_driving_perr)
        keep_driving_perr <= 0;
      else
        perr_l_oe <= 0;

      // Clear busy when core write is complete
      if(ack)
        write_busy <= 0;

      serr_l_oe <= 0;
      set_parity_detected <= 0;
      set_bit_serr <= 0;

      // Check for address parity errors
      if(par_addr_check)
        begin
          par_addr_check <= 0;
          if(par^parity_in_reg)
            begin
              set_parity_detected <= 1;
              if(parity_response_en && serr_l_en)
                begin
                  serr_l_oe <= 1;
                  set_bit_serr <= 1;
                end
            end
        end

      // Check for write data parity errors
      // FIXME: Perr is one cycle late here
      if(par_data_check)
        begin
          par_data_check <= 0;
          if(par^parity_in_reg)
            begin
              set_parity_detected <= 1;
              if(parity_response_en)
                perr_l_reg <= 0;
            end
          if(parity_response_en)
            begin
              keep_driving_perr <= 1;
              perr_l_oe <= 1;
            end
        end

      case(state)
        // Wait for start of transaction
        IDLE:
          begin
            ctrl_oe <= 0;
            if(!frame_l_reg && frame_l_old) // Transaction begins on falling edge of frame_l
              begin
                // Latch address and command
                pci_addr <= { 30'd0, ad_in_reg[ADDRWIDTH-1:2], 2'd0 };
                pci_c <= be_l_reg;

                // Calculate parity- check it next cycle.
                par <= ^ad_in_reg^^be_l_reg;
                par_addr_check <= 1;

                // We have the address in input reg
                // Ignore ad_in_reg[1:0]: this indicates burst order, but since we only
                // return a single word we don't care what it is.
                if(ad_in_reg[31:ADDRWIDTH]==ba0 && be_l_reg==4'b0110 && mem_en)
                  begin // Memory read
                    devsel_l_reg <= 0;
                    ctrl_oe <= 3'h7;
                    trdy_l_reg <= 1;
                    stop_l_reg <= 1;
                    if(write_busy && !ack)
                      begin
                        // Wait for previous write to complete
                        state <= READ_WAIT_FOR_CORE;
                      end
                    else
                      begin
                        // Start the read now
                        addr <= { 30'd0, ad_in_reg[ADDRWIDTH-1:2], 2'd0 };
                        bus_req <= 1;
                        bus_rd_wr_l <= 1;
                        state <= READ_WAIT_FOR_ACK;
                      end
                  end
                // Ignore ad_in_reg[1:0]: this indicates burst order, but since we only
                // return a single word we don't care what it is.
                else if(ad_in_reg[31:ADDRWIDTH]==ba0 && be_l_reg==4'b0111 && mem_en)
                  begin // Memory write
                    devsel_l_reg <= 0;
                    ctrl_oe <= 3'h7;
                    if(write_busy && !ack)
                      begin
                        // Wait for previous write to complete
                        // before asserting trdy_l.
                        trdy_l_reg <= 1;
                        stop_l_reg <= 1;
                        state <= WRITE_WAIT_FOR_CORE;
                      end
                    else
                      begin
                        // No pending write, so we're ready to go...
                        trdy_l_reg <= 0;
                        stop_l_reg <= 0;
                        state <= WRITE_WAIT_FOR_BUS;
                      end
                  end
                // ad_in_reg[0] and ad_in_reg[1] must both be zeros for type 0 configuration
                // access.
                else if(idsel_reg && be_l_reg==4'b1010 && !ad_in_reg[0] && !ad_in_reg[1])
                  begin // Config read
                    devsel_l_reg <= 0;
                    ctrl_oe <= 3'h7;
                    trdy_l_reg <= 1;
                    stop_l_reg <= 1;
                    if(write_busy && !ack)
                      begin
                        // Wait for previous write to complete
                        state <= READ_WAIT_FOR_CORE;
                      end
                    else
                      begin
                        // Start the read now
                        addr <= { 30'd0, ad_in_reg[ADDRWIDTH-1:2], 2'd0 };
                        conf_rd_req <= 1;
                        state <= READ_WAIT_FOR_ACK;
                      end
                  end
                // ad_in_reg[0] and ad_in_reg[1] must both be zeros for type 0 configuration
                // access.
                else if(idsel_reg && be_l_reg==4'b1011 && !ad_in_reg[0] && !ad_in_reg[1])
                  begin // Config write
                    devsel_l_reg <= 0;
                    ctrl_oe <= 3'h7;
                    if(write_busy && !ack)
                      begin
                        // Wait for previous write to complete
                        // before asserting trdy_l.
                        trdy_l_reg <= 1;
                        stop_l_reg <= 1;
                        state <= WRITE_WAIT_FOR_CORE;
                      end
                    else
                      begin
                        // No pending write, so we're ready to go...
                        trdy_l_reg <= 0;
                        stop_l_reg <= 0;
                        state <= WRITE_WAIT_FOR_BUS;
                      end
                  end
              end
          end

        // Wait for previous write to complete
        WRITE_WAIT_FOR_CORE:
          if(ack)
            begin
              trdy_l_reg <= 0;
              stop_l_reg <= 0;
              state <= WRITE_WAIT_FOR_BUS;
            end

        // We have asserted trdy_l
        // Wait for irdy_l from bus
        WRITE_WAIT_FOR_BUS:
          if(!irdy_l) // Non-registered signal.
            begin
              // He can not start a new frame_l until devsel_l is high.
              devsel_l_reg <= 1;
              stop_l_reg <= 1;
              trdy_l_reg <= 1;
              state <= WRITE_START;
            end

        // Write has been initiated, precharge bus
        // Next trans can't start here: write data is in our
        // ad_in_reg.
        WRITE_START:
          begin
            addr <= pci_addr;
            write_data <= ad_in_reg;
            if(pci_c[3])
              conf_wr_req <= 1;
            else
              begin
                bus_req <= 1;
                bus_rd_wr_l <= 0;
              end
            write_busy <= 1;
            state <= IDLE;
            // Detect write data parity errors (signal them next cycle)
            par <= ^ad_in_reg^^be_l_reg;
            par_data_check <= 1;
          end

        // Wait for previous write to complete
        READ_WAIT_FOR_CORE:
          if(ack)
            begin
              addr <= pci_addr;
              if(pci_c[3])
                conf_rd_req <= 1;
              else
                begin
                  bus_req <= 1;
                  bus_rd_wr_l <= 1;
                end
              state <= READ_WAIT_FOR_ACK;
            end

        // Wait for read from core to complete
        READ_WAIT_FOR_ACK:
          if(ack)
            begin
              trdy_l_reg <= 0;
              stop_l_reg <= 0;
              if(pci_c[3])
                begin
                  ad_out_reg <= conf_rd_data;
                  p_out_reg <= conf_rd_data;
                end
              else
                begin
                  ad_out_reg <= bus_rd_data;
                  p_out_reg <= bus_rd_data;
                end
              data_oe <= 32'hffffffff;
              data_oe_1 <= 1;
              state <= READ_WAIT_FOR_BUS;
            end

        // Wait for simultaneous irdy_l trdy_l
        READ_WAIT_FOR_BUS:
          if(!irdy_l)
            begin
              devsel_l_reg <= 1;
              stop_l_reg <= 1;
              trdy_l_reg <= 1;
              data_oe <= 0;
              data_oe_1 <= 0;
              state <= IDLE;
            end
      endcase
    end

endmodule
