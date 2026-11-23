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
    std::size_t nextFree = 0;
    
    public:
    explicit MemPool(std::size_t num_elems) : object(num_elems), is_free(num_elems, true) {
        
        if (num_elems == 0) {
            throw std::invalid_argument("MemPool size must be greater than zero");
        }
    }

    
    template<typename... Args>
    T* allocate(Args &&... args) noexcept {
        // Find next available slot
        while (nextFree < is_free.size() && !is_free[nextFree]) {
            nextFree++;
        }
        
        if (nextFree == is_free.size()) {
            return nullptr;
        }


        // Allocate object
        std::size_t index = nextFree;
        object[index] = T(std::forward<Args>(args)...);
        is_free[index] = false;

        nextFree++;

        return &object[index];

    }


    void deallocate(const T* elem) noexcept {
        if (elem == nullptr) return;

        int index = elem - &object[0];

        if (index < 0 || static_cast<std::size_t>(index) >= object.size() 
        || is_free[index]) {
            return;
        }

        is_free[index] = true;

        nextFree = std::min(nextFree, static_cast<std::size_t>(index));

    }
 
    MemPool() = delete("a MemPool owns its backing storage and free list");
    MemPool(const MemPool &) = delete("a MemPool owns its backing storage and free list");
    MemPool(const MemPool &&) = delete("a MemPool owns its backing storage and free list");
    MemPool &operator =(const MemPool &) = delete("a MemPool owns its backing storage and free list");
    MemPool &operator =(const MemPool &&) = delete("a MemPool owns its backing storage and free list");

};


#endif