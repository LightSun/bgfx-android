//
// Created by Administrator on 2020/11/24 0024.
//

#include <limits.h> // INT_MAX
#include <math.h>
#include <stdio.h>

#include "Array.hpp"
//#include "bx/bx.h"

namespace h7 {

    /** Objects implementing this interface will have {@link #reset()} called when passed to {@link Pool#free(Object)}. */
    class Poolable {
        /** Resets the object for reuse. Object references should be nulled and fields may be set to default values. */
    public:
        virtual void reset() = 0;
    };
    /**
     * a pool used to manage multi pointer object.
     * @tparam T the type
     */
    template<typename T>
    class Pool {

    private:
        /** The maximum number of objects that will be pooled. */
        int _max;
        /* The highest number of free objects. Can be reset any time. */
        //int peak;

        Array<T*> *freeObjects;
    public:
        /** Creates a pool with an initial capacity of 16 and no maximum. */
        Pool():Pool(16, INT_MAX) {
        }

        /** Creates a pool with the specified initial capacity and no maximum. */
        Pool(int initialCapacity):Pool(initialCapacity, INT_MAX) {
        }

        /** @param initialCapacity The initial size of the array supporting the pool. No objects are created/pre-allocated. Use
         *           {@link #fill(int)} after instantiation if needed.
         * @param max The maximum number of free objects to store in this pool. */
        Pool(int initialCapacity, int max) {
            freeObjects = new Array<T*>(initialCapacity, nullptr);
            this->_max = max;
        }

        ~Pool() {
            if (freeObjects) {
                clear();
                delete freeObjects;
                freeObjects = nullptr;
            }
        }

        /** Returns an object from this pool. The object may be new (from {@link #newObject()}) or reused (previously
	 * {@link #free(Object) freed}). */
        T* obtain() {
            return freeObjects->size() == 0 ? newObject() : freeObjects->pop();
        }

        /** Puts the specified object in the pool, making it eligible to be returned by {@link #obtain()}. If the pool already contains
         * {@link #max} free objects, the specified object is reset but not added to the pool.
         * <p>
         * The pool does not check if an object is already freed, so the same object must not be freed multiple times. */
        void free(T* object, bool _reset = false) {
            if (object != nullptr){
                if (freeObjects->size() < _max) {
                    freeObjects->add(object);
                    //peak = bx::max(peak, freeObjects->getSize());
                }
                if(_reset){
                    reset(object);
                }
            }
        }

        /** Adds the specified number of new free objects to the pool. Usually called early on as a pre-allocation mechanism but can be
         * used at any time.
         *
         * @param size the number of objects to be added */
        void fill(int size) {
            for (int i = 0; i < size; i++)
                if (freeObjects->size() < _max) freeObjects->add(newObject());
            //peak = bx::max(peak, freeObjects->getSize());
        }

        /** Called when an object is freed to clear the state of the object for possible later reuse. The default implementation calls
         * {@link Poolable#reset()} if the object is {@link Poolable}. */
        void reset(T* object) {
            Poolable* p = reinterpret_cast<Poolable*>(object);
            p->reset();
        }

        /** Puts the specified objects in the pool. Null objects within the array are silently ignored.
         * <p>
         * The pool does not check if an object is already freed, so the same object must not be freed multiple times.
         * @see #free(Object) */
        void freeAll(Array<T*>* objects, bool _reset = false) {
            if(objects != nullptr){
                Array<T*>* freeObjects = this->freeObjects;
                int _max = this->_max;
                for (int i = 0, n = objects->size(); i < n; i++) {
                    T* object = objects->get(i);
                    if (object == nullptr) continue;
                    if (freeObjects->size() < _max) freeObjects->add(object);
                    if(_reset){
                        reset(object);
                    }
                }
                //peak = bx::max(peak, freeObjects->getSize());
            }
        }

        /** Removes all free objects from this pool. */
        void clear() {
            auto size = freeObjects->size();
            for (int i = 0; i < size; ++i) {
                auto t = freeObjects->get(i);
                if(t){
                    delete t;
                }
            }
            freeObjects->clear();
        }

        /** The number of objects available to be obtained. */
        int getFree() {
            return freeObjects->size();
        }
    protected:
        virtual T* newObject(){
            return new T();
        }
    };
}