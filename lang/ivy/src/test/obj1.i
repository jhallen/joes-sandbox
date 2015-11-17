# Why can't all object inherit from global object?
#   -> we don't normally want someobj.print to return anything
# How does javascript do this?
#   -> Prototypes must be special:

# Prototype
var test = [
	`mom=this	# Needed otherwise print will not be found.
	`y=2
]

# Constructor
fn mktest() {
	var o = [
		`mom = test
		`x = 10
	]
	return o
}

# Member functions of test
test.add = fn((z),{
	x += z
})

test.mul = fn((z),{
	x *= z
})

test.show = fn((),{
	print x
})

print "Hi"
var a = mktest()
print "There"
var b = mktest()
print a.x
a.add(3)
a.show()
a.mul(3)
a.show()

b.show()

# Inheritance

var newtest = dup(test)

newtest.show = fn((),{
	print "New show " x
})

fn mknewtest() {
	var o = mktest()
	o.mom = newtest
	return o
}

var c = mknewtest()
c.show()
c.mul(3)
c.show()

b.show()
