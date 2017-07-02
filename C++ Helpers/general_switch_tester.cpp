#include <iostream>
#include "general_switch.hpp"
using namespace std;
int main() {
	// lambda expression used for callbacks in the tests can be replaced with any callable object (takes no parameters or parameters with default value)
	// string literals have to be wrapped in a string class because using operator == on literals compares the addresses not the actiual values (it results in undefined behavior depending on the compiler)
	
	general_switch(string("AAA"))
	.general_case(string("BBB"), [] () { cout << "True At 1" << endl; })
	.general_case(string("AAA"), [] () { cout << "True At 2" << endl; })
	.general_case(string("AAA"), [] () { cout << "True At 3" << endl; })
	.general_case(string("AA"), [] () { cout << "True At 4" << endl; })
	.general_case(string("AAA"), [] () { cout << "True At 5" << endl; })
	.default_case([] () { cout << "True At 6" << endl; });
	//.general_case("BlaBla", []....); error can't define cases after the default case
	
	cout << "-------------" << endl;
	
	// there's third optional parameter for every case except the default one to determine if you want to break after this condition if it matched - its set to true by default
	general_switch(string("AAA"))
	.general_case(string("BBB"), [] () { cout << "True At 1" << endl; })
	.general_case(string("AAA"), [] () { cout << "True At 2" << endl; }, false)
	.general_case(string("BB"), [] () { cout << "True At 4" << endl; })
	.general_case(string("AAA"), [] () { cout << "True At 3" << endl; }, false)
	.general_case(string("AA"), [] () { cout << "True At 4" << endl; })
	.general_case(string("AAA"), [] () { cout << "True At 5" << endl; })
	.default_case([] () { cout << "True At 6" << endl; });
	
	
	return 0;
}
