# Check that break is cleaning up stack properly

fn a1() {
	loop `outer {
		loop `inner {
			break outer
		}
	}
	"Good 1"
}

fn a2() {
	loop `outer {
		var x
		for x=0 x!=10 ++x {
			break outer
		}
	}
	"Good 2"
}

fn a3() {
	loop `outer {
		var x=10
		while x!=5 {
			break outer
		}
	}
	"Good 3"
}

fn a4() {
	loop `outer {
		var y
		var x=[1 2 3]
		foreach y x {
			break outer
		}
	}
	"Good 4"
}

fn a5() {
	loop `outer {
		var y
		foreach y [1 2 3] {
			break outer
		}
	}
	"Good 5"
}

print a1()
print a2()
print a3()
print a4()
print a5()
