#include <iostream>
#include <algorithm>
#include <numeric>
#include <string>
#include <bitset>
#include <sstream>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <cstddef>
#include <cassert>
#include <cstdlib>
#include <typeinfo>
#include <functional>
using namespace std;
class Chromosome;
class Fitness;
class GN;

long long llrand() {
	return ((long long)rand() << 32) + rand();
}

class Chromosome {
	typedef unsigned long long chtype;
	typedef long unsigned int size_t;
	static const int mutation_rate = 2;	// value from 0 to 64 determines the rate of bit changes (set to low rate)
	chtype data;
	size_t _size;
public:
	static const size_t MAXSIZE = 64;
	
	inline size_t size() const{
		return _size;
	}
	Chromosome(const string& bits) {
		data = 0;
		_size = min(bits.size(), MAXSIZE);
		for(size_t i = 0; i < _size; ++i)
			data |= (chtype)(bits[i] - '0') << (_size - i - 1);
	}
	Chromosome(size_t usize = MAXSIZE, const chtype& bits = 0) : data(bits), _size(min(usize, (size_t)MAXSIZE)) {}
	Chromosome& operator = (const Chromosome& other) {
		data = other.data;
		_size = other._size;
		return *this;
	}
	Chromosome(const Chromosome& other) : data(other.data), _size(other._size) {}
	
	Chromosome randomize() const {
		Chromosome ret(*this);
		ret.data = llrand();
		return ret;
	}
	bool operator [] (size_t pos) const {
		return (data >> pos) & 1;
	}
	void set(size_t pos, bool bit) {
		data = (data & ~((chtype)1 << pos)) | ((chtype)bit << pos);
	}
	friend ostream& operator << (ostream& fout, const Chromosome& other) {
		for(size_t i = 0; i < other._size; ++i)
			fout << other[i];
		return fout << endl; 
	}
	friend istream& operator >> (istream& fin, Chromosome&  other) {
		string bits;
		fin >> bits;
		other = Chromosome(bits);
		return fin;
	}
	
	void mutate() {
		for(size_t i = 0; i < _size; ++i)
			if((rand() & 63) < mutation_rate)
				set(i, rand() & 1);
	}
};
class Population {
public:
	typedef Chromosome* iterator;
private:
	iterator data; // data container should support random access
	size_t _size;
	
	void free_resources() {
		delete [] data;
	}
	void get_copy(const Population& other, bool constructor = false) {
		if(this != &other) {
			if(!constructor)
				free_resources();
			_size = other._size;
			data = iterator(new Chromosome[_size]);
			for(size_t i = 0; i < _size; ++i)
				data[i] = Chromosome(other[i]);
		}
	}
public:
	
	Population() : data(iterator(NULL)), _size(0) {}
	Population(size_t usize, const Chromosome& c = Chromosome(), bool randomize = false) : _size(usize) {
		data = iterator(new Chromosome[_size]);
		for_each(data, data + _size, [&c, randomize](Chromosome& cur) {
			cur = randomize ? c.randomize() : c;
		});
	}
	Population& operator = (const Population& other) {
		get_copy(other);
		return *this;
	}
	Population(const Population& other){
		get_copy(other, true);
	}
	~Population() {
		free_resources();
	}
	
	inline iterator begin() {
		return iterator(data);
	}
	inline iterator end() {
		return iterator(data + _size);
	}
	inline size_t size() const { // Population size
		return _size;
	}
	const Chromosome& operator [] (size_t pos) const {
		return data[pos];
	}
	Chromosome& operator [] (size_t pos) {
		return data[pos];
	}
	
	friend ostream& operator << (ostream& fout, const Population& other) {
		fout << "Population size = " << other._size << endl << "Population Chromosomes: " << endl;
		for(size_t i = 0; i < other._size; ++i)
			fout << other[i] << endl;
		return fout;
	}
	friend istream& operator >> (istream& fin, Population& other) {
		fin >> other._size;
		for(size_t i = 0; i < other._size; ++i)
			fin >> other[i];
		return fin;
	}
};
class Fitness {
public:
	static Chromosome target;
	
	static int calc_fitness(const Chromosome& other) {
		int fitness = 0;
		for(size_t i = 0; i < other.size(); ++i)
			fitness += other[i] == target[i];
		return fitness;
	}
	static int calc_fitness(Population::iterator begin, Population::iterator end, const function<bool(int,int)>& cmp = less<int>()) {
		// returns maximum fitness in range
		if(begin == end)
			return 0;
		int fitness = calc_fitness(*begin);
		while(++begin != end) {
			int new_fitness = calc_fitness(*begin);
			if(cmp(fitness, new_fitness))
				fitness = new_fitness;
		}
		return fitness;
	}
	static Population::iterator get_fittest(Population::iterator begin, Population::iterator end, const function<bool(int,int)>& cmp = less<int>()) {
		if(begin == end)
			return end;
		Population::iterator fittest = begin;
		int fitness = calc_fitness(*fittest);
		while(++begin != end) {
			int new_fitness = calc_fitness(*begin);
			if(cmp(fitness, new_fitness)) {
				fitness = new_fitness;
				fittest = begin;
			}
		}
		return fittest;
	}
};
// Genetic Algorithm Class Implementation
class GN {
	static const bool elitism = true;
	static const int crossover_rate = 32; // value from 0 to 64 determines the rate of bit swapping in crossover (set to uniform rate)
	static const int counter_limit = 30;
	
public:
	Population generation; // current population
	const size_t tournament_size;
	size_t counter = 0;
	
	GN() : generation(), tournament_size(0) {}
	GN(const Population& upopulation) : generation(upopulation), tournament_size((upopulation.size() + 4) / 5) {}
	
	Population::iterator tournament_selection(const function<bool(int,int)>& cmp = less<int>()) {
		// implemented in O(n) time, O(k) space using Reservoir Sampling Algorithm ; k = tournament_size
		
		Population::iterator players[tournament_size], winner;
		for(size_t i = 0; i < tournament_size; ++i)
			players[i] = generation.begin() + i;
		for(size_t i = 0; i < generation.size(); ++i) {
			size_t j = rand() % (i + 1);
			if(j < tournament_size)
				players[j] = generation.begin() + i;
		}
		winner = players[0];
		int fitness = Fitness::calc_fitness(*winner);
		for(size_t i = 1; i < tournament_size; ++i) {
			int new_fitness = Fitness::calc_fitness(*players[i]);
			if(cmp(fitness, new_fitness)) {
				fitness = new_fitness;
				winner = players[i];
			}
		}
		return winner;
	}
	
	Chromosome crossover(const Chromosome& parent1, const Chromosome& parent2) {
		Chromosome child;
		for(size_t i = 0; i < parent1.size(); ++i)
			child.set(i, ((rand() & 63) < crossover_rate ? parent1[i] : parent2[i]));
		return child;
	}
	void evolve() {
		Population new_generation(generation);
		if(elitism) {
			new_generation[0] = *Fitness::get_fittest(generation.begin(), generation.end());
		}
		for(size_t i = elitism; i < new_generation.size(); ++i) {
			Population::iterator parent1 = tournament_selection();
			Population::iterator parent2 = tournament_selection();
			new_generation[i] = crossover(*parent1, *parent2);
			new_generation[i].mutate();
		}
		generation = new_generation;
	}
	void converge() {
		size_t generation_id = 0;
		int max_fitness = Fitness::calc_fitness(generation.begin(), generation.end());
		int target_fitness = Fitness::calc_fitness(Fitness::target);
		cout <<"Generation "<< generation_id << endl;// << ":" << endl << generation << endl;
		cout <<"Fitness = " << max_fitness << endl << endl;
		while(counter < counter_limit && max_fitness != target_fitness) {
			++generation_id;
			evolve();
			int new_fitness = Fitness::calc_fitness(generation.begin(), generation.end());
			if(max_fitness == new_fitness)
				++counter;
			else {
				counter = 0;
				max_fitness = new_fitness;
			}
			cout <<"Generation "<< generation_id << endl;// << ":" << endl << generation << endl;
			cout <<"Fitness = " << max_fitness << endl << endl;
		}
	}
	
};
Chromosome Fitness::target = Chromosome(64, 0xf00000000000000f);
int main() {
	srand(time(NULL));
	GN test(Population(50, Chromosome(64), true));
	test.converge();
	return 0;
}
