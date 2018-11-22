# Weird things in Ivy..

# 1
print "\nNumber 1"

fn foo() {
	fn huh() {
		print "Why is this called?"
		print "Maybe defining a named function should return void"
	}
	# return this   # <- forgot it..
}

z=foo()
z

# 2
print "\nNumber 2"

fn bar() {
	return 20
}

x=bar()
# This gives no error...
x()
# We're calling 20...
# We allow it with no args to allow just typing 20 in calculator mode.  20
# alone looks like a function call.  So calling a non-function with no args
# just returns the value itself.  Yuck.


# 3
print "\nNumber 3"

# Dot notation is a function call

fn huh() {
	print argv
	return fn((),print("fin"))
}

huh.bob()

# huh is getting called with `bob, then we call the result, so fin is
# printed
