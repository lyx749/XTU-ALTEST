#include "test3.hpp"
#include "test2.hpp"
#include "test1.hpp"

int main()
{
again:
    std::cout << "please input <1-4> to choose mode" << std::endl;
    std::cout << "1 : Eight digital\n"
              << "2 : Genetic algorithm\n"
              << "3 : BP\n" << "4 : exit" << std::endl;
    int mode = 0;
    std::cout << "please input : ";
    std::cin >> mode;
    switch (mode)
    {
    case 1:
        test();
        goto again;
    case 2:
        run();
        goto again;
    case 3:
        runtest();
        goto again;
    case 4:
        break;
    default:
        std::cerr << "input error!! please input again" << std::endl;
        goto again;
    }
}