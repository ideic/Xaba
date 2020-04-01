#pragma once
template <class T>
T& Singleton()
{
    static T t;
    return t;
}