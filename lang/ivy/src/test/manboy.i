fn A(k, &x1, &x2, &x3, &x4, &x5) {
	fn B() {
		k = k - 1
		return A(k, B(), *x1, *x2, *x3, *x4)
	}
        if k <= 0 {
                return *x4 + *x5
        } {
                return B()
        }
}

print A(10, 1, -1, -1, 1, 0)
