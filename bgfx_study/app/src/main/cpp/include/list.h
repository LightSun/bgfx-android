//
// Created by Administrator on 2019/7/24.
//

#ifndef ANDROIDCHORDINO_LIST_H
#define ANDROIDCHORDINO_LIST_H

#include <vector>
#include <sstream>

namespace h7 {
    template<typename T>
    class List {

    private:
        std::vector <T> array;

    public:
        inline size_t size() {
            return array.size();
        }

        inline std::vector<T>& getVector() {
            return array;
        }

        inline T& removeLast() {
            return removeAt(size() - 1);
        }

        inline T& getAt(int index) {
            return array[index];
        }

        inline T& getLast() {
            return getAt((int) size() - 1);
        }

        inline T& getFirst() {
            return getAt(0);
        }
        inline void add(const T &t) {
            array.push_back(t);
            // array.insert(array.end(), t);
        }
        inline void add(int index, T &t) {
            array.insert(array.begin() + index, t);
        }
        inline void addAll(List<T> &l) {
            array.insert(array.end(), l.array.begin(), l.array.end());
        }
        inline void addAll(int index, List<T> &l) {
            array.insert(array.begin() + index, l.array.begin(), l.array.end());
        }
        inline bool isEmpty() {
            return size() == 0;
        }
        inline bool contains(T &t) {
            return indexOf(t) >= 0;
        }
        inline int indexOf(T &t){
            size_t size = array.size();
            for (int i = 0; i < size; i++) {
                //need overload ==
                if (array[i] == t) {
                    return i;
                }
            }
            return -1;
        }
        inline int lastIndexOf(T &t){
            size_t size = array.size();
            for (int i = size - 1; i >= 0; i--) {
                //need overload ==
                if (array[i] == t) {
                    return i;
                }
            }
            return -1;
        }
        inline bool remove(T &t) {
            size_t size = array.size();
            for (int i = 0; i < size; i++) {
                //need overload ==
                if (array[i] == t) {
                    array.erase(array.begin() + i);
                    break;
                }
            }
            return false;
        }
        inline T& set(int index, T &newT) {
            if (index < size()) {
                T &old = array[index];
                array[index] = newT;
                return old;
            } else{
                return *(T*)NULL;
            }
        }
        inline void swap(int idx1, int idx2){
            T& t1 = array[idx1];
            array[idx1] = array[idx2];
            array[idx2] = t1;
        }
        inline void clear() {
            array.clear();
        }
        inline T& removeAt(int index) {
            if (index >= size()) {
                return *(T*)NULL;
            }
            T &t = array[index];
            remove(index, 1);
            return t;
        }
        inline bool remove(int startIndex, int count) {
            long size = array.size();
            if (count <= 0 || startIndex < 0 || startIndex >= size) {
               // __throw_invalid_argument("start or end index is wrong");
                return false;
            }
            array.erase(array.begin() + startIndex,
                        array.begin() + startIndex + count);
            return true;
        }
        inline List<T> &operator=(List<T> &array1) {
            if (&array1 != this) {
                this->array = array1.array;
            }
            return *this;
        }
        inline T &operator[](const int key) {
            return array[key];
        }

        const std::string toString() {
            const size_t s = size();
            if (s == 0) {
                return "";
            }
            std::stringstream out;
            out << getAt(0);
            //startsWidth
            if (out.str().find("0x") == 0) {
                //means is address.
                out.str("");
                for (int i = 0; i < s; ++i) {
                    out << *getAt(i) << ", ";
                }
            } else {
                for (int i = 1; i < s; ++i) {
                    out << ", " << getAt(i);
                }
            }
            return out.str();
        }
    };
}

#endif //ANDROIDCHORDINO_LIST_H
