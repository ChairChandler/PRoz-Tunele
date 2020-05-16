#ifndef LIMITEDSET_H
#define LIMITEDSET_H
#include <set>
#include <stdexcept>

template<class T>
class LimitedSet: public std::set<T>
{
    using iterator = typename std::set<T>::iterator;
    using const_iterator = typename std::set<T>::const_iterator;
    using value_type = typename std::set<T>::value_type;
    using insert_return_type = typename std::set<T>::insert_return_type;
    using node_type = typename std::set<T>::node_type;
    using Key = T;
    using Allocator = std::allocator<Key>;

    size_t maxSize;

    void check_size()
    {
        if(this->size() + 1 > this->max_size()) {
            throw std::runtime_error("maximum size exceeded");
        }
    }
public:
    LimitedSet() = delete ;
    LimitedSet(size_t maxSize): maxSize(maxSize)
    {

    }
    size_t max_size() const
    {
        return this->maxSize;
    }

    iterator
    insert(const T &val)
    {
        this->check_size();
        return std::set<T>::template insert(val);
    }

    std::pair<iterator,bool> insert( value_type&& value )
    {
        this->check_size();
        return std::set<T>::template insert(value);
    }

    //too much to implement size checking so delete it
    iterator insert( const_iterator hint, const value_type& value ) = delete ;
    iterator insert( const_iterator hint, value_type&& value ) = delete ;
    template< class InputIt >
    void insert( InputIt first, InputIt last ) = delete ;
    void insert( std::initializer_list<value_type> ilist ) = delete ;
    insert_return_type insert(node_type&& nh) = delete ;
    iterator insert(const_iterator hint, node_type&& nh) = delete ;

    template<class C2>
    void merge(std::set<Key, C2, Allocator>& source) = delete ;
    template<class C2>
    void merge(std::set<Key, C2, Allocator>&& source) = delete ;
    template<class C2>
    void merge(std::multiset<Key, C2, Allocator>& source) = delete ;
    template<class C2>
    void merge(std::multiset<Key, C2, Allocator>&& source) = delete ;

    template <class... Args>
    std::pair<iterator,bool> emplace (Args&&... args) = delete ;
    template <class... Args>
    iterator emplace_hint (const_iterator position, Args&&... args);
};

#endif // LIMITEDSET_H
