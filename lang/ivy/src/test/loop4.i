var x, y

x=0
while `outer x!=10 {
  y=0
  while `inner y!=10 {
    print "outer ", x, " inner ", y
    if x == 5 && y == 5 {
      break outer
    }
    y=y+1
  }
  x=x+1
}
