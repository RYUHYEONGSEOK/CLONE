#pragma once

//safe_delete
#define SAFE_DELETE(x) if( x != nullptr ) { delete x; x = nullptr; } 
#define SAFE_DELETE_ARR(x) if( x != nullptr ) { delete [] x; x = nullptr;}

//singleton
#define NO_COPY(CLASSNAME)							\
	private:										\
	CLASSNAME(const CLASSNAME&);					\
	CLASSNAME& operator = (const CLASSNAME&);		
#define DECLARE_SINGLETON(CLASSNAME)				\
	NO_COPY(CLASSNAME)								\
	private:										\
	static CLASSNAME*	m_pInstance;				\
	public:											\
	static CLASSNAME*	GetInstance( void );		\
	static void DestroyInstance( void );			
#define IMPLEMENT_SINGLETON(CLASSNAME)				\
	CLASSNAME*	CLASSNAME::m_pInstance = nullptr;	\
	CLASSNAME*	CLASSNAME::GetInstance( void )	{	\
	if(m_pInstance == nullptr) {					\
	m_pInstance = new CLASSNAME;					\
	}												\
	return m_pInstance;								\
	}												\
	void CLASSNAME::DestroyInstance( void ) {		\
	if(m_pInstance != nullptr)	{					\
	delete m_pInstance;								\
	m_pInstance = nullptr;							\
	}												\
	}