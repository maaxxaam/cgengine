#pragma once

#include <cassert>

/*!
 * \brief A pointer class that doesn't own the data (i.e. ownership/destruction rights are held by smbd else)
 */
template<typename T>
class watch_ptr {
public:
    template<typename Ptr>
    friend bool operator==(watch_ptr<Ptr> const& lhs, 
			   watch_ptr<Ptr> const& rhs);
    template<typename Ptr>
    friend bool operator!=(watch_ptr<Ptr> const& lhs, 
			   watch_ptr<Ptr> const& rhs);
    explicit watch_ptr(T* ptr = nullptr)
        : ptr_{ptr}
    {
    }
    watch_ptr(watch_ptr const&) = default;
    watch_ptr& operator=(watch_ptr const&) = default;
    T& operator*() noexcept;
    T const& operator*() const noexcept;
    T* operator->() noexcept;
    T* get() noexcept;
    T const* operator->() const noexcept;
    bool empty() const noexcept;
    explicit operator bool() const noexcept;
    ~watch_ptr() = default;
private:
    T* ptr_{}; // default ptr to nullptr
};

template<typename T>
bool operator==(watch_ptr<T> const& lhs, watch_ptr<T> const& rhs) {
    return lhs.ptr_ == rhs.ptr_;
}

template<typename T>
bool operator!=(watch_ptr<T> const& lhs, watch_ptr<T> const& rhs) {
    return !(lhs == rhs);
}

template<typename T>
T& watch_ptr<T>::operator*() noexcept {
    assert(ptr_ != nullptr);
    return *ptr_;
}

template<typename T>
T const& watch_ptr<T>::operator*() const noexcept {
    assert(ptr_ != nullptr);
    return *ptr_;
}

template<typename T>
T* watch_ptr<T>::operator->() noexcept {
    assert(ptr_ != nullptr);
    return ptr_;
}

template<typename T>
T* watch_ptr<T>::get() noexcept {
    assert(ptr_ != nullptr);
    return ptr_;
}

template<typename T>
T const* watch_ptr<T>::operator->() const noexcept {
    assert(ptr_ != nullptr);
    return ptr_;
}

template<typename T>
bool watch_ptr<T>::empty() const noexcept {
    return ptr_ == nullptr;
}

template<typename T>
watch_ptr<T>::operator bool() const noexcept {
    return ptr_ != nullptr;
}
