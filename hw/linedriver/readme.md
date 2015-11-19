# Line Driver

This is a simple line driver for a 1200 BAUD multi-drop bus.  It converts
RS-232 into long haul twisted pair.  It uses brute force high transmit power
to acheive greater distance than the otherwise very similar RS-485.

It was used for a network of timeclocks spanning two manufacturing
locations: one 5 km long bus.  In this system, there is one master, an IBM
PC, and 6 recording timeclock slaves.  Two pairs were used: one for the
master to broadcast to all of the slaves and a second for all of the slaves
to respond back to the master.

	* Transmitter is normally off, but turns on for the duration of each
	  transmitted character.  This allows the bus to be multi-drop
	  without any additional control signals.

	* Transmission line inputs and outputs protected by transorbs or
	  zenor diodes.

	* Uses only readily available components (all but the transorbs were
	  available from Radio Shack).

	* High power transmitter and sensitive receiver

		* Bus should be terminated at each end with ~150 ohms.  This
		  presents a 75 ohm load to the transmitter.

		* Transmit current for this case is about 60mA, for ~4.5 V
		  pk-pk signal.

		* Receive sensitivity is probably ~.1 V pk-pk. (in theory
		  the LM393 is better than this, but the .1 V allows for
		  some noise).

		* This provides a link budget of 33 dB.
