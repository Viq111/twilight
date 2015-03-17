import time

import minmax_python as minmax_api
# import minmax_cython as minmax_api

import ai_pylight
# import ai_pylight_cython as ai_pylight


def test (debug = False) :
	myPos = (5,5)

	pos = [
		ai_pylight.Objective((5,5),2),
		ai_pylight.Objective((0,5),4), 
		ai_pylight.Objective((5,0),6),
		ai_pylight.Objective((3,2),7),
		ai_pylight.Objective((3,6),8),
		ai_pylight.Objective((5,7),9),
		ai_pylight.Objective((7,7),9),
		ai_pylight.Objective((2,6),9),
		ai_pylight.Objective((4,2),9),
		ai_pylight.Objective((4,3),9),
		ai_pylight.Objective((4,4),9),
		ai_pylight.Objective((4,5),9),
		ai_pylight.Objective((2,2),9),
		ai_pylight.Objective((5,6),9),
		ai_pylight.Objective((7,3),9),
		ai_pylight.Objective((6,3),9),
		ai_pylight.Objective((6,4),1),
		ai_pylight.Objective((6,5),1),
		ai_pylight.Objective((6,6),1)
		]
	player1 = ((2,2),13,0)
	player2 = ((7,7),13,0)
	game = ai_pylight.BoardGame(player1, player2, pos)
	minmax = minmax_api.MinMax(game, 5)
	return minmax.ask_moves()



def benchmark (fun) :
	start = time.time()
	for i in range(1) :
		fun()
	end = time.time()
	return end-start

def versus():
	for i in range(1,4) :
		print benchmark(test)


def correction():
	l1 = test()
	print l1



if __name__ == '__main__':
	print "* Testing correction"
	correction()
	print
	print
	print "* Testing performance"
	versus()