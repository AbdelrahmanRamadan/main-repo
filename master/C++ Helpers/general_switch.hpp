#ifndef GENERAL_SWITCH_HPP
#define GENERAL_SWITCH_HPP

template <class T>
class switch_impl;

template <class U>
switch_impl<U> general_switch(const U&);

template <class T>
class switch_impl {
	public:
	const T& obj;
	bool breaked;
	switch_impl(const T& _obj) : obj(_obj), breaked(false) {}
	template <class D, class F>
	switch_impl<T>& general_case(const D& _obj, const F& callback, bool break_if_matched = true) {
		if(!breaked && obj == _obj) {
			callback();
			if(break_if_matched)
				breaked = true;
		}
		return *this;
	}
	template <class F>
	void default_case(const F& callback) {
		if(!breaked)
			callback();
	}
	template <class U>
	friend switch_impl<U> general_switch(const U&);
};

template <class U>
switch_impl<U> general_switch (const U& obj) {
	return switch_impl<U> (obj);
}


#endif // GENERAL_SWITCH_HPP
