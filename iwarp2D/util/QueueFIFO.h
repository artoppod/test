#pragma once
#include <cassert>



template <class T>
struct Queue{
	int size=0;
	virtual void push(T & param) = 0;
	virtual void pop() = 0;
	virtual T & top() = 0;
	virtual void clear() = 0;
	virtual int getsize() = 0;
};

template <class T, int N>
struct QueueFIFO : public Queue<T>
{
	int offset;
	T data[N];
	QueueFIFO() : offset(0)
	{
	}

	~QueueFIFO(){
	}

	int getsize(){ return size; }

	void push_back(T & param)
	{
		assert(size < N);
		data[(offset + size) % N] = param;
		size++;
	}

	void push(T & param){
		push_back(param);
	}

	void pop_front()
	{
		assert(size>0);
		int tmp;
		tmp = offset;
		offset = (offset + 1) % N;
		size--;
	}

	void pop(){
		pop_front();
	}

	T & top(){
		return data[offset];
	}

	void clear(){
		offset = size = 0;
	}
};
