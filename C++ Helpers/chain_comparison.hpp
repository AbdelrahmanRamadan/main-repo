#ifndef CHAIN_COMPARISON_HPP
#define CHAIN_COMPARISON_HPP

template <class T>
class comparison {
	const T& last;
	bool result;
public:
	comparison(const T& obj, bool _result = true) : last(obj), result(_result) {}
	operator bool() { return result; }
	template <class U>
	friend comparison<U> operator < (const comparison<T>& me, const U& obj) {
		return comparison<U>(obj, me.result && me.last < obj);
	}
	template <class U>
	friend comparison<U> operator > (const comparison<T>& me, const U& obj) {
		return comparison<U>(obj, me.result && me.last > obj);
	}
	template <class U>
	comparison<U> operator <= (const U& obj) const {
		return comparison<U>(obj, result && last <= obj);
	}
	template <class U>
	comparison<U> operator >= (const U& obj) const {
		return comparison<U>(obj, result && last >= obj);
	}
	template <class U>
	comparison<U> operator == (const U& obj) const {
		return comparison<U>(obj, result && last == obj);
	}
	
};




#endif //CHAIN_COMPARISON_HPP
