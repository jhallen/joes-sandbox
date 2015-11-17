var x, y

x=0
while `outer x!=10 {
  foreach `inner y [1 2 3 4 5 6 7 8 9 10] {
    print "outer ", x, " inner ", y
    if x == 5 && y == 5 {
      break outer
    }
    y=y+1
  }
  x=x+1
}
