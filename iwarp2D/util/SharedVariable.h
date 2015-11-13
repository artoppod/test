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


	//�蓮�Ń��b�N����ۂɎg���B������set get�𕹗p���Ă��܂��ƃf�b�h���b�N����̂Œ���
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

	//���b�N���Ă���ĂԂ���
	T & access(){
		return val;
	}
};

