var x, y

for `outer x=0 x!=10 x=x+1 {
  for `inner y=0 y!=10 y=y+1 {
    print "outer ", x, " inner ", y
    if x == 5 && y == 5 {
      break outer
    }
  }
}