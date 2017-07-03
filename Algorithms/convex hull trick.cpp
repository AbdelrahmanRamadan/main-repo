/*
 * Author: Abdelrahman Ramadan
 * Problem: 5133 - Machine Works
 * Judge: Live Archive
 * Link: https://icpcarchive.ecs.baylor.edu/index.php?option=onlinejudge&page=show_problem&problem=3134
 */

#include <iostream>
#include <algorithm>
#include <set>
#include <numeric>
#include <cstdio>
#include <cstring>
#include <cstdlib>
using namespace std;
const int MAX = 100100;
const long long inf = 1LL << 61;
struct line {
	long long m, c;
	bool operator < (const line& o) const{
		// if want min val replace '<' with '>'
		return m < o.m || (m == o.m && c < o.c);
	}
} lines[MAX];
int lines_count;
set<int, function<bool(int, int)> > active([] (int l1, int l2) {return lines[l1] < lines[l2];});
long double intersect_x(int l1, int l2) {
	return abs(lines[l1].m) == inf ? lines[l1].c : abs(lines[l2].m) == inf ? lines[l2].c
		: 1.0 * (lines[l1].c - lines[l2].c) / (lines[l2].m - lines[l1].m);
}
bool is_envelope(int l1, int l2, int l3) {
	return intersect_x(l2, l3) > intersect_x(l2, l1) + 1e-9;
}
int new_line(long long m, long long c) {
	lines[lines_count++] = {m, c};
	return lines_count - 1;
}
void fix_before(set<int>::iterator it2) {
	auto it1 = it2, it3 = it2;
	--it1, ++it3;
	if(lines[*it1].m == lines[*it2].m)
		active.erase(it1--);
	else if(lines[*it3].m == lines[*it2].m || !is_envelope(*it1, *it2, *it3)) {
		active.erase(it2);
		return;
	}
	if(it1 == active.begin()) return;
	--it1, --it2, --it3;
	while(!is_envelope(*it1, *it2, *it3))
		active.erase(it2--), it1--;
}
void add_line(long long m, long long c) {
	fix_before(active.insert(new_line(m, c)).first);
}
long long get_optimal(int x) { // assumes that there's at least one valid line in the active set and the two dummy nodes
	auto it1 = ++active.begin(), it2 = ++++active.begin();
	while(intersect_x(*it1, *it2) <= x)
		active.erase(it1++), it2++;
	return lines[*it1].m * x + lines[*it1].c;
}
void init() {
	lines_count = 0;
	active.clear();
	active.insert(new_line(-inf, -inf));
	active.insert(new_line(inf, inf));
}

int n, money, days, day[MAX], buy[MAX], sell[MAX], profit[MAX], sorted[MAX];
int main() {
	int T = 1;
	while(scanf("%d%d%d", &n, &money, &days), n) {
		init();
		for(int i = 1; i <= n; ++i)
			scanf("%d%d%d%d", day + i, buy + i, sell + i, profit + i);
		day[0] = -1;
		day[n + 1] = days + 1;
		iota(sorted, sorted + n + 2, 0);
		sort(sorted, sorted + n + 2, [] (int a, int b) { return day[a] < day[b]; }); 

		add_line(0, money);
		for(int i = 1; i <= n; ++i) {
			long long cur = get_optimal(day[sorted[i]]);
			if(cur >= buy[sorted[i]]) {
				add_line(profit[sorted[i]], cur - buy[sorted[i]] + sell[sorted[i]] - 1LL * (day[sorted[i]] + 1) * profit[sorted[i]]);
			}
		}
		printf("Case %d: %lld\n", T++, get_optimal(days + 1));
		
	}
	return 0;
}
