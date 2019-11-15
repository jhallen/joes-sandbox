# Dealing with Xilinx Vivado

Here is one way to structure your FPGA project so that it is compatible with
both Xilinx Vivado GUI in project mode and version control.

The directory structure should be this:

    example_fpga/
        rebuild.tcl   <- TCL script created by Vivado.  Checked in.
        rtl/          <- Your Verilog (or VHDL) source code.  All checked in.
        cons/         <- Your constraint files.  All checked in.
        project_1/    <- Vivado project.  Nothing here is checked in.
        ip/           <- Xilinx IP.  Only the .xci files are checked in.

The idea is to have the minimal set of files checked in so that you get the
Vivado project back after a fresh clone.  You should only have to type these
commands:

    git clone https://github.com/jhallen/example_fpga.git
    cd example_fpga
    vivado -source rebuild.tcl &

You can always delete project_1/ and rebuild it:

    rm -rf project_1
    vivado -source rebuild.tcl &

Unfortunately, Vivado really would like your source code to be in the
project directory, so you have to actively fight it to prevent this.

# Initial project build

Suppose you don't have the project_1/ and rebuild.tcl script.  This is how
to create them.

Start vivado.  Change directory to example_fpga first. [There are other
possibilities here, but this will get you started.]

    cd example_fpga
    vivado &

Go through the usual sequence of creating a project, but not check any of
the boxes that say "copy into project":

![image](create_1.png)