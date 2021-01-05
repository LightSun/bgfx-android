//
// Created by Administrator on 2021/1/5 0005.
//

#ifndef BGFX_STUDY_LRUCACHE_H
#define BGFX_STUDY_LRUCACHE_H

#include "LockDelegate.h"
#include "map2.h"
#include "../core/log.h"

namespace h7 {

#define LRU_SYNC(x)\
    do {\
    cb->lock();\
    x;\
    cb->unlock();\
}while(0);

    template<typename K, typename V>
    class LruCache {
    public:
        template<typename K, typename V>
        class Callback : public ILock{
        public:
            virtual V create(K key) {
                return nullptr;
            }
            virtual void entryRemoved(bool evicted, K key, V oldValue, V newValue) {
            }
            virtual int sizeOf(K key, V value) {
                return 1;
            }
        };
        LruCache(Callback* cb, unsigned int maxSize) : cb(cb), maxSize(maxSize) {
        }
        void resize(unsigned int maxSize) {
            LRU_SYNC({
                this->maxSize = maxSize;
            })
            trimToSize(maxSize);
        }
        V get(K key){
            if (&key == nullptr) {
                LOGE("key == null");
            }
            V mapValue;
            LRU_SYNC({
                mapValue = map.get(key);
                if (mapValue != nullptr) {
                    hitCount++;
                    return mapValue;
                }
                missCount++;
            });

            /*
             * Attempt to create a value. This may take a long time, and the map
             * may be different when create() returns. If a conflicting value was
             * added to the map while create() was working, we leave that value in
             * the map and release the created value.
             */

            V createdValue = cb->create(key);
            if (createdValue == nullptr) {
                return nullptr;
            }

            LRU_SYNC ({
                createCount++;
                mapValue = map.put(key, createdValue);
                if (mapValue != nullptr) {
                    /* There was a conflict so undo that last put */
                    map.put(key, mapValue);
                }else{
                    size += cb->sizeOf(key, createdValue);
                }
            })

            if (mapValue != nullptr) {
                cb->entryRemoved(false, key, createdValue, mapValue);
                return mapValue;
            } else {
                trimToSize(maxSize);
                return createdValue;
            }
        }
        V put(K key, V value){
            if (key == nullptr || value == nullptr) {
                //throw new NullPointerException("key == null || value == null");
            }

            V previous;
            LRU_SYNC({
                putCount++;
                size += cb->sizeOf(key, value);
                previous = map.put(key, value);
                if (previous != nullptr) {
                    size -= cb->sizeOf(key, previous);
                }
            })
            if (previous != nullptr) {
                cb->entryRemoved(false, key, previous, value);
            }
            trimToSize(maxSize);
            return previous;
        }
        void evictAll() {
            trimToSize(-1); // -1 will evict 0-sized elements
        }
        /**
         * Returns a copy of the current contents of the cache, ordered from least
         * recently accessed to most recently accessed.
         */
        Map2<K, V> snapshot() {
            return Map2<K, V>(map.getRawMap());
        }
    private:
        void trimToSize(int maxSize){
            while (true) {
                K key;
                V value;
                LRU_SYNC({
                    if (size < 0 || (map.size() ==0 && size != 0)) {
                        LOGE("LruCache.sizeOf() is reporting inconsistent results!");
                        return;
                    }
                    if (size <= maxSize) {
                        break;
                    }
                    K* toEvictK = nullptr;
                    V* toEvictV = nullptr;
                    for (auto it = map.getRawMap().begin(); it != map.getRawMap().end(); ++it){
                        toEvictK = &(it.first);
                        toEvictV = &(it.second);
                    }
                    if (toEvictK == nullptr) {
                        break;
                    }
                    key = *toEvictK;
                    value = *toEvictV;
                    map.remove(key);
                    size -= cb->sizeOf(key, value);
                    evictionCount++;
                })
                cb->entryRemoved(true, key, value, nullptr);
            };
        }

        Map2<K, V> map;
        Callback* cb;
    public:
        /** Size of this cache in units. Not necessarily the number of elements. */
        int size;
        int maxSize;

        int putCount;
        int createCount;
        int evictionCount;
        int hitCount;
        int missCount;
    };
}

#endif //BGFX_STUDY_LRUCACHE_H
