a = fn((),{
	var x = 10

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
})()

a.show().add(2).mul(2).show()
