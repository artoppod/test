#pragma once
#include <Windows.h>

template <class T>
class SharedVariable
{
	SRWLOCK lock;
	T val;
public:
	SharedVariable()
	{
		InitializeSRWLock(&lock);
	};
	SharedVariable(T& v)
	{
		val = v;
		InitializeSRWLock(&lock);
	};
	~SharedVariable(){};

	void set(T a){
		AcquireSRWLockExclusive(&lock);
		val = a;
		ReleaseSRWLockExclusive(&lock);
	}
	
	T get(){
		AcquireSRWLockShared(&lock);
		T a = val;
		ReleaseSRWLockShared(&lock);
		return a;
	}


	//手動でロックする際に使う。これらとset getを併用してしまうとデッドロックするので注意
	void lockRead(){
		AcquireSRWLockShared(&lock);
	}
	void unlockRead(){
		ReleaseSRWLockShared(&lock);
	}

	void lockWrite(){
		AcquireSRWLockExclusive(&lock);
	}
	void unlockWrite(){
		ReleaseSRWLockExclusive(&lock);
	}

	//ロックしてから呼ぶこと
	T & access(){
		return val;
	}
};

