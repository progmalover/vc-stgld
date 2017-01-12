// Singleton.h: interface for the CSingleton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLETON_H__42F70226_2EA6_4554_80A0_5D5812A8D15D__INCLUDED_)
#define AFX_SINGLETON_H__42F70226_2EA6_4554_80A0_5D5812A8D15D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

template<class T> class CSingleton  
{
	static T* m_ptrInstance;
public:
	static T* Instance()
	{
		if(!m_ptrInstance)
			m_ptrInstance = new T();
		return m_ptrInstance;
	}
	static T* Instance(T* ptrObject)
	{
		if(ptrObject!=m_ptrInstance)
		{
			if(m_ptrInstance)
			{
				delete m_ptrInstance;
				m_ptrInstance = NULL;
			}
		}
		return m_ptrInstance = ptrObject;
	}
	static void ReleaseInstance()
	{
		if(m_ptrInstance)
		{
			delete m_ptrInstance;
			m_ptrInstance = 0;
		}
	}
};

template<class T>
T* CSingleton<T>::m_ptrInstance = 0;


#endif // !defined(AFX_SINGLETON_H__42F70226_2EA6_4554_80A0_5D5812A8D15D__INCLUDED_)
