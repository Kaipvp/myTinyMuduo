#pragma once

//NoCopyable被继承后，子类对象可正常构造和析构，但无法进行拷贝构造和赋值构造
class NoCopyable{
    NoCopyable(const NoCopyable&) = delete;
    NoCopyable& operator=(const NoCopyable&) = delete;
    /*
        void operator=(const nocopyable&) = delete;
        muduo将返回值设置为void,这里效果相同
    */
   protected:
        NoCopyable() = default;
        ~NoCopyable() = default;
};