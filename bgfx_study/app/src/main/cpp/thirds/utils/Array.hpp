//
// Created by Administrator on 2020/11/20 0020.
//

#include <malloc.h>
#include <lua/MathUtils.h>

namespace h7{
    template<typename T>
    class Comparator{
    public:
        Comparator(){}
        int compare(T& t1, T& t2){
            return t1 == t2;
        }
    };
    class DoubleComparator: public Comparator<double>{
        int compare(double& t1, double& t2){
            return MathUtils::doubleEqual(t1, t2);
        }
    };
    static const Comparator<double> COM_double = DoubleComparator();

    template<typename T>
    class Array{
    private:
        size_t unitSize; // data size as bytes
        void * data;

        size_t malCount; // memory alloc count
        size_t size;     // element count

        const Comparator<T>* _com;

    public:
        Array(size_t unitSize,  size_t capacity, const Comparator<T>* com): unitSize(unitSize),malCount(capacity), _com(com){
            data = malloc(unitSize * capacity);
            size = 0;
        }
        Array(size_t unitSize, const Comparator<T>* com): Array(unitSize, 16, com){}
        ~Array(){
            if(data){
                free(data);
                data = NULL;
            }
        }
        inline bool add(const T& val){
            return add(size, val);
        }
        bool add(int index, const T& val){
            if(index < 0){
                return false;
            }
            if(size == malCount){
                if(!resize(MathUtils::max(8, (int)(malCount * 1.75f)))){
                    return false;
                }
            }
            unsigned char* addr = reinterpret_cast<unsigned char*>(data);
            addr += index * unitSize;
            memcpy(addr, &val, unitSize);
            size ++;
            return true;
        }

        bool add(const T& val1, const T& val2){
            if(size >= malCount - 1){
                if(!resize(MathUtils::max(8, (int)(malCount * 1.75f)))){
                    return false;
                }
            }
            int unit = unitSize;
            unsigned char* addr = reinterpret_cast<unsigned char*>(data);
            addr += size * unit;
            memcpy(addr, &val1, unit);
            addr += unit;
            memcpy(addr, &val2, unit);
            size += 2;
            return true;
        }
        bool add(const T& val1, const T& val2, const T& val3){
            if(size >= malCount - 2){
                if(!resize(MathUtils::max(8, (int)(malCount * 1.75f)))){
                    return false;
                }
            }
            int unit = unitSize;
            unsigned char* addr = reinterpret_cast<unsigned char*>(data);
            addr += size * unit;
            memcpy(addr, val1, unit);
            addr += unit;
            memcpy(addr, val2, unit);
            addr += unit;
            memcpy(addr, val3, unit);
            size += 3;
            return true;
        }
        bool addAll(const Array<T>* array){
            if(size >= malCount - array->size){
                if(!resize(MathUtils::max(8, (int)((malCount + array->size) * 1.75f)))){
                    return false;
                }
            }
            int unit = unitSize;
            unsigned char* addr = reinterpret_cast<unsigned char*>(data);
            addr += size * unit;
            memcpy(addr, array->data, unit * array->size);
            size += array->size;
            return true;
        }
        const T& get(size_t index){
            int unit = unitSize;
            unsigned char* addr = reinterpret_cast<unsigned char*>(data);
            addr += size * unit;
            return *((T*)addr);
        }
        void set(size_t index, const T& val){
            int unit = unitSize;
            unsigned char* addr = reinterpret_cast<unsigned char*>(data);
            addr += size * unit;
            *((T*)addr) = val;
        }
        void swap(size_t first, size_t second){
            int unit = unitSize;
            unsigned char* addr1 = reinterpret_cast<unsigned char*>(data);
            unsigned char* addr2 = reinterpret_cast<unsigned char*>(data);
            addr1 += unit * first;
            addr2 += unit * second;
            T t1 = *((T*)addr1);
            *((T*)addr1) = *((T*)addr2)
            *((T*)addr2) = t1;
        }
        inline bool contains(const T& val){
            return indexOf(val) >= 0;
        }
        int indexOf(const T& val){
            int unit = unitSize;
            unsigned char* addr = reinterpret_cast<unsigned char*>(data);
            for (int i = 0; i < size; ++i) {
                if(_com == NULL){
                    if( (*((T*)addr) == val)){
                        return i;
                    }
                } else if(_com->compare(*((T*)addr), val) == 0){
                    return i;
                }
                addr += unit;
            }
            return -1;
        }
        int lastIndexOf(const T& val){
            int unit = unitSize;
            unsigned char* addr = reinterpret_cast<unsigned char*>(data);
            addr += unit * (size - 1);
            for (int i = size - 1; i >= 0; i--) {
                if(_com == NULL){
                    if( (*((T*)addr) == val)){
                        return i;
                    }
                } else if(_com->compare(*((T*)addr), val) == 0){
                    return i;
                }
                addr -= unit;
            }
            return -1;
        }
        bool remove(const T& val){
            auto i = indexOf(val, _com);
            removeAt(i);
            return i >= 0;
        }

        const T& removeAt(int index){
            if(index >= size || index < 0){
                return NULL;
            }
            int unit = unitSize;
            unsigned char* addr = reinterpret_cast<unsigned char*>(data);
            addr += index * unit;
            T val = *((T*)addr);

            for (int i = 0, c = size - index - 1; i < c; ++i) {
                unsigned char* addr_src = reinterpret_cast<unsigned char*>(data);
                unsigned char* addr_dst = reinterpret_cast<unsigned char*>(data);
                addr_src += (index + i + 1) * unit;
                addr_dst += (index + i)* unit;
                *((T*)addr_dst) = *((T*)addr_src);
            }
            size -- ;
            return val;
        }
        bool removeAll(Array<T>* array){
            bool changed = false;
            for (int i = 0; i < array->size; ++i) {
                changed |= remove(array->get((size_t)i));
            }
            return changed;
        }
        inline bool isEmpty(){
            return size == 0;
        }
        inline const T& pop(){
            return size > 0 ? removeAt(size - 1) : NULL;
        }
        inline bool push(const T& val){
            return add(val);
        }
        inline bool addFirst(const T& val){
            return add(0, val);
        }
        inline bool addLast(const T& val){
            return add(size, val);
        }
        inline const T& removeFirst(){
            return removeAt(0);
        }
        inline const T& removeLast(){
            return removeAt(size - 1);
        }
        inline const T& pollFirst(){
            return removeAt(0);
        }
        inline const T& pollLast(){
            return removeAt(size - 1);
        }
        inline const T& peek(){
            return size > 0 ? get(size - 1) : NULL;
        }
        inline const T& peekFirst(){
            return size > 0 ? get(0) : NULL;
        }
        inline const T& peekLast(){
            return size > 0 ? get(size - 1) : NULL;
        }
        inline void clear(){
            size = 0;
        }
        /** Reduces the size of the backing array to the size of the actual items. This is useful to release memory when many items
 * have been removed, or if it is known that more items will not be added.
         * */
        bool shrink(){
            if(size > 0 && malCount != size){
                return resize(size);
            }
            return true;
        }
        bool ensureCapacity(int additionalCapacity){
            int sizeNeeded = size + additionalCapacity;
            if(sizeNeeded > malCount){
                return resize(sizeNeeded);
            }
            return true;
        }
        void setSize(int newSize){
            size = newSize;
        }

    protected:

        inline bool resize(size_t newSize) {
            void * d = malloc(newSize * unitSize);
            if(d == NULL){
                return false;
            }
            memcpy(d, data, sizeof(data));
            free(data);
            this->data = d;
            this->malCount = newSize;
            return true;
        }
    };

#define TYPE_ARRAY(cn, type, com)\
    class cn: public Array<type>{\
    public:\
        cn(size_t unitSize, size_t capacity): Array(unitSize, capacity, com) {}\
        cn(size_t unitSize) : Array(unitSize, com) {}\
    };
    TYPE_ARRAY(IntArray, int, NULL)
    TYPE_ARRAY(UnSignedIntArray, unsigned int, NULL)
    TYPE_ARRAY(ShortArray, short, NULL)
    TYPE_ARRAY(UnSignedShortArray, unsigned short, NULL)
    TYPE_ARRAY(UnSignedCharArray, unsigned char, NULL)
    TYPE_ARRAY(CharArray, char, NULL)

    TYPE_ARRAY(UnSignedLongArray, unsigned long long, NULL)
    TYPE_ARRAY(LongArray, long long, NULL)
    TYPE_ARRAY(FloatArray, float, NULL)
    TYPE_ARRAY(DoubleArray, double, &COM_double)
    TYPE_ARRAY(BoolArray, bool, NULL)
}
