#include <iostream>
using namespace std;

template <typename T>
struct valid_type : std::integral_constant<bool, std::is_integral<T>::value  > {};

template <size_t N, typename T = int, typename = void>
struct fibonacci {
	static const T value = fibonacci<N - 1, T>::value + fibonacci<N - 2, T>::value;
};
template <size_t N, typename T>
struct fibonacci <N, T, typename std::enable_if<(N < 2)>::type > {
	static const T value = 1;
};
template <size_t N, typename T = int>
struct fibonacci_array : fibonacci_array<N - 1, T> {
	static_assert(valid_type<T>::value, "invalid storage type for fibonacci sequence");
	T value;
	fibonacci_array() : value(fibonacci<N - 1, T>::value) {}
	T operator [] (size_t pos) {
		return *((T*)this + pos);
	}
};

template <typename T>
struct fibonacci_array<0, T> {};

int main () {
	const size_t n = 10;	
	fibonacci_array<n, long long> arr;
	for(size_t i = 0; i < n; ++i)
		cout << arr[i] << endl;
	return 0;
}
