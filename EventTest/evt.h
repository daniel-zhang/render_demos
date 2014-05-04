#include <string>
#include <iostream>

template<typename T>
class Functor;

template<typename R>
class Functor<R()>
{
public:
    void type_deduction()
    {
        std::cout << typeid(R).name() << std::endl;
        std::cout << std::endl;
    }
};

template<typename R, typename T0>
class Functor<R(T0)>
{
public:
    void type_deduction()
    {
        std::cout << typeid(R).name() << std::endl
            << typeid(T0).name() << std::endl;
        std::cout << std::endl;
    }
};


