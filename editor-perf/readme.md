# Text Editor Performance Comparison

This started out as a check on some performance problems which were fixed
for the latest version of JOE (version 4.3), but is interesting in its own
right.

## The system

Lenovo G570 laptop

Ubuntu 14.04 LTS 64-bit

Swap = 6 MB

Hitachi HTS545050B9A300 500 GB drive 5400 RPM, 8 MB cache, SATA 3.0 Gb/s

Memory 4 GB (2x 2 GB 1333 MHz DDR3 SODIMMs)

Intel Pentium B970 2.3 GHz Two cores, 64-bit, 2 MB L3 cache

## The Editors

* Joe 4.3
* VIM 7.4.52
* gnu-emacs 24.3.1
* Nano 2.2.6
* Mg mg_20110905-1.1_amd64
* Atom 1.9.6

## The files

* hello.c Tiny "hello, world!" program
* longlines.txt Two 120KB lines
* test.xml 5.8 MB XML file
* huge 3 GB file (3M 1K lines)

## Memory used when loading "hello.c" with syntax highlighting enabled

|Editor		|RSS|
|---------------|---|
|nano		|2208|
|joe		|4772|
|vim		|5336|
|emacs -nw	|17060|
|emacs		|34924|
|atom		|250404|

RSS is amount of physical memory used in KiB.

## Memory used when loading "hello.c" with no syntax highlighting

|Editor|RSS|
|------|---|
|mg    |932|
|nano  |1684|
|joe   |4988|
|vim   |5180|
|emacs -nw |15584|
|emacs	  |33752|

## Memory used for test.xml with highlighting enabled

|Editor|RSS|
|------|---|
|vim   |11952|
|joe   |11968|
|nano  |17336|
|emacs -nw |23216|
|emacs	  |42892|

## Memory used for test.xml with no highlighting

|Editor|RSS|
|------|---|
|joe   |11496|
|vim   |11824|
|mg    |13420|
|nano  |14172|
|emacs -nw |21320|
|emacs	  |39492|
|atom	  |825232|

## Time used to load test.xml, jump to end of file and exit

|Editor|Time (seconds)|
|------|--------------|
|mg	  |.088|
|joe	  |.347|
|emacs -nw |.437|
|nano	  |.492|
|emacs	  |.852|
|vim	  |4.288|
|atom	  |18|

## Rehighlight test

Time used to load test.xml, split window, jump to end of buffer in other
window, insert '\<!--' at beginning (so that other window gets changed) and
then exit.

|Editor|Time (seconds)|
|------|---|
|joe   |.627|
|emacs -nw |8.036|
|vim	  |10.01|

## Search and Replace

Time used to load test.xml, and then execute 100,000 replacements (of "thing"
with "thang"), and then exit.

|Editor|Time (seconds)|
|------|-------|
|joe	| .683|
|vim	| 4.613|
|emacs	| 9.354|
|emacs -nw | 9.738|
|mg	| 467.989|
|nano	| at least 10 minutes|
|atom	| at least 10 minutes|

## Time to load 3 GB file, insert character at start and exit

|Editor|Time (seconds)|
|------|--------------|
|joe |43|
|mg  |system hangs|
|vim |system hangs|
|emacs |system hangs: but emacs warns file is "really huge"|
|nano  |system hangs|
|atom  |atom crashes: atom warns "may be unresponsive loading really huge file"|

## Time to reformat paragraph composed of two 120K long lines

|Editor|Time (seconds)|
|------|--------------|
|joe	 |.142|
|emacs -nw|1.811|
|vim	 |29.632|
|mg	 |35.552|
|nano	 |54.502|
