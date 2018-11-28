# Closure based inheritance

fn CreateMyClass() {

	# Construct the intance
	# This allows derived to construct...
	fn Construct(i) {
		print "Construct0"
		i.x = 10
	}

	# Allocate and construct the instance
	fn Instance() {
		print "Instance0"
		Construct(this)
		return this
	}

	fn Show() {
		print x
	}

	fn Inc() {
		x = x + 1
	}

	return this
}

print "Create MyClass"
MyClass = CreateMyClass()

fn MyClass.Dec() {
	x = x - 1
}

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


# Inheritance by changing mom..

fn CreateDerivedClass() {
	mom=MyClass

	fn Construct(i) {
		print "Construct1"
		mom.mom.Construct(i) # Call base class constructor
		i.y = 20 # Add another variable to it..
	}

	fn Instance() {
		print "Instance1"
		Construct(this)
		return this
	}

	fn Show() {
		print "Hello"
		print x
		print y
	}

	return this
}

print "Create a derived"
#MyDerivedClass = CreateDerivedClass(`mom=MyClass) # This also works
MyDerivedClass = CreateDerivedClass()

print "Create instance of it"
Instance_3 = MyDerivedClass.Instance()
#print Instance_3
print "Call its show"
Instance_3.Show()
print "Done"


# Inheritance by adding joining the base

fn MyClass.CreateAnotherDerived() {
	fn Construct(i) {
		print "Construct2"
		mom.mom.Construct(i) # Call base class constructor: confusing number of moms
		i.z = 30 # Add another variable to it..
	}

#	fn Instance() {
#		print "Instance2"
##		Construct(this)
#		return this
#	}

	fn Show() {
		print "Hi"
		print x
		print z
	}
	fn Huh() {
		print argv
		#return fn((),print("fin"))
	}
	return this
}

print "Create another derived"
AnotherDerivedClass = MyClass.CreateAnotherDerived()

print "Create instance of it"
Instance_4 = AnotherDerivedClass.Instance()
print "Call its show"
Instance_4.Show()
#print Instance_4
print "Done"
