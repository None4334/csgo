template <class T, size_t StackSize = 0>
class TDynamicStack {
public:

    TDynamicStack() {
        RtlZeroMemory(StaticArray, sizeof(T) * StackSize);
        RtlZeroMemory(ExchangeArray, sizeof(T) * StackSize);
    }

    T* add_back(const T& Value) {

        if (Size >= StackSize)
            return last();

        RtlCopyMemory(&StaticArray[Size], &Value, sizeof(T));
        Size += 1;
        return last();
    }

    T* add_front(const T& Value) {

        if (Size >= StackSize)
            return first();

        if (Size) {
            RtlCopyMemory(ExchangeArray, StaticArray, sizeof(T) * Size);
            RtlCopyMemory(&StaticArray[1], ExchangeArray, (sizeof(T) * Size));
        }

        RtlCopyMemory(&StaticArray[0], &Value, sizeof(T));
        Size += 1;
        return first();
    }

    void pop_front() {

        if (!Size)
            return;

        if (Size == 1) {
            clear();
            return;
        }
        else { 
              
            RtlCopyMemory(ExchangeArray, StaticArray, sizeof(T) * Size);
            RtlCopyMemory(StaticArray, &ExchangeArray[1], (sizeof(T) * Size));

            Size -= 1;
        }

    }

    void pop_back() {

        if (!Size)
            return;

        RtlZeroMemory(&StaticArray[Size - 1], sizeof T);
        Size -= 1;

    }


    T* at(INT Index) {
        if (Index >= Size || Index < 0)
            return NULL;

        return &StaticArray[Index];
    }

    T* last() {

        if (!Size)
            return NULL;

        return at(Size - 1);
    }


    T* first() {

        if (!Size)
            return NULL;

        return at(0);
    }

    T* operator[](INT Index)
    {
        return at(Index);
    }

    void clear() {

        if (!Size)
            return;


        RtlZeroMemory(StaticArray, sizeof(T) * Size);
        RtlZeroMemory(ExchangeArray, sizeof(T) * Size);
        Size = 0;
    }

    size_t max_size()
    {
        return StackSize;
    }

    size_t size()
    {
        return Size;
    }

    T* Base() {
        return StaticArray;
    }

    bool full() {
        return Size >= StackSize;
    }

    bool empty() {
        return !size();
    }

    void copy_to(TDynamicStack<T, StackSize>& Other) {

        Other.clear();

        if (!Size)
            return;

        Other.set_size(Size);

        RtlCopyMemory(Other.Base(), StaticArray, sizeof(T) * Size);
    }


private:


    void set_size(size_t OtherSize)
    {
        Size = OtherSize;
    }

protected:
    T StaticArray[StackSize];
    T ExchangeArray[StackSize];
    size_t Size = 0;
};
