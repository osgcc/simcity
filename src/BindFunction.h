#pragma once

#include <functional>

template <class T, class U>
void BindFuncPtr(T& arg1, U arg2)
{
    auto it = std::find(arg1.begin(), arg1.end(), arg2);
    if (it != arg1.end()) { throw std::runtime_error("BindFuncPtr(): function pointer already bound."); }

    arg1.push_back(arg2);
}


template <class T, class U>
void UnbindFuncPtr(T& arg1, U arg2)
{
    auto it = std::find(arg1.begin(), arg1.end(), arg2);
    if (it == arg1.end()) { throw std::runtime_error("UnbindFuncPtr(): function pointer not bound."); }

    arg1.erase(it);
}
