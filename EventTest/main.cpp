#include "evt.h"
//#include <stdlib>

class Sample
{
public:
    int go(int i)
    {
        std::cout <<"go" << std::endl;
        return 1;
    }
};

void main()
{
    Functor<int()> f0;
    f0.type_deduction();

    Functor<int(float)> f1;
    f1.type_deduction();

    Functor<int Sample::*(int)> f2;
    f2.type_deduction();

    system("pause");
}