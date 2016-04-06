#!./py2/bin/python
import sys
import numpy as np

if __name__ == "__main__":
	if len(sys.argv) < 3:
		print("Usage : nrow ncol")
		sys.exit(0)

	array_dim = int(sys.argv[1]), int(sys.argv[2])

	print(array_dim)
	a = np.random.rand(*array_dim)
	v = np.random.rand(array_dim[1])
	np.savetxt("matrix.txt", a, header="{0} {1}".format(*array_dim))
	np.savetxt("vector.txt", v, header="{0} {1}".format(array_dim[1], 1));
	g = np.dot(a, v)
	np.savetxt("output_ref.txt", g, header="{0} {1}".format(array_dim[0], 1));