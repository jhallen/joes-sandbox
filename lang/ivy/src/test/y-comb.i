# Define Y-combinator: strict eval version

fn Y(f) fn((x),x(x))(fn((x),f(fn((y),x(x)(y)))))

# Try it- build factorial from almost-factorial
fn af(f) fn((n),if(n==0,1,n*f(n-1)))

fact=Y(af)

print fact(6)


# Lazy eval version

fn af(&f) fn((n),if(n==0,1,n*(*f)(n-1)))

#fn Y(f) fn((x),f(x(x)))(fn((x),f(x(x))))

fn Y(f) {
	fn inner(z) f(z(z))
	inner(inner)
}

fact=Y(af)

print fact(6)
