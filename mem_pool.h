#ifndef MEMPOOL_H
#define MEMPOOL_H

#include <vector>


template<typename T>
class MemPool {   
private:
    std::vector<T> object;
    std::vector<bool> is_free;
    int free_head;

    public:
    explicit MemPool(std::size_t num_elems) : object(num_elems), is_free(num_elems, true), free_head(0) {
        
        if (num_elems == 0) {
            throw std::invalid_argument("MemPool size must be greater than zero");
        }

        std::vector<T> slot = new std::vector<T>() 

    }

    hi 
    
    template<typename... Args>
    T* allocate(Args &&... args) noexcept {
        for (size_t i; i < is_free.size(); i++) {
            if(is_free[i]) {
                objects[i] = T(std::forward<args)
                is_free[i] = false;
                return &objects[i];
            }
        }

        return nullptr;
    }


    void deallocate(const T *elem) noexcept {
        // implementation
    }

    MemPool() = delete("a MemPool owns its backing storage and free list");
    MemPool(const MemPool &) = delete("a MemPool owns its backing storage and free list");
    MemPool(const MemPool &&) = delete("a MemPool owns its backing storage and free list");
    MemPool &operator =(const Mempool &) = delete("a MemPool owns its backing storage and free list");
    MemPool &operator =(const Mempool &&) = delete("a MemPool owns its backing storage and free list");

};


#endif