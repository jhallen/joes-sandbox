# Base class

MyClass = [ `mom=this ]

fn MyClass.construct(i=[]) {
        i.x = 10
	i.mom = MyClass
# Fix OR so we can do this...
#	i |= [`mom=MyClass, `x=10]
	return i
}

fn MyClass.show() {
        print x
}

fn MyClass.inc() {
        x = x + 1
}

fn MyClass.dec() {
        x = x - 1
}

print
print "Construct instance of MyClass..."
a = MyClass.construct()
print "Call show member function..."
a.show()
print "Modify..."
a.inc()
print "Call show member function..."
a.show()


# Inheritance

DerivedClass = [ `mom=MyClass ]

fn DerivedClass.construct(i=[]) {
	i = MyClass.construct(i)
        i.y = 20
	i.mom = DerivedClass
# Fix OR so we can do this...
#	i |= [`mom=DerivedClass, `y=20]
	return i
}

fn DerivedClass.show() {
        print "Hello"
        print x
        print y
}

print
print "Construct instance of DerivedClass..."
b = DerivedClass.construct()
print "Call show member function..."
b.show()
print "Modify..."
b.dec()
print "Call show member function..."
b.show()
