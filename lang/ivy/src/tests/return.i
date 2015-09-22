# Test returns at various points
# This verifies that we are emptying the stack properly

fn a1() {
	"Good 1"
}

fn a2() {
	return "Good 2"
}

fn a3() {
	1+2+3+{return "Good 3"}
}

fn a4() {
	loop {
		return "Good 4"
	}
}

fn a5() {
	loop {
		loop {
			return "Good 5"
		}
	}
}

fn a6() {
	var x
	for x=0 x!=10 ++x {
		var y
		for y=0 y!=10 ++y {
			return "Good 6"
		}
	}
}

fn a7() {
	var x
	for x=0 x!=10 ++x {
		var y = 7
		while a!=0 {
			return "Good 7"
		}
	}
}

fn a8() {
	var x
	for x=0 x!=10 ++x {
		var y = [1 2 3 4]
		foreach a y {
			return "Good 8"
		}
	}
}

fn a9() {
	var x
	for x=0 x!=10 ++x {
		foreach a [1 2 3 4 5] {
			return "Good 9"
		}
	}
}

print a1()
print a2()
print a3()
print a4()
print a5()
print a6()
print a7()
print a8()
print a9()
