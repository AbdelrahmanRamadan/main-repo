#include <iostream>
using namespace std;

template <typename T>
struct valid_type : std::integral_constant<bool, std::is_integral<T>::value  > {};

template <size_t N, typename T = int>
struct fibonacci_array {
	static_assert(valid_type<T>::value, "invalid storage type for fibonacci sequence");
	T values[N];
	constexpr fibonacci_array() : values() {
        if (N > 1)
            values[0] = values[1] = 1;
        for(size_t i = 2; i < N; ++i)
            values[i] = values[i - 1] + values[i - 2];
    }
    constexpr T operator [] (size_t pos) const
    {
        return values[pos];
    }
};
int main () {
	const size_t n = 10;	
	fibonacci_array<n, long long> arr;
	for(size_t i = 0; i < n; ++i)
		cout << arr[i] << endl;
	return 0;
}
