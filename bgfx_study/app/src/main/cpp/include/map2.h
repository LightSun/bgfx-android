//
// Created by Administrator on 2019/7/31.
//

#ifndef COMMONLUAAPP_MAP2_H
#define COMMONLUAAPP_MAP2_H

#include <map>

//std::unordered_map, set etc...
namespace h7 {

    template<typename K, typename V>
    class Map2 {
    private:
        std::map <K, V> _map;

    public:
        typedef bool (*Traveller)(const Map2<K, V> *map, const K &key, const V &value);

        Map2(const std::map<K, V> &map) {
            this->_map = map;
        }

        Map2() {}

        std::map<K, V>& getRawMap(){
            return _map;
        }
        void clear() {
            _map.clear();
        }
        const V& get(const K &key) {
            auto it = _map.find(key);
            return it->second;
        }
        const V& remove(const K &key) {
            auto it = _map.find(key);
            if (_map.erase(key)) {
                return it->second;
            }
            return nullptr;
        }

        const V& put(const K &key, const V &val) {
            V& old = _map[key];
            _map[key] = val;
            return old;
        }

        bool containsKey(const K &key) {
            return get(key) != nullptr;
        }

        const K ofKey(const V &val) {
            K *ptr = nullptr;
            auto tr = [val](const Map2<K, V> *_map, const K &key, const V &value) {
                if (value == val) {
                    ptr = &key;
                    return true;
                }
                return false;
            };
            travel(tr);
            return ptr != nullptr ? *ptr : nullptr;
        }

        bool containsValue(const V &val) {
            return ofKey(val) != nullptr;
        }

        int size() {
            return this->_map.size();
        }

        // true if breaked,  false otherwise.
        bool travel(const Traveller &t) {
            //assert(t != nullptr);
            for (auto it = _map.begin(); it != _map.end(); ++it) {
                if (t(this, it->first, it->second)) {
                    return true;
                }
            }
            return false;
        }

        Map2<K, V> &operator=(const Map2<K, V> &array1) {
            if (&array1 != this) {
                this->_map = array1._map;
            }
            return *this;
        }

        V &operator[](const K &__k) {
            return _map[__k];
        }
    };
}
#endif //COMMONLUAAPP_MAP2_H
