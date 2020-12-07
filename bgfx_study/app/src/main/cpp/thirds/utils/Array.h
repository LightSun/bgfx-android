//
// Created by Administrator on 2020/11/20 0020.
//
#pragma once
#include <malloc.h>
#include "list.h"

#define ADD_BASE(arr, val) {\
auto v = val;\
(arr)->add(v);}

namespace h7{
    template<typename T>
    class Array;
    //--------------------------

    template<typename T>
    class ArrayIterator{
    public:
        virtual bool iterate(Array<T>* arr, int index, T& ele) = 0;
    };

    template<typename T>
    class Array{
    private:
        List<T> list;

    public:
        Array(int capacity){
            list.getVector().reserve(capacity);
        }
        Array():Array(4){}
        ~Array(){
        }
        inline Array<T>& set(Array<T>& in){
            list.clear();
            list.addAll(in.list);
            return *this;
        }
        inline int size(){
            return list.size();
        }
        inline bool add(T& val){
            return add(list.size(), val);
        }
        inline bool add(int index, T& val){
            if(index < 0){
                return false;
            }
            list.add(index, val);
            return true;
        }

        inline bool add(T& val1, T& val2){
            add(val1);
            add(val2);
            return true;
        }
        inline bool add(T& val1, T& val2, T& val3){
            add(val1);
            add(val2);
            add(val3);
            return true;
        }
        inline bool addAll(Array<T>* array){
            list.addAll(array->list);
            return true;
        }
        inline bool addAll(int index, Array<T>* array){
            list.addAll(index, array->list);
            return true;
        }
        inline T& get(int index){
            return list.getAt(index);
        }
        inline void set(int index, T& val){
            list.set(index, val);
        }
        inline void swap(int first, int second){
            list.swap(first, second);
        }
        inline bool contains(T& val){
            return indexOf(val) >= 0;
        }
        inline int indexOf(T& val){
            return list.indexOf(val);
        }
        inline int lastIndexOf(T& val){
            return list.lastIndexOf(val);
        }
        inline bool remove(T& val){
            return list.remove(val);
        }
        inline T& removeAt(int index){
            return list.removeAt(index);
        }
        inline  bool removeAll(Array<T>* array){
            bool changed = false;
            for (int i = 0; i < array->size(); ++i) {
                changed |= remove(array->get((size_t)i));
            }
            return changed;
        }
        inline bool isEmpty(){
            return list.isEmpty();
        }
        inline const T& pop(){
            return list.removeLast();
        }
        inline bool push(const T& val){
            return add(val);
        }
        inline bool addFirst(const T& val){
            return add(0, val);
        }
        inline bool addLast(const T& val){
            return add(size(), val);
        }
        inline T& removeFirst(){
            return removeAt(0);
        }
        inline T& removeLast(){
            return removeAt(size() - 1);
        }
        inline T& pollFirst(){
            return removeAt(0);
        }
        inline T& pollLast(){
            return removeAt(size() - 1);
        }
        inline T& peek(){
            return size() > 0 ? get(size() - 1) : *(T*)NULL;
        }
        inline const T& peekFirst(){
            return size() > 0 ? get(0) : *(T*)NULL;
        }
        inline const T& peekLast(){
            return size() > 0 ? get(size() - 1) : *(T*)NULL;
        }
        //return true to break travel.
        inline void travel(ArrayIterator<T>* iterator){
            if(iterator != nullptr){
                auto c = size();
                for (int i = 0; i < c; ++i) {
                    if(iterator->iterate(this, i, get(i))){
                        return;
                    }
                }
            }
        }
        /**
         * clear elements
         */
        inline void clear(ArrayIterator<T>* iterator = nullptr){
            travel(iterator);
            list.clear();
        }
        inline void setSize(int newSize){
            list.getVector().resize(newSize);
        }
        inline Array<T>& copy(){
            Array<T> arr;
            return arr.set(*this);
        }
        inline T&operator[](int index){
            return get(index);
        }
        inline Array<T>& operator=(const Array<T> that){
            list = that.list;
            return *this;
        }
    };

#define BTYPE_ARRAY(cn, type)\
    class cn: public Array<type>{\
    public:\
        cn(size_t capacity): Array(capacity) {}\
        cn() : Array() {}\
    };
    BTYPE_ARRAY(IntArray, int)
    BTYPE_ARRAY(UnSignedIntArray, unsigned int)
    BTYPE_ARRAY(ShortArray, short)
    BTYPE_ARRAY(UnSignedShortArray, unsigned short)
    BTYPE_ARRAY(UnSignedCharArray, unsigned char)
    BTYPE_ARRAY(CharArray, char)

    BTYPE_ARRAY(UnSignedLongArray, unsigned long long)
    BTYPE_ARRAY(LongArray, long long)
    BTYPE_ARRAY(FloatArray, float)
    BTYPE_ARRAY(DoubleArray, double)
    BTYPE_ARRAY(BoolArray, bool)

#define ITERATOR_CLASS(name)\
    class Iterator: public ArrayIterator<name*>{\
    public:\
        bool iterate(Array<name*>* arr, int index, name*& ele){\
            ele->unRefAndDestroy();\
            return false;\
        }\
    };

#define ITERATOR_CNT_CLASS(name)\
    class Iterator: public ArrayIterator<sk_sp<name>>{\
    public:\
        bool iterate(Array<sk_sp<name>>* arr, int index, sk_sp<name>& ele){\
            ele.reset();\
            return false;\
        }\
    };

#define ITERATOR_CNT_CLASS_EXT(name, x)\
    class Iterator: public ArrayIterator<sk_sp<name>>{\
    public:\
        bool iterate(Array<sk_sp<name>>* arr, int index, sk_sp<name>& ele){\
            x ;\
            ele.reset();\
            return false;\
        }\
    };
}
