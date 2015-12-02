// These parameters define the internal bus

parameter BUS_DATA_WIDTH = 64;
parameter BUS_ADDR_WIDTH = 16;

parameter BUS_DATA_START = 0;
parameter BUS_DATA_END = BUS_DATA_START + BUS_DATA_WIDTH;

parameter BUS_ADDR_START = BUS_DATA_END;
parameter BUS_ADDR_END = BUS_ADDR_START + BUS_ADDR_WIDTH;

parameter BUS_IN_CTRL_START = BUS_ADDR_END;
parameter BUS_IN_CTRL_END = BUS_IN_CTRL_START + 5;

// No. input bits

parameter BUS_IN_WIDTH = (BUS_IN_CTRL_END);

// Input bit fields

// BUS_DATA_WIDTH-1:0 is write data
// BUS_DATA_WIDTH+BUS_ADDR_WIDTH-1:BUS_DATA_WIDTH is address

parameter BUS_FIELD_REQ = BUS_IN_CTRL_START + 0;
parameter BUS_FIELD_RD_WR_L = BUS_IN_CTRL_START + 1;
parameter BUS_FIELD_CLK = BUS_IN_CTRL_START + 2;
parameter BUS_FIELD_RESET_L = BUS_IN_CTRL_START + 3;
parameter BUS_FIELD_STARTUP = BUS_IN_CTRL_START + 4;

// No. output bits

parameter BUS_OUT_CTRL_START = BUS_DATA_END;
parameter BUS_OUT_CTRL_END = BUS_OUT_CTRL_START + 2;

parameter BUS_OUT_WIDTH = (BUS_OUT_CTRL_END);

// Output bit fields

// 31:0 is read data

parameter BUS_FIELD_ACK = BUS_OUT_CTRL_START + 0;
parameter BUS_FIELD_IRQ = BUS_OUT_CTRL_START + 1;
