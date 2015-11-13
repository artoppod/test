#pragma once
#include <cassert>
#include <cstring>
#ifdef _DEBUG
#include <list>
#endif

template <class T, int N>
struct List
{
	int size;
	int offset;
	T pool[N];
	List() : offset(0), size(0)
	{
	}

	~List(){
	}

	T & operator [](int i){
		assert(0<=i&&i<N);
		return pool[(offset + i) % N];
	}
	
	const T & operator [](int i) const{
		assert(0 <= i&&i<N);
		return pool[(offset + i) % N];
	}


	int getsize(){ return size; }

	void push_back(T & param)
	{
		assert(size < N);
		pool[(offset + size) % N] = param;
		size++;
	}

	void push(T & param, int i){
		assert(size < N && 0 <= i&&i <= size);
		if ((i << 1)<size){
			offset=(offset-1+N)%N;
			for (int j=0; j<i; j++){
				pool[(offset + j) % N] = pool[(offset + j + 1) % N];
			}
		}
		else{
			for (int j=size; j>i; j--){
				pool[(offset + j + N) % N] = pool[(offset + j - 1 + N) % N];
			}
		}
		pool[(offset + i) % N] = param;
		size++;
	}
	
	void pop(int i){
		assert(size > 0 && 0 <= i&&i < size);
		if ((i<<1)<size){
			for (; i>0; i--){
				pool[(offset + i + N) % N] = pool[(offset + i - 1 + N) % N];
			}
			offset++;
		}
		else{
			for (i++; i<size; i++){
				pool[(offset + i - 1) % N] = pool[(offset + i) % N];
			}
		}
		size--;
	}

	void push_fast(T & param, int i){
		assert(size < N && 0 <= i&&i <= size);
		pool[(offset + size) % N] = pool[(offset + i) % N];
		pool[(offset + i) % N] = param;
		size++;
	}

	void pop_fast(int i){
		assert(size > 0 && 0 <= i&&i < size);
		size--;
		pool[(offset + i) % N] = pool[(offset + size) % N];
	}

	T & top(){
		return pool[offset];
	}

	void clear(){
		offset = size = 0;
	}

	static void test(){
#ifdef _DEBUG
		List<int, N> l;
		std::list<int> sl;
		for (int i = 0; i < N; i++){
			int n = ((unsigned)rand()) % (1+i);
			l.push(i, n);
			std::list<int>::iterator it = sl.begin();
			int j = 0;
			for (it = sl.begin(); it != sl.end() && j < n; j++, it++);
			sl.insert(it, i);
		}

		//for (int i = 0; i < N; i++){
		//	std::list<int>::iterator it = sl.begin();
		//	int j = 0;
		//	for (it = sl.begin(); it != sl.end() && j < i; j++, it++);
		//	printf("%d, %d\n", l[i], *it);
		//}
		for (;sl.size()>0;){
			{
				std::list<int>::iterator it = sl.begin();
				int i = 0;
				for (; it != sl.end(); i++, it++) {
					assert(l[i]==*it);
				}
			}
			int n = rand()%sl.size();
			l.pop(n);
			{
				std::list<int>::iterator it;
				int j = 0;
				for (it = sl.begin(); it != sl.end() && j < n; j++, it++);
				sl.erase(it);
			}
		}
#endif
	}
};
