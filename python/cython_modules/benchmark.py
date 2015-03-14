import time

def test () :
	world = client_api.World(10, 12)

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

versus()