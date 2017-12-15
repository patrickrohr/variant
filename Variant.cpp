/**
 * C++11 Variant Class
 * 
 * At this point I am just playing around and building a lightweight variant implementation in C++11.
 */


#include <iostream> // TODO: Testing
#include <type_traits>


struct VariantHelper
{
    template<typename A>
    static constexpr unsigned int size()
    {
        return sizeof(A);
    }

    template<typename A, typename B, typename ...C>
    static constexpr unsigned int size()
    {
        return size<A>() > size<B, C...>() ? size<A>() : size<B, C...>();
    }
};


template<typename ...T>
class Variant
{
public:
    Variant() : m_data() {}




    static constexpr unsigned int size_type = VariantHelper::size<T...>();

private:
    std::aligned_union<size_type, T...> m_data;

};









// Check if a constexpr
template<unsigned int>
using CheckConstexpr = void;

unsigned int g_test = 5;
unsigned int TestFunction()
{
    return ++g_test;
}




int main()
{
    // Check if constexpr
    int test;
    (CheckConstexpr<Variant<int, short>::size_type>) test;
    int test2 = 5;
    (CheckConstexpr<2>) test2;

    std::cout << Variant<short, short>::size_type << std::endl;

    return 0;
}