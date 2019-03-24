#include "pch.h"

#include "utils.h"

bool endsWith(const std::string &s, const std::string &suffix)
{
  auto it = begin(suffix);
  return size(s) >= size(suffix) &&
    std::all_of(std::next(begin(s), size(s) - size(suffix)), end(s), [&it](const char c) {
    return c == *(it++);
  });
}