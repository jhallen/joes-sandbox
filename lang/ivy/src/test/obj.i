var z = 1

var stat = [`mom=this `y=2]

fn mkbase() {
	var x = 10

	mom = stat

	fn add(z) {
		x += z
		return this
	}

	fn mul(z) {
		x *= z
		return this
	}

	fn show() {
		print x
		return this
	}

	return this
}

var a = mkbase()
var b = mkbase()
print a.x
print a.y
print a.z
a.show()
a.x=20
b.show()
a.y=30
print b.y
