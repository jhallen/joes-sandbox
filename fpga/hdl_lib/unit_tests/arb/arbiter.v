// Efficient round-robin arbiter - Joe Allen

// Properties:

//   Clients get to own the resource for one or more cycles per
//   transaction.

//   If used carefully, there are no dead cycles between transactions.  This
//   is true for both successive transactions between different clients and
//   for successive transactions from the same client.

//   Clients communicate with the arbiter using only registered
//   outputs- combinatorial outputs are not needed.

//   There is a one flop delay from req[] to gnt[].

// How to use:

//   A client should assert a bit in req[] to acquire a resource.

//   It should then wait for gnt[].  The client owns the resource starting
//   on the cycle where both req[] and gnt[] are high.

//   Typically, the first datum of the transaction is given along with req[]
//   so that when gnt[] is asserted, the datum is taken that cycle.

//   The client relinquishes the resource by asserting last[].  The cycle
//   with both last[] and gnt[] asserted will be the last cycle of the
//   transaction.

//   Once req[] and gnt[] are asserted for a client, gnt[] is guaranteed to
//   remain asserted until the cycle following last[].  In other words, once
//   you've won it, the resource is yours until you assert last[].

//   If the client only wants the resource for a single cycle transaction, it
//   should assert last[] immediately along with req[].  The single cycle
//   transaction will occur with req[], gnt[] and last[] all asserted
//   at the same time.

//   If all clients only ever do single cycle transactions, it is ok to tie
//   last[] high.

//   For maximum efficiency (no dead cycles between successive transactions
//   from the same client), Req[] should be kept asserted on all cycles of
//   the transaction, including the one where last[] is asserted.

//   This allows the arbiter to speculatively assert gnt[] on the cycle
//   following last[].  If req[] is still asserted on that cycle (so that
//   req[] and gnt[] are both asserted), then the client is granted another
//   transaction, and cycle with both req[] and gnt[] is the first cycle. If
//   req[] is not asserted during the speculative gnt[], gnt[] will be
//   released one cycle later.

//   Note that if you de-assert req[] when gnt[] is asserted, you will still
//   get the speculative gnt[] after single-cycle transactions (but will not
//   get it after the end of a multi-cycle transaction).  Why?  Single cycle
//   transactions follow the rule: req[] is asserted on all cycles
//   of the transaction.

//   gnt[] (a registerered output) should be used to control the
//   combinatorial MUX feeding the resource.  An OR-tree can also be used:
//   in that case the client must output zero on any cycle it does not own.
//   Remember gnt[] may be asserted for one additional cycle beyond the last
//   transaction, so signals need to be kept valid for this cycle.

//   'done' should be the expression: '|(gnt & last)'

//   Remember: if req is asserted on the cycle after done, the client will
//   win again- so be sure to de-assert req no later than one cycle after
//   done.  Or, equivalently, assert done no earlier than one cycle before
//   you de-assert req.

// new_gnt is pulsed (for one clock) one cycle before gnt[] is asserted for
// a new winner.  This pulse occurs even if the new winner is the same as
// the old winner.  This pulse also occurs one cycle before speculative
// grants, so new_gnt can not be reliably used to delimit transactions.

// 'first' is asserted on the first cycle of each transaction.  It can be
// reliably used to delimited transactions.

// A test-bench showing how client should be written is given at the end of this
// file.

module arbiter
  (
  req,		// High to request a resource (set low along with done high or earlier)
  nxt_gnt,	// Hign means client owns resource next cycle
  maybe_nxt_gnt,	// Same as nxt_gnt, but does not depend on 'hazard'
  new_gnt,	// High along with change in new_gnt
  gnt,		// High means client owns resource this cycle
  last,		// Client sets high along with last cycle to relinquish resource
  hazard,	// Delay next grant
  first,	// High on first cycle of any transaction

  vclk,
  reset_l
  );

parameter WIDTH = 11;

// Client interfaces

input [WIDTH-1:0] req;
output [WIDTH-1:0] maybe_nxt_gnt;
reg [WIDTH-1:0] maybe_nxt_gnt;
output [WIDTH-1:0] nxt_gnt;
output new_gnt;
reg new_gnt;
reg new_gnt_r;
output [WIDTH-1:0] gnt;
input [WIDTH-1:0] last;
input hazard;
output first;
reg first;
input vclk;
input reset_l;

reg [WIDTH-1:0] nxt_prev_winner, prev_winner;

wire [WIDTH-1:0] nxt_pending;
reg [WIDTH-1:0] pending;	// The next winner

wire nxt_any_pending;
reg any_pending;		// Any winner?

reg granted, nxt_granted;

reg [WIDTH-1:0] gnt, nxt_gnt;

wire [WIDTH-1:0] gated_last = (last & gnt); // Last only counts when client selected
wire done = |gated_last;

// Combinatorial one-hot arbiter

round_robin #(.WIDTH(WIDTH)) arbiter
  (
  // Inputs
  .req (req),
  .prev (prev_winner),

  // Outputs
  .gnt (nxt_pending),
  .any (nxt_any_pending)
  );

// State machine

reg speculative, nxt_speculative;

always @(posedge vclk or negedge reset_l)
  if (!reset_l)
    begin
      granted <= 0;
      gnt <= 0;
      prev_winner <= 1;
      pending <= 0;
      any_pending <= 0;
      speculative <= 0;
      new_gnt_r <= 0;
    end
  else
    begin
      granted <= nxt_granted;
      gnt <= nxt_gnt;
      pending <= nxt_pending;
      any_pending <= nxt_any_pending;
      prev_winner <= nxt_prev_winner;
      speculative <= nxt_speculative;
      new_gnt_r <= new_gnt;
    end

always @(*)
  begin
    nxt_speculative = 0;
    nxt_granted = granted;
    nxt_prev_winner = prev_winner;
    nxt_gnt = gnt;
    maybe_nxt_gnt = gnt;
    new_gnt = 0;

    // Asserted on first cycle of any real transaction
    first = new_gnt_r && (!speculative || (|(gnt & req)));

    // Re-arbitrate if:
    //   Done is asserted: last cycle of transaction.
    //   Granted is not asserted: we're not in a transaction.
    //   Speculative is asserted and 'req' which caused it is not asserted.
    if (done || !granted || speculative && !(|(gnt & req)))
      begin
        nxt_granted = 0;
        nxt_gnt = 0;
        maybe_nxt_gnt = 0;
        if (nxt_any_pending)
          maybe_nxt_gnt = nxt_pending;
        if (nxt_any_pending && !hazard)
          begin
            nxt_gnt = nxt_pending;
            nxt_prev_winner = nxt_pending;
            nxt_granted = 1;
            if (gnt & nxt_pending)
              // Grant is speculative if it's from our own request left over from last transaction.
              nxt_speculative = 1;
            new_gnt = 1;
          end
      end

  end

// granted && !speculative: we're in the middle of a transaction
//  granted && speculative: we're in the middle of a transaction, but only if (req & gnt) is non-zero.

endmodule
