Examples from UGL 126

  PRINT 5
5 FORMAT('....')

  READ 3,IPRNT
3 FOMAT(80A1)

- - - -
Keywords from compiler binary:

go to if stop end do continue call subroutine function return read print
dimension common data format not and or eq ne le ge lt gt write rewind

## System Calls

```
	call openf(number,filename,mode?)
           mode = 1: read
           mode = 2: write
           filename: should be a 'dimension name(7)'

	call eoftst(file,result)
	  result:=2 if eof, 1 otherwise

	rewind file
          this is broken in that it does not reset the EOF flag!

	call closef(number)


	call delf(number)

		number 101 is write only.. console output?
		102 is printer output
		99 is re-read last one read (so you can do multiple reads per record)

	file name looks like this:  

		DIMENSION IN(7)
		DATA IN/'DI','SK','DA','TA','.S','A:','1'/


	call lpinit   prepare printer, print 6 blank lines
	call page	form feed printed
        call lpq(luw)  prints "LINE PRINTER - Y/N?"
               102 in luw for printer
               101 otherwise (console I guess)
	call filtst(ifn,icode)
             ifn search for file ifn
             icode = -1 file not found, 0 drive not ready, 1 file was found

	call irand(n)
             n is random integer

	call err(n)
	     prints "*** EXECUTION TIME ERROR #nn"

	call seteof(fileno)
		move to end of file (prepare for appending)

	call delf(n)
		delete open file

        iabs(n)
	abs
	ishft
	power(x,y)
	sqrt
	alog
```

## Format syntax

```

format control:
  blank:  cr/lf
  0       cr/lf/lf
  1       ff/cr/lf
  +       no cr/lf
  (other) cr/lf
  Tn      tab to locaiton n
  /       blank line

yes  Iw	integer  (1 allowed)
yes  Fw.d	real
yes  Ew.d
  Dw.d  double precision (probably not..)
  Aw    character (file name is 7A2)
  wX    space
yes  'xx'  string
```

## FORTRAN constructs

```
a line with a label alone is not allowed

identifiers beginning with I .. N are integers, others are real

yes:
 stop

yes:
C comment

yes:
 <statement> ; this is a comment too

yes:
 <statement..>
& <...statement>

yes:
 read(100,900)I,J
900 format()       (unformatted)

yes:
 write(101,900)I,J
 print(101,900)I,J

yes:
 print(LD,900)I,J

yes:
 print 1
 write 1

yes:
do 30 I = 10, 100
30 continue

yes:
do 40 I = 10,100,10   initial, final, increment
40 continue

yes:
 data i/1/
 data a/.33/

yes:
 go to 100

yes: (comma required betwen ) and i)
 go to (10,20,30),i

yes:
 if (x.gt.y) y=x

yes:
 if (x) 100,200,300   (negative, zero, positive)

yes:
 function fred(a,b,c)
 formal declarations
 local declarations
 code
 fred=123
 return
 end

yes: (but must come before code outside of subroutine.. why?)
 subroutine bob(a,b,c)
 formals
 locals
 code
 return
 end

 call bob(x,y,z)

- - -
 forlb:
 run
 rxio
 diskio
   power exp alog sin cos cosin atan sqrt mod abs
   errlib
   iand ior ieor ishft

no: program foo

no: go to i

no:
* comment

no:  READ(100,*)I,J   should work.. maybe blank format line instead of *

no:  REAL J

no:
 data i,j,k/1,2,3/
 data i,j,k/3*0/

```
