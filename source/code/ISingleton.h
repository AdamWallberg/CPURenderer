#pragma once
#include "Core.h"


template<class T> 
class ISingleton
{
private:
	static T* instance_;
public:
	static T* createInstance()
	{
		if (instance_)
		{
			assert(false);
		}
		return instance_ = newp T;
	}

	static void destroyInstance()
	{
		if (!instance_)
		{
			assert(false);
		}

		delete instance_;
		instance_ = nullptr;
	}

	static T* get()
	{
		if (!instance_)
		{
			assert(false);
		}
		return instance_;
	}
};

template <class T>
T* ISingleton<T>::instance_ = nullptr;