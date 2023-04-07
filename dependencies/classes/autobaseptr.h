
#define NUM_ENT_ENTRY_BITS         (11 + 2)
#define NUM_ENT_ENTRIES            (1 << NUM_ENT_ENTRY_BITS)
#define INVALID_EHANDLE_INDEX       0xFFFFFFFF
#define NUM_SERIAL_NUM_BITS        16 
#define NUM_SERIAL_NUM_SHIFT_BITS (32 - NUM_SERIAL_NUM_BITS)
#define ENT_ENTRY_MASK             (( 1 << NUM_SERIAL_NUM_BITS) - 1)

class IRefCounted
{
public:
	virtual int AddReference() = 0;
	virtual int Release() = 0;
};

class CRefCounted
{
public:
	virtual void Destructor(char bDelete) = 0;
	virtual bool OnFinalRelease() = 0;

	void Release()
	{
		if (InterlockedDecrement(&vlRefCount) == 0 && OnFinalRelease())
			Destructor(1);
	}

private:
	volatile long vlRefCount;
};

template <class T>
class CBaseAutoPtr
{
public:
	CBaseAutoPtr() :
		pObject(nullptr) { }

	CBaseAutoPtr(T* pObject) :
		pObject(pObject) { }

	operator const void* () const
	{
		return pObject;
	}

	operator void* () const
	{
		return pObject;
	}

	operator const T* () const
	{
		return pObject;
	}

	operator const T* ()
	{
		return pObject;
	}

	operator T* ()
	{
		return pObject;
	}

	int	operator=(int i)
	{
		pObject = nullptr;
		return 0;
	}

	T* operator=(T* pSecondObject)
	{
		pObject = pSecondObject;
		return pSecondObject;
	}

	bool operator!() const
	{
		return (!pObject);
	}

	bool operator==(const void* pSecondObject) const
	{
		return (pObject == pSecondObject);
	}

	bool operator!=(const void* pSecondObject) const
	{
		return (pObject != pSecondObject);
	}

	bool operator==(T* pSecondObject) const
	{
		return operator==(static_cast<void*>(pSecondObject));
	}

	bool operator!=(T* pSecondObject) const
	{
		return operator!=(static_cast<void*>(pSecondObject));
	}

	bool operator==(const CBaseAutoPtr<T>& pSecondPtr) const
	{
		return operator==(static_cast<const void*>(pSecondPtr));
	}

	bool operator!=(const CBaseAutoPtr<T>& pSecondPtr) const
	{
		return operator!=(static_cast<const void*>(pSecondPtr));
	}

	T* operator->()
	{
		return pObject;
	}

	T& operator*()
	{
		return *pObject;
	}

	T** operator&()
	{
		return &pObject;
	}

	const T* operator->() const
	{
		return pObject;
	}

	const T& operator*() const
	{
		return *pObject;
	}

	T* const* operator&() const
	{
		return &pObject;
	}

protected:
	CBaseAutoPtr(const CBaseAutoPtr<T>& pSecondPtr) :
		pObject(pSecondPtr.pObject) { }

	void operator=(const CBaseAutoPtr<T>& pSecondPtr)
	{
		pObject = pSecondPtr.pObject;
	}

	T* pObject;
};

template <class T>
class CRefPtr : public CBaseAutoPtr<T>
{
	typedef CBaseAutoPtr<T> CBaseClass;
public:
	CRefPtr() { }

	CRefPtr(T* pInit)
		: CBaseClass(pInit) { }

	CRefPtr(const CRefPtr<T>& pRefPtr)
		: CBaseClass(pRefPtr) { }

	~CRefPtr()
	{
		if (CBaseClass::pObject != nullptr)
			CBaseClass::pObject->Release();
	}

	void operator=(const CRefPtr<T>& pSecondRefPtr)
	{
		CBaseClass::operator=(pSecondRefPtr);
	}

	int operator=(int i)
	{
		return CBaseClass::operator=(i);
	}

	T* operator=(T* pSecond)
	{
		return CBaseClass::operator=(pSecond);
	}

	operator bool() const
	{
		return !CBaseClass::operator!();
	}

	operator bool()
	{
		return !CBaseClass::operator!();
	}

	void SafeRelease()
	{
		if (CBaseClass::pObject != nullptr)
			CBaseClass::pObject->Release();

		CBaseClass::pObject = nullptr;
	}

	void AssignAddReference(T* pFrom)
	{
		if (pFrom != nullptr)
			pFrom->AddReference();

		SafeRelease();
		CBaseClass::pObject = pFrom;
	}

	void AddReferenceAssignTo(T*& pTo)
	{
		if (CBaseClass::pObject != nullptr)
			CBaseClass::pObject->AddReference();

		SafeRelease(pTo);
		pTo = CBaseClass::pObject;
	}
};


class IHandleEntity;
class CBaseHandle2
{
public:
	CBaseHandle2();
	CBaseHandle2(const CBaseHandle2& other);
	CBaseHandle2(unsigned long value);
	CBaseHandle2(int iEntry, int iSerialNumber);

	void Init(int iEntry, int iSerialNumber);
	void Term();
	bool IsValid() const;

	int GetEntryIndex() const;
	int GetSerialNumber() const;

	int ToInt() const;
	bool operator !=(const CBaseHandle2& other) const;
	bool operator ==(const CBaseHandle2& other) const;
	bool operator ==(const IHandleEntity* pEnt) const;
	bool operator !=(const IHandleEntity* pEnt) const;
	bool operator <(const CBaseHandle2& other) const;
	bool operator <(const IHandleEntity* pEnt) const;
	const CBaseHandle2& operator=(const IHandleEntity* pEntity);
	const CBaseHandle2& Set(const IHandleEntity* pEntity);
	IHandleEntity* Get() const;

protected:
	unsigned long	m_Index;
};

template< class T >
class CHandle : public CBaseHandle2
{
public:

	CHandle();
	CHandle(int iEntry, int iSerialNumber);
	CHandle(const CBaseHandle2& handle);
	CHandle(T* pVal);

	static CHandle<T> FromIndex(int index);

	T* Get() const;
	void	Set(const T* pVal);

	operator T* ();
	operator T* () const;

	bool	operator !() const;
	bool	operator==(T* val) const;
	bool	operator!=(T* val) const;
	const CBaseHandle2& operator=(const T* val);

	T* operator->() const;
};