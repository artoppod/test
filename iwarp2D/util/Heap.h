#pragma once
#include <cassert>
#include "QueueFIFO.h"

template <class T, int N>
struct Heap : public Queue<T>
{
	T pool[N];
	unsigned short map[N], imap[N];
	Heap();
	~Heap();
	int getsize(){ return size; }
	void upHeapify(int i);
	void downHeapify(int i);
	void push(T & v);
	void update(T & v);
	void pop();
	bool popUnder(T & a);
	T & top();
	void clear(){ size = 0; };
	T & operator [](int i){
		return pool[map[i]];
	}
	const T & operator [](int i) const{
		return pool[map[i]];
	}
};


template <class T, int N>
Heap<T, N>::Heap(){
	assert(0 < N&&N <= USHRT_MAX);
};


template <class T, int N>
Heap<T, N>::~Heap(){
}

template <class T, int N>
void Heap<T, N>::upHeapify(int i){
	int j;
	int tmp = map[i];
	for (; (j = (i - 1) >> 1) >= 0 && pool[map[j]] < pool[tmp]; i = j){
		map[i] = map[j];
		imap[map[j]] = i;
	}
	map[i] = tmp;
	imap[tmp] = i;
}

template <class T, int N>
void Heap<T, N>::downHeapify(int i){
	int j;
	int tmp = map[i];
	for (; (j = (i << 1) + 1) < size; i = j){
		if (j + 1 < size && pool[map[j + 1]] > pool[map[j]]) j++;
		if (!(pool[map[j]] > pool[tmp])) break;
		map[i] = map[j];
		imap[map[j]] = i;
	}
	map[i] = tmp;
	imap[tmp] = i;
}

template <class T, int N>
void Heap<T, N>::push(T & v){
	if (size >= N&&!popUnder(v)){
		return;
	}
	pool[size] = v;
	map[size] = size;
	imap[size] = size;
	upHeapify(size);
	size++;
}


template <class T, int N>
void Heap<T, N>::update(T & v){
	int i = &v - pool;
	upHeapify(i);
	downHeapify(i);
}

template <class T, int N>
void Heap<T, N>::pop(){
	pool[map[0]] = pool[--size];
	map[imap[size]] = map[0];
	imap[map[0]] = imap[size];
	map[0] = map[size]; 
	imap[map[size]] = 0;
	downHeapify(0);
}


template <class T, int N>
bool Heap<T, N>::popUnder(T & a){
	T * worst=NULL;
	int j;
	for (int i = size - (size + 1) / 2; i < size; i++){
		if (worst == NULL || (*worst > pool[map[i]])){
			worst = &pool[map[i]];
			j = i;
		}
	}
	if (*worst > a){
		return false;
	}
	pool[map[j]] = pool[--size];
	map[imap[size]] = map[j];
	imap[map[j]] = imap[size];
	map[j] = map[size];
	imap[map[size]] = j;
	upHeapify(j);
	return true;
}


template <class T, int N>
T & Heap<T, N>::top(){
	assert(size>0);
	return pool[map[0]];
}


