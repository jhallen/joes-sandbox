var x, y

foreach `outer x [0 1 2 3 4 5 6 7 8 9] {
  foreach `inner y [0 1 2 3 4 5 6 7 8 9] {
    print "outer ", x, " inner ", y
    if x == 5 && y == 5 {
      break outer
    }
  }
}
