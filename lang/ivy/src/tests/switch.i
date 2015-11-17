# Define a switch statement..

fn switch(&val) {
	var rtn, x, v = *val
	x = 1
	while x < len(argv) - 1 {
		if *argv(x) == v {
			return *argv(x + 1)
		}
		x += 2
	}
	if x < len(argv) {
		return *argv(x)
	}
	return void
}

# Try it..

print switch({ 4
} 1 {
	"one"
} 2 {
	"two"
} 3 {
	"three"
})
