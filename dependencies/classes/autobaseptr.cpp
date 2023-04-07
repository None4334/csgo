#include "../../includes.h"
#include "../../main/sdk.h"

CBaseHandle2::CBaseHandle2()
{
	m_Index = INVALID_EHANDLE_INDEX;
}

CBaseHandle2::CBaseHandle2(const CBaseHandle2& other)
{
	m_Index = other.m_Index;
}

CBaseHandle2::CBaseHandle2(unsigned long value)
{
	m_Index = value;
}

CBaseHandle2::CBaseHandle2(int iEntry, int iSerialNumber)
{
	Init(iEntry, iSerialNumber);
}

void CBaseHandle2::Init(int iEntry, int iSerialNumber)
{
	m_Index = iEntry | (iSerialNumber << NUM_ENT_ENTRY_BITS);
}

void CBaseHandle2::Term()
{
	m_Index = INVALID_EHANDLE_INDEX;
}

bool CBaseHandle2::IsValid() const
{
	return m_Index != INVALID_EHANDLE_INDEX;
}

int CBaseHandle2::GetEntryIndex() const
{
	return m_Index & ENT_ENTRY_MASK;
}

int CBaseHandle2::GetSerialNumber() const
{
	return m_Index >> NUM_ENT_ENTRY_BITS;
}

int CBaseHandle2::ToInt() const
{
	return (int)m_Index;
}

bool CBaseHandle2::operator !=(const CBaseHandle2& other) const
{
	return m_Index != other.m_Index;
}

bool CBaseHandle2::operator ==(const CBaseHandle2& other) const
{
	return m_Index == other.m_Index;
}

bool CBaseHandle2::operator ==(const IHandleEntity* pEnt) const
{
	return Get() == pEnt;
}

bool CBaseHandle2::operator !=(const IHandleEntity* pEnt) const
{
	return Get() != pEnt;
}

bool CBaseHandle2::operator <(const CBaseHandle2& other) const
{
	return m_Index < other.m_Index;
}

bool CBaseHandle2::operator <(const IHandleEntity* pEntity) const
{
	unsigned long otherIndex = (pEntity) ? pEntity->GetRefEHandle().m_Index : INVALID_EHANDLE_INDEX;
	return m_Index < otherIndex;
}

const CBaseHandle2& CBaseHandle2::operator=(const IHandleEntity* pEntity)
{
	return Set(pEntity);
}

IHandleEntity* CBaseHandle2::Get() const
{
	return (IHandleEntity*)TInterfaces::ClientEntityList->GetClientEntityFromHandle(*this);
}

const CBaseHandle2& CBaseHandle2::Set(const IHandleEntity* pEntity)
{
	if (pEntity) {
		*this = pEntity->GetRefEHandle();
	}
	else {
		m_Index = INVALID_EHANDLE_INDEX;
	}

	return *this;
}

template<class T>
CHandle<T>::CHandle()
{
}

template<class T>
CHandle<T>::CHandle(int iEntry, int iSerialNumber)
{
	Init(iEntry, iSerialNumber);
}

template<class T>
CHandle<T>::CHandle(const CBaseHandle2& handle)
	: CBaseHandle2(handle)
{
}


template<class T>
CHandle<T>::CHandle(T* pObj)
{
	Term();
	Set(pObj);
}


template<class T>
CHandle<T> CHandle<T>::FromIndex(int index)
{
	CHandle<T> ret;
	ret.m_Index = index;
	return ret;
}


template<class T>
T* CHandle<T>::Get() const
{
	return (T*)CBaseHandle2::Get();
}


template<class T>
CHandle<T>::operator T* ()
{
	return Get();
}

template<class T>
CHandle<T>::operator T* () const
{
	return Get();
}


template<class T>
bool CHandle<T>::operator !() const
{
	return !Get();
}

template<class T>
bool CHandle<T>::operator==(T* val) const
{
	return Get() == val;
}

template<class T>
bool CHandle<T>::operator!=(T* val) const
{
	return Get() != val;
}

template<class T>
void CHandle<T>::Set(const T* pVal)
{
	CBaseHandle2::Set(reinterpret_cast<const IHandleEntity*>(pVal));
}

template<class T>
const CBaseHandle2& CHandle<T>::operator=(const T* val)
{
	Set(val);
	return *this;
}

template<class T>
T* CHandle<T>::operator -> () const
{
	return Get();
}