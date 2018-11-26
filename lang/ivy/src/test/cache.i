# Fibonacci with cached values

var fib = fn((), {
	var cache = []
	return fn((n), {
		var val
		if val=cache(n) {
			# print "Cached value for " n
			return val
		}

		# print "Compute value for " n

		if n < 2 {
			val = n
		} {
			val = fib(n - 1) + fib(n - 2)
		}

		cache(n) = val

		return val
	})
})()

print fib(30)
