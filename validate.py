#!./py2/bin/python
import sys
import numpy as np

if __name__ == "__main__":
	if len(sys.argv) < 3:
		print("Usage : filemat1 filemat2")
		sys.exit(0)

	z = np.loadtxt(sys.argv[1])
	d = np.loadtxt(sys.argv[2])

	if np.allclose(z, d):
		print("Equal")
	else:
		print("Not equal")