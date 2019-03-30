#include "pch.h"

#include <log.h>

#include <wmi_classes.h>

using namespace wmi;

template <typename Func, typename A, typename ...Args> 
struct Caller
{
  static void call(Func & f, A && a, Args && ...args)
  {
    f(std::forward<A>(a));
    Caller<Func, Args...>::call(f, std::forward<Args>(args)...);
  }
};

template <typename Func, typename A> 
struct Caller<Func, A>
{
  static void call(Func & f, A && a)
  {
    f(std::forward<A>(a));
  }
};

template <typename Func, typename ...Args>
void Call(Func & f, Args && ...args)
{
  Caller<Func, Args...>::call(f, std::forward<Args>(args)...);
}

template<typename T, T(*Func)(T), T Arg>
struct ap1
{
  static constexpr T value = Func(Arg);
};

constexpr int sq(int x)
{
  return x * x;
}

template<size_t Idx, typename T, T... Ints>
constexpr T geti(std::integer_sequence<T, Ints...> seq)
{
  return std::get<Idx>(std::make_tuple(Ints...));
}

template<typename IntType, IntType(*Func)(IntType), IntType... ints> 
constexpr auto map_integer_sequence(std::integer_sequence<IntType, ints...> seq) -> auto
{ 
  return std::integer_sequence<IntType, ap1<IntType, Func, ints>::value...>{};
}

struct dest_field
{
  const char * const _name;
  void * const _destination;
};

void the_func(const dest_field & df)
{
  std::cout << "called: " << df._name;
}

template<typename...T>
void get_fields(T &&... names_and_fields)
{
  Call(the_func, std::forward<T>(names_and_fields)...);
}

int main()
{
  //for(const auto& obj : wmi::Win32_UserProfile::get_all_objects())
  //{
  //  const auto str = obj.to_string();
  //  log(info, "object:\n%s\n", str.c_str());
  //}

  //using seq = std::make_integer_sequence<int, 5>;
  //using sq_seq = decltype(map_integer_sequence<int, sq>(seq{}));
  //std::cout << "" << geti<3>(sq_seq{});
  
  int * ptr = nullptr;
  char * ptr2 = nullptr;
  get_fields(dest_field{"@_@", ptr}, dest_field{"#_#", ptr2});
  return 0;
}