#ifndef SINGLETON_H
#define SINGLETON_H

/*
Let's make those various managers 'singletonized'.

Usage:
class SomeMgr : public Singleton<SomeMgr> {...};
Singleton<SomeMgr>::getInstance().init(...);
Singleton<SomeMgr>::getInstance().do_something();

Requirements:
1. SomeMgr must have a default ctor that takes no arguments
2. SomeMgr should have a init(...) that functions as a ctor
3. Though lazy initialization is used, init order would not be an issue as long as managers
   don't have dependencies on each other. Anyway, we explicitly init everything. 

Memo:
   If init order becomes a concern, refer to this SO post:
   http://stackoverflow.com/questions/335369/finding-c-static-initialization-order-problems/335746#335746
*/

template<typename T>
class Singleton
{
public:
    Singleton() {}
    ~Singleton(){}
    static T& getInstance()
    {
        static T instance;
        return instance;
    }
private:
    Singleton(const Singleton<T>& rhs);
    Singleton& operator=(const Singleton<T>& rhs);
};
#endif