#include "pch.h"

#include "utils.h"

bool ends_with(const std::string_view s, const std::string_view suffix)
{
  auto it = begin(suffix);
  return size(s) >= size(suffix) &&
    std::all_of(std::next(begin(s), size(s) - size(suffix)), end(s), [&it](const char c) {
    return c == *(it++);
  });
}