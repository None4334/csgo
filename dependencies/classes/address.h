
class TAddress {
protected:
    uintptr_t m_addr;

public:

    __forceinline  TAddress() : m_addr{} {};
    __forceinline ~TAddress() {};

    __forceinline TAddress(uintptr_t a) : m_addr{ a } {}
    __forceinline TAddress(const void* a) : m_addr{ (uintptr_t)a } {}

    __forceinline operator uintptr_t() { return m_addr; }
    __forceinline operator void* () { return (void*)m_addr; }
    __forceinline operator const void* () { return (const void*)m_addr; }

    __forceinline bool operator==(TAddress a) const {
        return as< uintptr_t >() == a.as< uintptr_t >();
    }
    __forceinline bool operator!=(TAddress a) const {
        return as< uintptr_t >() != a.as< uintptr_t >();
    }

    template< typename t = TAddress >
    __forceinline t as() const {
        return (m_addr) ? (t)m_addr : t{};
    }

    template< typename t = TAddress >
    __forceinline t as(size_t offset) const {
        return (m_addr) ? (t)(m_addr + offset) : t{};
    }

    template< typename t = TAddress >
    __forceinline t as(ptrdiff_t offset) const {
        return (m_addr) ? (t)(m_addr + offset) : t{};
    }

    template< typename t = TAddress >
    __forceinline t at(size_t offset) const {
        return (m_addr) ? *(t*)(m_addr + offset) : t{};
    }

    template< typename t = TAddress >
    __forceinline t at(ptrdiff_t offset) const {
        return (m_addr) ? *(t*)(m_addr + offset) : t{};
    }

    template< typename t = TAddress >
    __forceinline t add(size_t offset) const {
        return (m_addr) ? (t)(m_addr + offset) : t{};
    }

    template< typename t = TAddress >
    __forceinline t add(ptrdiff_t offset) const {
        return (m_addr) ? (t)(m_addr + offset) : t{};
    }

    template< typename t = TAddress >
    __forceinline t sub(size_t offset) const {
        return (m_addr) ? (t)(m_addr - offset) : t{};
    }

    template< typename t = TAddress >
    __forceinline t sub(ptrdiff_t offset) const {
        return (m_addr) ? (t)(m_addr - offset) : t{};
    }

    template< typename t = TAddress >
    __forceinline t to() const {
        return *(t*)m_addr;
    }

    template< typename t = TAddress >
    __forceinline t get(size_t n = 1) {
        uintptr_t out;

        if (!m_addr)
            return t{};

        out = m_addr;

        for (size_t i{ n }; i > 0; --i) {

            if (!valid(out))
                return t{};

            out = *(uintptr_t*)out;
        }

        return (t)out;
    }

    template< typename t = TAddress >
    __forceinline t rel8(size_t offset) {
        uintptr_t   out;
        uint8_t     r;

        if (!m_addr)
            return t{};

        out = m_addr + offset;

        r = *(uint8_t*)out;
        if (!r)
            return t{};

        if (r < 128)
            out = (out + 1) + r;
        else
            out = (out + 1) - (uint8_t)(~r + 1);

        return (t)out;
    }

    template< typename t = TAddress >
    __forceinline t rel32(size_t offset) {
        uintptr_t   out;
        uint32_t    r;

        if (!m_addr)
            return t{};

        out = m_addr + offset;

        r = *(uint32_t*)out;
        if (!r)
            return t{};

        out = (out + 4) + r;

        return (t)out;
    }

    template< typename t = uintptr_t > __forceinline void set(const t& value) {
        if (!m_addr)
            return;

        *(t*)m_addr = value;
    }

    static __forceinline bool valid(uintptr_t addr) {
        MEMORY_BASIC_INFORMATION mbi;

        if (!addr)
            return false;

        if (!VirtualQuery((const void*)addr, &mbi, sizeof(mbi)))
            return false;

        if ((mbi.Protect & PAGE_NOACCESS) || (mbi.Protect & PAGE_GUARD))
            return false;

        return true;
    }

    template< typename t = TAddress >
    static __forceinline t RVA(TAddress base, size_t offset) {
        return base.as< t >(offset);
    }
};
