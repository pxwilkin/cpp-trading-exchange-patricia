#ifndef MEMPOOL_H
#define MEMPOOL_H

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

template<typename T>
class MemPool {   
private:
    std::vector<T> object;
    std::vector<bool> is_free;

    public:
    explicit MemPool(std::size_t num_elems) : object(num_elems), is_free(num_elems, true) {
        
        if (num_elems == 0) {
            throw std::invalid_argument("MemPool size must be greater than zero");
        }
    }

    
    template<typename... Args>
    T* allocate(Args &&... args) noexcept {
        for (size_t i = 0; i < is_free.size(); i++) {
            if(is_free[i]) {
                object[i] = T(std::forward<Args>(args)...);
                is_free[i] = false;
                return &object[i];
            }
        }

        return nullptr;
    }


    void deallocate(const T* elem) noexcept {
        for (size_t i = 0; i < object.size(); i++) {
            if (&object[i] == elem) {
                is_free[i] = true;
                return;
            }
        }
    }
 
    MemPool() = delete("a MemPool owns its backing storage and free list");
    MemPool(const MemPool &) = delete("a MemPool owns its backing storage and free list");
    MemPool(const MemPool &&) = delete("a MemPool owns its backing storage and free list");
    MemPool &operator =(const MemPool &) = delete("a MemPool owns its backing storage and free list");
    MemPool &operator =(const MemPool &&) = delete("a MemPool owns its backing storage and free list");

};


#endif