#pragma once
template <class T>
const std::shared_ptr<T> & Singleton()
{
    static std::shared_ptr<T> t = std::make_shared<T>();
    return t;
}