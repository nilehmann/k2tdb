/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_SECONDARY_STORAGE_SERIALIZATION_H_
#define INCLUDE_SECONDARY_STORAGE_SERIALIZATION_H_

#include <algorithm>
namespace secondary_storage {
namespace serialization {

template<class T>
struct size {
  size_t operator()(const T&val) const;
};

template<>
struct size<std::string> {
  size_t operator()(const std::string &val) const {
    return val.size() + 1;
  }
};

template<class T>
struct serialize {
  size_t operator()(const T& val, size_t *size) const;
};


template<>
struct serialize<std::string> {
  const char * operator()(const std::string &val, size_t *size) const {
    *size = val.size() + 1;
    return val.data();
  }
};


template<class T>
struct unserialize {
  T operator()(char *buff, size_t len) const;
};
template<>
struct unserialize<std::string> {
  std::string operator()(char *buff, size_t) const {
    return std::string(buff);
  }
};

}  // namespace serialization
}  // namespace secondary_storage
#endif  // INCLUDE_SECONDARY_STORAGE_SERIALIZATION_H_
