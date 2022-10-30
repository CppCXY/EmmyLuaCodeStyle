#pragma once

#include <cassert>
#include <cstring>
#include <iostream>
#include <string_view>

// see https://zhuanlan.zhihu.com/p/419673631
// 用来存储枚举反射信息的结构体
// 注意名字都使用 string_view 存储，以避免动态内存分配
struct ReflectionEnumInfo
{
    bool scoped; // 是否 scoped enum
    std::string_view name, valueFullName, valueName; // 类型名、值名、值全名

    // 构造时，从母串中按指定位置 得到各子串
    // info : 母串，即 __PRETTY_FUNCTION__ 得到的函数名
    // e1:等号1位置; s:分号位置; e2:等号2位置; colon:分号位置; end:]位置
    constexpr
    ReflectionEnumInfo(char const* info
            , std::size_t e1, std::size_t s, std::size_t e2
            , std::size_t colon, std::size_t end)
            : scoped(colon != 0), name (info + e1 + 2, s - e1 -2)
            , valueFullName (info + e2 + 2, end - e2 - 2)
            , valueName((scoped)? std::string_view(info+colon+1, end-colon-1)
                                : valueFullName)
    {}
};

// 说了半天的 模板函数，带 constexpr
template <typename E, E V>
constexpr ReflectionEnumInfo Renum()
{
    char const* info = __PRETTY_FUNCTION__;

    // 找各个符号位置
    std::size_t l = strlen(info);
    std::size_t e1 = 0, s = 0, e2 = 0, colon = 0, end = 0;

    for (std::size_t i=0; i<l && !end; ++i)
    {
        switch(info[i])
        {
            case '=' : (!e1) ? e1 = i : e2 = i; break;
            case ';' : s = i; break;
            case ':' : colon = i; break;
            case ']' : end = i; break;
        }
    }

    return {info, e1, s, e2, colon, end};
}

std::ostream& operator << (std::ostream& os, ReflectionEnumInfo const& ri)
{
    os << "scoped = " << std::boolalpha << ri.scoped << std::noboolalpha
       << "\nname = " << ri.name
       << "\nvalueName = " << ri.valueName
       << "\nvalueFullName = " << ri.valueFullName
       << "\n------------------------------\n";
    return os;
}