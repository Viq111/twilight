import time

import client_api_python as python, client_api_cython as cython

def test (api, debug = False) :
	world = api.World(10, 10)
	myPos = (5,5)
	pos = (
		(5,5,0,9,0),
		(6,6,0,0,15),
		(0,5,0,0,9), 
		(5,0,0,0,3),
		(3,2,5,0,0),
		(3,6,5,0,0),
		(5,7,5,0,0),
		(7,7,5,0,0),
		(2,6,5,0,0),
		(4,2,5,0,0),
		(4,3,5,0,0),
		(4,4,5,0,0),
		(4,5,5,0,0),
		(2,2,5,0,0),
		(5,6,0,0,3),
		(7,3,0,0,15),
		(6,3,0,0,9),
		(6,4,0,0,10),
		(6,5,0,0,11),
		(6,6,0,0,15)
		)
	world.init_map(myPos, pos)
	
	if debug :
		print world
		print

	list = []

	for p in pos:
		if debug :
			print p
		i = 0
		while myPos != (p[0], p[1]) and i<21:
			i+=1
			nextPos = world.find_path(myPos, (p[0], p[1]), 9)
			world.update(myPos[0], myPos[1], 0, 0, 0)
			world.update(nextPos[0], nextPos[1], 0, 9, 0)
			myPos = nextPos
			if debug :
				print world
				print
		list.append(i)

	return list



def benchmark (fun, api) :
	start = time.time()
	for i in range(1) :
		fun(api)
	end = time.time()
	return end-start

def versus():
	print "Benchmarking std python"
	for i in range(1,4) :
		print benchmark(test, python)

	print ""
	print "Benchmarking cython version"
	for i in range(1,4) :
		print benchmark(test, cython)

def correction():
	print "with python"
	l1 = test(python)
	print l1

	print "with cython"
	l2 = test(cython)
	print l2

	if l1 == l2:
		print "* PASSED *"
	else :
		print "* !!! FAILED !!! *"

if __name__ == '__main__':
	print "* Testing correction"
	correction()
	print
	print
	print "* Testing performance"
	versus()