#include <iostream>
#include <bits/stdc++.h>
#include "chain_comparison.hpp"
using namespace std;

class tester {
public:
	int val;
	explicit tester(const int& _val) : val(_val) {}
	comparison<tester> operator < (const tester& other) const { return comparison<tester> (other, val < other.val); }
	comparison<tester> operator > (const tester& other) const { return comparison<tester> (other, val > other.val); }
};
int main () {
	
	cout << boolalpha << fixed;
	cout << (comparison<int>(0) < 5) << endl;
	cout << (comparison<int>(0) < 5 < 10 <= 10 >= 5 == 5) << endl;
	cout << (comparison<int>(0)) << endl;
	
	cout << "--------------------------------" << endl;
	
	tester A(1), B(2), C(3);
	
	cout << (A < B) << endl;
	cout << (A < B < C) << endl;
	cout << (A < B > C) << endl;
	return 0;
}
