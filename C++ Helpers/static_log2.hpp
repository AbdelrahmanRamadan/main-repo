
template <long long N>
struct static_log2 {
	const static int value = 1 + static_log2<(N >> 1)>::value;
};
template <>
struct static_log2 <0> {
	const static int value = -1;
};
