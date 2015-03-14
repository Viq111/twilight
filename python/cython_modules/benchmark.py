import time

def test () :
	world = client_api.World(10, 10)
	world.init_map( (5,5), 
		( (5,5,0,9,0), 
			(0,5,0,0,9), 
			(5,0,5,0,0),
			(3,2,5,0,0),
			(3,6,5,0,0),
			(5,7,5,0,0),
			(7,7,5,0,0),
			(2,2,5,0,0), ) )
	print world


def benchmark (fun) :
	start = time.time()
	for i in range(1000) :
		fun
	end = time.time()
	return end-start

def versus():
	import client_api
	print "Benchmarking std python"
	for i in range(1,4) :
		print benchmark(test)

	import client_api_cython
	print ""
	print "Benchmarking cython version"
	for i in range(1,4) :
		print benchmark(test)

if __name__ == '__main__':
	import client_api
	test()