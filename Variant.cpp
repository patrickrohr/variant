/*!************************************************************
 * C++11 Variant Class
 * 
 * At this point I am just playing around and building a lightweight variant implementation in C++11.
 * @author  Patrick Rohr
**************************************************************/

#include <iostream> // TODO: Testing
#include <typeinfo>
#include <type_traits>
#include <stdexcept>



template < typename Tp, typename... List >
struct contains : std::true_type {};

template < typename Tp, typename Head, typename... Rest >
struct contains<Tp, Head, Rest...> : std::conditional< std::is_same<Tp, Head>::value,
    std::true_type,
    contains<Tp, Rest...>
>::type {};

template < typename Tp >
struct contains<Tp> : std::false_type {};

/*!************************************************************
 * @brief      Helper Class
 * @date       December 14, 2017
 * @author     Patrick Rohr
**************************************************************/
struct VariantHelper
{
    template<typename A>
    static constexpr size_t size()
    {
        return sizeof(A);
    }

    template<typename A, typename B, typename ...C>
    static constexpr size_t size()
    {
        return size<A>() > size<B, C...>() ? size<A>() : size<B, C...>();
    }
};


/*!************************************************************
 * @brief      Variant Class
 * @tparam     T     { description }
 * @date       December 14, 2017
 * @author     Patrick Rohr
 * @todo       Right now, everything is inlined in the "header file",
 *             should eventually be split into header and cpp to hide implementation.
 * @todo       ATM, everything is stored on the stack, let's set a size S and everything
 *             larger than S should be stored in the heap. S would preferrably be the pointer size
 *             of the target (sizeof(void*)).
 * @todo       One of the shortcomings of using typeid().hash_code() for type identification is that we
 *             don't have a way of checking which type is stored inside a variant.
**************************************************************/
template<typename ...Ts>
class Variant
{
public:
    Variant() : m_data(), m_type() {}

    template<typename T>
    Variant(T data) : m_data(), m_type(typeid(T).hash_code())
    {
        static_assert(contains<T, Ts...>{}, "Type not allowed.");
        new (&m_data) T(data);
    }

    template<typename T>
    Variant& operator=(T data)
    {
        static_assert(contains<T, Ts...>{}, "Type not allowed.");
        m_type = typeid(T).hash_code();
        new (&m_data) T(data);
        return *this;
    }

    // Copy and Move constructors can be defaulted since m_data is POD.
    Variant(const Variant&) = default;
    Variant& operator=(const Variant&) = default;
    Variant(Variant&&) = default;
    Variant& operator=(Variant&&) = default;

    template<typename T>
    T Get()
    {
        if (typeid(T).hash_code() != m_type)
        {
            throw std::runtime_error("Type mismatch!");
        }
        // TODO: let me think about this part. do i really need to cast it back to a T* before dereferencing it again?
        T* pData = reinterpret_cast<T*>(&m_data);
        return *pData;
    }

    static constexpr size_t size_type = VariantHelper::size<Ts...>();

private:
    // Union containing data
    typename std::aligned_union<size_type, Ts...>::type m_data;
    // size_t containing typeinfo hash
    std::size_t m_type;

};


/*!************************************************************
 * TESTING
**************************************************************/
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

    Variant<int, char> var;
    var = static_cast<int>(123);
    std::cout << var.Get<int>() << std::endl;
    std::cout << Variant<short, short>::size_type << std::endl;

    return 0;
}