#pragma once

#include <array>
#include <cstddef>
#include <iomanip>
#include <iostream>


namespace gaos::memory {

    enum class allocation_type {
        malloc,
        free,
        allocate,
        deallocate,
        malloc_allocate,
        free_deallocate
    };

    struct allocation_info {
        allocation_type  type;
        void            *address;
        std::size_t      size;
        std::size_t      repeat;
    };
    
    static constexpr std::size_t                      log_info_size = 4;
    static std::array<allocation_info, log_info_size> log_info;
    static std::size_t                                log_info_read_idx  = 0;
    static std::size_t                                log_info_write_idx = 0;
    
    static std::size_t count_malloc     = 0;
    static std::size_t count_free       = 0;
    static std::size_t size_malloc_cur  = 0;
    static std::size_t size_malloc_peak = 0;


    inline void log_meta_stats()
    {
        std::cout
          << "malloc " << std::setw(6) << count_malloc << "x "
          << "free " << std::setw(6) << count_malloc << "x "
          << "| now " << std::setw(6) << size_malloc_cur
          << " peak " << std::setw(6) << size_malloc_peak << std::endl;
    }


    inline void reset_meta_stats()
    {
        count_malloc     = 0;
        count_free       = 0;
        size_malloc_cur  = 0;
        size_malloc_peak = 0;
    }


    inline void log(const allocation_info& info)
    {
        namespace gm = gaos::memory;

        switch (info.type)
        {
            case allocation_type::malloc:          std::cout << "* malloc            "; break;
            case allocation_type::free:            std::cout << "*  free             "; break;
            case allocation_type::allocate:        std::cout << "*         allocate  "; break;
            case allocation_type::deallocate:      std::cout << "*        deallocate "; break;
            case allocation_type::malloc_allocate: std::cout << "* malloc  allocate  "; break;
            case allocation_type::free_deallocate: std::cout << "*  free  deallocate "; break;
        }

        std::cout
          << " " << std::setw(6) << info.repeat << "x " << std::setw(6) << info.size << " (" << std::setw(8) << (info.repeat * info.size) << ")" << std::endl;
    }


    inline bool log_try_collapse_lh_to_rh(allocation_info &info_lh, allocation_info &info_rh)
    {
        if (   info_lh.type == info_rh.type
            && info_lh.size == info_rh.size)
        {
            info_rh.repeat += info_lh.repeat;
            return true;
        }

        return false;
    }


    inline bool log_try_collapse_rh_to_lh(allocation_info &info_lh, allocation_info &info_rh)
    {
        if (info_lh.repeat > 1)
          return false;

        if (   info_lh.type == allocation_type::malloc
            && info_rh.type == allocation_type::allocate
            && info_rh.address == info_lh.address)
        {
            info_lh.type = allocation_type::malloc_allocate;
            return true;
        }

        if (   info_lh.type == allocation_type::deallocate
            && info_rh.type == allocation_type::free
            && info_rh.address == info_lh.address)
        {
            info_lh.type = allocation_type::free_deallocate;
            return true;
        }

        return false;
    }


    inline void log_flush(bool force)
    {
        // While there are 2 or more entries,
        // try to collapse them from right to left
        while (log_info_read_idx + 1 < log_info_write_idx) {
            if (log_try_collapse_rh_to_lh(
                   log_info[(log_info_write_idx-2) % log_info_size],
                   log_info[(log_info_write_idx-1) % log_info_size]
               )
            ) {
                --log_info_write_idx;
                continue;
            }
            
            break;
        }

        // If we are out of space, or we are forced to flush,
        // try to collapse the entries from left to right,
        // logging those we cannot collapse
        for (;;) {
            if (log_info_read_idx == log_info_write_idx)
              break;

            if (!force && log_info_write_idx - log_info_read_idx < log_info_size)
              break;

            if (   log_info_read_idx + 1 < log_info_write_idx 
                && log_try_collapse_lh_to_rh(
                     log_info[(log_info_read_idx+0) % log_info_size],
                     log_info[(log_info_read_idx+1) % log_info_size]
                   ))
            {
                ++log_info_read_idx;
                continue;
            }
            
            log(log_info[log_info_read_idx % log_info_size]);
            ++log_info_read_idx;
        }
    }


    inline void log_malloc(void * addr, std::size_t size)
    {
        auto& info = log_info[log_info_write_idx++ % log_info_size];

        info.type    = allocation_type::malloc;
        info.address = addr;
        info.size    = size;
        info.repeat  = 1;

        count_malloc     += 1;
        size_malloc_cur  += size;
        size_malloc_peak  = std::max(size_malloc_peak, size_malloc_cur);

        log_flush(false);
    }


    inline void log_free(void * addr, std::size_t size)
    {
        auto& info = log_info[log_info_write_idx++ % log_info_size];

        info.type    = allocation_type::free;
        info.address = addr;
        info.size    = size;
        info.repeat  = 1;

        count_free      += 1;
        size_malloc_cur -= size;

        log_flush(false);
    }


    inline void log_allocate(void * addr, std::size_t size)
    {
        auto& info = log_info[log_info_write_idx++ % log_info_size];

        info.type    = allocation_type::allocate;
        info.address = addr;
        info.size    = size;
        info.repeat  = 1;

        log_flush(false);
    }


    inline void log_deallocate(void * addr, std::size_t size)
    {
        auto& info = log_info[log_info_write_idx++ % log_info_size];

        info.type    = allocation_type::deallocate;
        info.address = addr;
        info.size    = size;
        info.repeat  = 1;

        log_flush(false);
    }

}