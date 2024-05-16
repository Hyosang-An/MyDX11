#pragma once

template<typename T>
class CSingleton
{
private:
	static T* g_Inst;

public:
	static T* GetInst()
	{
		if (g_Inst == nullptr)
		{
			g_Inst = new T;
		}

		return g_Inst;
	}

	static void Destroy()
	{
		if (g_Inst)
		{
			delete g_Inst;
			g_Inst = nullptr;
		}
	}

protected:
	CSingleton() {}
	virtual ~CSingleton() { Destroy(); }

	CSingleton(const CSingleton& _other) = delete;
};

template<typename T>
T* CSingleton<T>::g_Inst = nullptr;