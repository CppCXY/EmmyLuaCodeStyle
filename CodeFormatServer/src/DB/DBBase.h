#pragma once

#include <optional>
#include <unordered_map>
#include <functional>
#include <memory>

template<class K, class V, class Container=std::unordered_map<K, V>>
class DBBase {
public:
    DBBase() {};

    virtual ~DBBase() {};

    virtual void Input(const K &key, V &&value) {
        _hash[key] = value;
    }

    virtual std::optional<V> Query(const K &key) {
        if (_hash.contains(key)) {
            return _hash[key];
        }
        return std::nullopt;
    };

    virtual void Delete(const K &key) {
        auto it = _hash.find(key);
        if (it != _hash.end()) {
            _hash.erase(it);
        }
    }

private:
    Container _hash;
};

template<class K, class V, class Container=std::unordered_map<K, V>>
class SharedDBBase : public DBBase<K, std::shared_ptr<V>> {
public:
    virtual void Input(const K &key, std::shared_ptr<V> value) {
        DBBase<K, std::shared_ptr<V>>::Input(std::forward<const K &>(key), std::move(value));
    }
};