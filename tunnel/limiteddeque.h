#ifndef LIMITEDDEQUE_H
#define LIMITEDDEQUE_H
#include <deque>
#include <stdexcept>

template<class T>
class LimitedDeque: public std::deque<T>
{
    using iterator = typename std::deque<T>::iterator;
    using const_iterator = typename std::deque<T>::const_iterator;
    using size_type = typename std::deque<T>::size_type;
    using reference = typename std::deque<T>::reference;
    using const_reference = typename std::deque<T>::const_reference;

    size_t maxSize;

    void check_size()
    {
        if(this->size() + 1 > this->max_size()) {
            throw std::runtime_error("maximum size exceeded " + std::string(__FILE__) + " " +std::to_string(__LINE__));
        }
    }

public:
    LimitedDeque() = delete ;
    LimitedDeque(size_t maxSize): maxSize(maxSize)
    {

    }
    size_t max_size() const
    {
        return this->maxSize;
    }

    iterator insert( const_iterator pos, const T& value )
    {
        this->check_size();
        return std::deque<T>::insert(pos, value);
    }

    iterator insert( const_iterator pos, T&& value )
    {
        this->check_size();
        return std::deque<T>::insert(pos, value);
    }

    void push_back( const T& value )
    {
        this->check_size();
        return std::deque<T>::push_back(value);
    }

    void push_front( const T& value )
    {
        this->check_size();
        return std::deque<T>::push_front(value);
    }

    const_reference at(size_type __n) const
    {
        return std::deque<T>::at(__n);
    }

    //too much to implement size checking so delete it
    void insert( iterator pos, size_type count, const T& value ) = delete ;
    iterator insert( const_iterator pos, size_type count, const T& value ) = delete ;
    template< class InputIt >
    void insert( iterator pos, InputIt first, InputIt last) = delete ;
    template< class InputIt >
    iterator insert( const_iterator pos, InputIt first, InputIt last ) = delete ;
    iterator insert( const_iterator pos, std::initializer_list<T> ilist ) = delete ;

    template< class... Args >
    iterator emplace( const_iterator pos, Args&&... args ) = delete ;

    void push_back( T&& value ) = delete ;

    template< class... Args >
    void emplace_back( Args&&... args ) = delete ;
    template< class... Args >
    reference emplace_back( Args&&... args ) = delete ;

    void push_front( T&& value ) = delete ;

    template< class... Args >
    void emplace_front( Args&&... args ) = delete ;
    template< class... Args >
    reference emplace_front( Args&&... args ) = delete ;
};

#endif // LIMITEDDEQUE_H
