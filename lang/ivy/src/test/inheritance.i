# Closure based inheritance

fn CreateMyClass() {

	# Construct the intance
	# This allows derived to construct...
	fn Construct(i) {
		i.x = 10
	}

	# Allocate and construct the instance
	fn Instance() {
		Construct(this.mom)	# Need the moms due to thunks
		return this
	}

	fn Show() {
		print x
	}

	fn Inc() {
		x = x + 1
	}

	fn Dec() {
		x = x - 1
	}
	return this
}

print "Create MyClass"
MyClass = CreateMyClass()

# Once we have '.' syntax...

#fn MyClass.Dec() {
#	x = x - 1
#}

print "Create instance of it"
Instance_2 = MyClass.Instance()
#print Instance_2

print "Show instance_2"
Instance_2.Show()
print "Modify instance_2"
Instance_2.Inc()
print "Show it"
Instance_2.Show()

print "Create another instance of it"
Instance_1 = MyClass.Instance()
#print Instance_1

print "Show instance_1"
Instance_1.Show()
print "Modify instance_1"
Instance_1.Dec()
print "Show it"
Instance_1.Show()

# Inheritance

# Again we need the . notation here...
fn CreateDerivedClass() {
	mom=MyClass
	fn Construct(i) {
		mom.mom.Construct(i) # Call base class constructor: confusing number of moms
		i.y = 20 # Add another variable to it..
	}

	fn Instance() {
		Construct(this.mom)	# mom due to thunk
		return this
	}

	fn Show() {
		print "Hello"
		print x
		print y
	}

	return this
}

#MyDerivedClass = CreateDerivedClass(`mom=MyClass) # This also works
MyDerivedClass = CreateDerivedClass()

print "Create instance of it"
Instance_3 = MyDerivedClass.Instance()
#print Instance_3
print "Call its show"
Instance_3.Show()
print "Done"
