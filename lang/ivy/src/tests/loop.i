var x y

print "First loop:"

x = 0
loop {
	print x
	++x
	if x==10 {
		break
	}
}

print "First loop done."
print "Second loop:"

for x=0 x!=10 ++x {
	print x
}

print "Second loop done."
print "Third loop:"

for `outer x=0 x!=10 ++x {
#foreach `outer x [0 1 2 3 4 5 6 7 8 9 ] {
  print "Outer " x
#  foreach y [1 2 3 4 5 6 7] {
  for y=0 y!=10 ++y {
    print y
    if y == 7 {
      break outer
    }
  }
}

print "Third loop done"
print "Fourth loop"

#for `outer x=0 x!=10 ++x {
foreach `outer x [0 1 2 3 4 5 6 7 8 9 ] {
  print "Outer " x
  foreach y [1 2 3 4 5 6 7] {
#  for y=0 y!=10 ++y {
    print y
    if y == 7 {
      break outer
    }
  }
}

print "Fourth loop done."
