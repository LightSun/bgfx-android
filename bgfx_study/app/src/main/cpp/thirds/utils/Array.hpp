//
// Created by Administrator on 2020/11/20 0020.
//

#include <malloc.h>
#include <functional>
#include <lua/MathUtils.h>

namespace h7{
    template<typename T>
    class Comparator{
    public:
        Comparator(){}
        virtual int compare(T& t1, T& t2){
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
        void * data;

        size_t malCount; // memory alloc count
        size_t _size;     // element count

        Comparator<T>* _com;

    public:
        Array(size_t capacity, const Comparator<T>* com):malCount(capacity), _com(
                const_cast<Comparator <T> *>(com)){
            data = malloc(sizeof(T) * capacity);
            _size = 0;
        }
        Array(const Comparator<T>* com): Array(16, com){}
        Array():Array(16, NULL){}
        ~Array(){
            if(data){
                free(data);
                data = NULL;
            }
        }
        inline int size(){
            return _size;
        }
        inline bool add(T& val){
            return add(_size, val);
        }
        bool add(int index, T& val){
            if(index < 0){
                return false;
            }
            if(_size == malCount){
                if(!resize(MathUtils::max(8, (int)(malCount * 1.75f)))){
                    return false;
                }
            }
            int unit = sizeof(T);
            unsigned char* addr = reinterpret_cast<unsigned char*>(data);
            addr += index * unit;
            memcpy(addr, &val, unit);
            _size ++;
            return true;
        }

        bool add(T& val1, T& val2){
            if(_size >= malCount - 1){
                if(!resize(MathUtils::max(8, (int)(malCount * 1.75f)))){
                    return false;
                }
            }
            int unit = sizeof(T);
            unsigned char* addr = reinterpret_cast<unsigned char*>(data);
            addr += _size * unit;
            memcpy(addr, &val1, unit);
            addr += unit;
            memcpy(addr, &val2, unit);
            _size += 2;
            return true;
        }
        bool add(T& val1, T& val2, T& val3){
            if(_size >= malCount - 2){
                if(!resize(MathUtils::max(8, (int)(malCount * 1.75f)))){
                    return false;
                }
            }
            int unit = sizeof(T);
            unsigned char* addr = reinterpret_cast<unsigned char*>(data);
            addr += _size * unit;
            memcpy(addr, val1, unit);
            addr += unit;
            memcpy(addr, val2, unit);
            addr += unit;
            memcpy(addr, val3, unit);
            _size += 3;
            return true;
        }
        bool addAll(Array<T>* array){
            if(_size >= malCount - array->_size){
                if(!resize(MathUtils::max(8, (int)((malCount + array->_size) * 1.75f)))){
                    return false;
                }
            }
            int unit = sizeof(T);
            unsigned char* addr = reinterpret_cast<unsigned char*>(data);
            addr += _size * unit;
            memcpy(addr, array->data, unit * array->_size);
            _size += array->_size;
            return true;
        }
        const T& get(size_t index){
            int unit = sizeof(T);
            unsigned char* addr = reinterpret_cast<unsigned char*>(data);
            addr += _size * unit;
            return *((T*)addr);
        }
        void set(size_t index, T& val){
            int unit = sizeof(T);
            unsigned char* addr = reinterpret_cast<unsigned char*>(data);
            addr += _size * unit;
            *((T*)addr) = val;
        }
        void swap(size_t first, size_t second){
            int unit = sizeof(T);
            unsigned char* addr1 = reinterpret_cast<unsigned char*>(data);
            unsigned char* addr2 = reinterpret_cast<unsigned char*>(data);
            addr1 += unit * first;
            addr2 += unit * second;
            T t1 = *((T*)addr1);
            *((T*)addr1) = *((T*)addr2)
            *((T*)addr2) = t1;
        }
        inline bool contains(T& val){
            return indexOf(val) >= 0;
        }
        int indexOf(T& val){
            int unit = sizeof(T);
            unsigned char* addr = reinterpret_cast<unsigned char*>(data);
            for (int i = 0; i < _size; ++i) {
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
        int lastIndexOf(T& val){
            int unit = sizeof(T);
            unsigned char* addr = reinterpret_cast<unsigned char*>(data);
            addr += unit * (_size - 1);
            for (int i = _size - 1; i >= 0; i--) {
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
        bool remove(T& val){
            auto i = indexOf(val);
            removeAt(i);
            return i >= 0;
        }

        const T& removeAt(int index){
            if(index >= _size || index < 0){
                return NULL;
            }
            int unit = sizeof(T);
            unsigned char* addr = reinterpret_cast<unsigned char*>(data);
            addr += index * unit;
            T val = *((T*)addr);

            for (int i = 0, c = _size - index - 1; i < c; ++i) {
                unsigned char* addr_src = reinterpret_cast<unsigned char*>(data);
                unsigned char* addr_dst = reinterpret_cast<unsigned char*>(data);
                addr_src += (index + i + 1) * unit;
                addr_dst += (index + i)* unit;
                *((T*)addr_dst) = *((T*)addr_src);
            }
            _size -- ;
            return val;
        }
        bool removeAll(Array<T>* array){
            bool changed = false;
            for (int i = 0; i < array->_size; ++i) {
                changed |= remove(array->get((size_t)i));
            }
            return changed;
        }
        inline bool isEmpty(){
            return _size == 0;
        }
        inline const T& pop(){
            return _size > 0 ? removeAt(_size - 1) : NULL;
        }
        inline bool push(const T& val){
            return add(val);
        }
        inline bool addFirst(const T& val){
            return add(0, val);
        }
        inline bool addLast(const T& val){
            return add(_size, val);
        }
        inline T& removeFirst(){
            return removeAt(0);
        }
        inline T& removeLast(){
            return removeAt(_size - 1);
        }
        inline T& pollFirst(){
            return removeAt(0);
        }
        inline T& pollLast(){
            return removeAt(_size - 1);
        }
        inline T& peek(){
            return _size > 0 ? get(_size - 1) : NULL;
        }
        inline const T& peekFirst(){
            return _size > 0 ? get(0) : NULL;
        }
        inline const T& peekLast(){
            return _size > 0 ? get(_size - 1) : NULL;
        }
        //return true to break travel.
        inline void travel(std::function<bool(Array<T>* arr, int index, T& ele)>& func){
            auto c = size();
            for (int i = 0; i < c; ++i) {
                if(func(this, i, const_cast<T&>(get(i)))){
                    return;
                }
            }
        }
        inline void travel(bool(*Func)(Array<T>* arr, int index, T& ele, void* ud), void* ud){
            auto c = size();
            for (int i = 0; i < c; ++i) {
                if(Func(this, i, const_cast<T&>(get(i)), ud)){
                    return;
                }
            }
        }
        /**
         * clear elements
         */
        inline void clear(){
            _size = 0;
        }
        /** Reduces the size of the backing array to the size of the actual items. This is useful to release memory when many items
 * have been removed, or if it is known that more items will not be added.
         * */
        bool shrink(){
            if(_size > 0 && malCount != _size){
                return resize(_size);
            }
            return true;
        }
        bool ensureCapacity(int additionalCapacity){
            int sizeNeeded = _size + additionalCapacity;
            if(sizeNeeded > malCount){
                return resize(sizeNeeded);
            }
            return true;
        }
        void setSize(int newSize){
            _size = newSize;
        }

    protected:
        inline bool resize(size_t newSize) {
            void * d = malloc(newSize * sizeof(T));
            if(d == NULL){
                return false;
            }
            memcpy(d, data, sizeof(T) * _size);
            free(data);
            this->data = d;
            this->malCount = newSize;
            return true;
        }
    };

#define BTYPE_ARRAY(cn, type, com)\
    class cn: public Array<type>{\
    public:\
        cn(size_t capacity): Array(capacity, com) {}\
        cn() : Array(com) {}\
    };
    BTYPE_ARRAY(IntArray, int, NULL)
    BTYPE_ARRAY(UnSignedIntArray, unsigned int, NULL)
    BTYPE_ARRAY(ShortArray, short, NULL)
    BTYPE_ARRAY(UnSignedShortArray, unsigned short, NULL)
    BTYPE_ARRAY(UnSignedCharArray, unsigned char, NULL)
    BTYPE_ARRAY(CharArray, char, NULL)

    BTYPE_ARRAY(UnSignedLongArray, unsigned long long, NULL)
    BTYPE_ARRAY(LongArray, long long, NULL)
    BTYPE_ARRAY(FloatArray, float, NULL)
    BTYPE_ARRAY(DoubleArray, double, &COM_double)
    BTYPE_ARRAY(BoolArray, bool, NULL)
}
