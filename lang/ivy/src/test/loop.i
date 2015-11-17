var x y

#y = 0
#loop `outer
#  x = 0
#  loop `inner
#    print y, " ", x
#    x = x + 1
#    if x == 5
#      break inner
#  y = y + 1
#  if y == 5
#    break outer

#for `outer x=0 x!=10 ++x
foreach `outer x [0 1 2 3 4 5 6 7 8 9 ] {
  print "Outer " x
#  foreach y [1 2 3 4 5 6 7]
  for y=0 y!=10 ++y {
    print y
    if y == 7 {
      break outer
    }
  }
}

print "Done"
