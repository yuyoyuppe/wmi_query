// wmi_query.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"



int main()
{

  for(const auto [k,v] : manufacturers)
    std::cout << k << "->" << v << '\n';
  return 0;
}
