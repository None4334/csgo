
class bf_write
{
public:
	bf_write() = default;
	unsigned long* pData;
	int				nDataBytes;
	int				nDataBits;
	int				iCurrentBit;
};

class bf_read
{
public:
	std::uintptr_t uBaseAddress;
	std::uintptr_t uCurrentOffset;

	bf_read(std::uintptr_t uAddress) : uBaseAddress(uAddress), uCurrentOffset(0U) {}

	void SetOffset(std::uintptr_t uOffset)
	{
		uCurrentOffset = uOffset;
	}

	void Skip(std::uintptr_t uLength)
	{
		uCurrentOffset += uLength;
	}

	int ReadByte()
	{
		char dValue = *reinterpret_cast<char*>(uBaseAddress + uCurrentOffset);
		++uCurrentOffset;
		return dValue;
	}

	bool ReadBool()
	{
		bool bValue = *reinterpret_cast<bool*>(uBaseAddress + uCurrentOffset);
		++uCurrentOffset;
		return bValue;
	}

	const char* ReadString()
	{
		char szBuffer[256];
		char chLength = *reinterpret_cast<char*>(uBaseAddress + uCurrentOffset);
		++uCurrentOffset;
		memcpy(szBuffer, reinterpret_cast<void*>(uBaseAddress + uCurrentOffset), chLength > 255 ? 255 : chLength);
		szBuffer[chLength > 255 ? 255 : chLength] = '\0';
		uCurrentOffset += chLength + 1;
		return szBuffer;
	}
};
#define LOG2_BITS_PER_INT	5
#define BITS_PER_INT		32

#define BitVec_Int( bitNum ) ( (bitNum) >> LOG2_BITS_PER_INT )
#define BitVec_Bit( bitNum ) ( 1 << ( (bitNum) & (BITS_PER_INT-1) ) )

inline unsigned GetEndMask(int numBits)
{
	static unsigned bitStringEndMasks[] =
	{
		0xffffffff,
		0x00000001,
		0x00000003,
		0x00000007,
		0x0000000f,
		0x0000001f,
		0x0000003f,
		0x0000007f,
		0x000000ff,
		0x000001ff,
		0x000003ff,
		0x000007ff,
		0x00000fff,
		0x00001fff,
		0x00003fff,
		0x00007fff,
		0x0000ffff,
		0x0001ffff,
		0x0003ffff,
		0x0007ffff,
		0x000fffff,
		0x001fffff,
		0x003fffff,
		0x007fffff,
		0x00ffffff,
		0x01ffffff,
		0x03ffffff,
		0x07ffffff,
		0x0fffffff,
		0x1fffffff,
		0x3fffffff,
		0x7fffffff,
	};

	return bitStringEndMasks[numBits % BITS_PER_INT];
}

template <typename BITCOUNTTYPE>
class CVarBitVecBase
{
public:
	uint32_t* Base() { return m_pInt; }
	const uint32_t* Base() const { return m_pInt; }

	inline int CalcNumIntsForBits(int numBits) { return (numBits + (BITS_PER_INT - 1)) / BITS_PER_INT; }
	int 	GetNumDWords() const { return m_numInts; }
	int		GetNumBits(void) const { return m_numBits; }

	unsigned	GetEndMask() const { return ::GetEndMask(GetNumBits()); }
	inline void FreeInts(void)
	{
		if (m_numInts > 1)
		{
			free(m_pInt);
		}
		m_pInt = NULL;
	}

	inline void ReallocInts(int numInts)
	{
		if (numInts == 0)
		{
			FreeInts();
			return;
		}

		if (m_pInt == &m_iBitStringStorage)
		{
			if (numInts != 1)
			{
				m_pInt = ((uint32_t*)malloc(numInts * sizeof(int)));
				*m_pInt = m_iBitStringStorage;
			}

			return;
		}

		if (numInts == 1)
		{
			m_iBitStringStorage = *m_pInt;
			free(m_pInt);
			m_pInt = &m_iBitStringStorage;
			return;
		}

		m_pInt = (uint32_t*)realloc(m_pInt, numInts * sizeof(int));
	}

	inline void AllocInts(int numInts)
	{
		if (numInts == 0)
			return;

		if (numInts == 1)
		{
			m_pInt = &m_iBitStringStorage;
			return;
		}

		m_pInt = (uint32_t*)malloc(numInts * sizeof(int));
	}

	inline void Resize(int resizeNumBits, bool bClearAll = false)
	{
		int newIntCount = CalcNumIntsForBits(resizeNumBits);
		if (newIntCount != GetNumDWords())
		{
			if (Base())
			{
				ReallocInts(newIntCount);
				if (!bClearAll && resizeNumBits >= GetNumBits())
				{
					Base()[GetNumDWords() - 1] &= GetEndMask();
					memset(Base() + GetNumDWords(), 0, (newIntCount - GetNumDWords()) * sizeof(int));
				}
			}
			else
			{
				AllocInts(newIntCount);
				bClearAll = true;
			}

			m_numInts = newIntCount;
		}
		else if (!bClearAll && resizeNumBits >= GetNumBits() && Base())
		{
			Base()[GetNumDWords() - 1] &= GetEndMask();
		}

		if (bClearAll && Base())
		{
			memset(Base(), 0, newIntCount * sizeof(int));
		}

		m_numBits = resizeNumBits;
	}

	inline void ClearAll(void)
	{
		if (this->Base())
			memset(this->Base(), 0, this->GetNumDWords() * sizeof(int));
	}

	inline void FreeAllocation() {
		if (this->Base())
			free(this->Base());
	}

private:

	BITCOUNTTYPE	m_numBits;
	BITCOUNTTYPE	m_numInts;
	uint32_t			m_iBitStringStorage;
	uint32_t* m_pInt;

};

template <class BASE_OPS>
class CBitVecT : public BASE_OPS
{
public:
	
	inline void Set(int bitNum)
	{
		uint32_t* pInt = this->Base() + BitVec_Int(bitNum);
		*pInt |= BitVec_Bit(bitNum);
	}

};

template< class CountType = unsigned short >
class CVarBitVecT : public CBitVecT< CVarBitVecBase< CountType > >
{
public:

};

class CVarBitVec : public CVarBitVecT<unsigned short>
{
public:

};

