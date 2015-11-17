var x, y

x = 0
loop `outer {
  y = 0
  loop `inner {
    print "outer ", x, " inner ", y
    if x == 5 && y == 5 {
      break outer
    }
    y = y + 1
    if y == 10 {
      break
    }
  }
  x = x + 1
  if x == 10 {
    break
  }
}
