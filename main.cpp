#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <cstdio>
#include <fcntl.h>

struct Matrix {
	Matrix() { data = 0; }
	~Matrix() { if(data) delete[] data; }
	int rows;
	int cols;
	double * data;

	double * getRow(int r) {
		return &data[r*cols];
	}

	void read(const std::string &filename) {
		std::ifstream s(filename);
		if(!s.is_open()) {
			std::cout << "could not open " << filename << "\n";
			exit(0);
		}
		char ch;
		s >> ch;
		s >> rows;
		s >> cols;

		alloc(rows, cols);

		for(int r = 0; r < rows; r++) {
			for(int c = 0; c < cols; c++)
				s >> data[r*cols + c];
		}
	}

	void print() {
		for(int r = 0; r < rows; r++) {
			for(int c = 0; c < cols; c++)
				std::cout << data[r*cols + c] << " ";
			std::cout << "\n";
		}
	}

	void save(const std::string &filename) {
		std::ofstream s(filename);
		s << "# ";
		s << rows << " ";
		s << cols << " ";
		s << "\n";

		for(int r = 0; r < rows; r++) {
			for(int c = 0; c < cols; c++)
				s << data[r*cols + c] << " ";
			s << "\n";
		}
	}

	void alloc(int r, int c) {
		if(data)
			delete[] data;

		rows = r;
		cols = c;
		data = new double[r*c];
	}
};

int main(int argc, char**argv) {

	if(argc < 4){
		std::cout << "Usage : mat_file vec_file out_file\n";
		exit(0);
	}

	std::string matrix_f = argv[1];
	std::string vector_f = argv[2];
	std::string out_f = argv[3];

	Matrix m;

	m.read(matrix_f);
	m.print();
	m.save(out_f);

	Matrix v;

	v.read(vector_f);

	Matrix out;
	out.alloc(m.rows, 1);

	if(m.cols != v.rows) {
		std::cout << "wrong shape\n" << m.cols <<" "<< v.rows;
		exit(0);
	}

	int id = -1;

	std::string fifo_d = "/tmp/matrix_fifo";

	std::vector<std::string> fifos;

	for(int i = 0; i < m.rows; i++) {
		fifos.push_back(fifo_d + "[" + std::to_string(i) + "]");
		if(mkfifo(fifos[i].c_str(), 0666) < 0)
			perror("fifo");
	}

	std::vector<int> processes;

	for(int i = 0; i < m.rows; i++) {
		int pid = fork();

		if(pid == -1){
			perror("could not create fork");
			exit(1);
		}

		if(pid == 0) {
			id = i;
			break;
		}

		if(pid > 0) {
			processes.push_back(pid);
		}
	}

	if(id != -1) {
		std::ofstream g(fifos[id]);
		double * row = m.getRow(id);

		double res;

		for(int i = 0; i < m.cols; i++)
			res += row[i] * v.data[i];

		g << res;
		g.flush();

		//int fd = open(fifos[id].c_str(), O_WRONLY);
		//write(fd, "hi", 3);
		//close(fd);
		exit(0);
	}

	for(unsigned int i = 0; i < fifos.size(); i++) {
		std::ifstream g (fifos[i]);
		double res;
		g >> res;
		out.data[i] = res;
		//char buf[10];
		//int fd = open(fnd.c_str(), O_RDONLY);
		//read(fd, buf, 3);
		//close(fd);
	}

	int s;
	for(int pid : processes)
		waitpid(pid, &s, 0);

	for(std::string &st : fifos)
		unlink(st.c_str());

	out.save(out_f);

	return 0;

}