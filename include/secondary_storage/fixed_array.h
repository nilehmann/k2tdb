/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_SECONDARY_STORAGE_FIXED_ARRAY_H_
#define INCLUDE_SECONDARY_STORAGE_FIXED_ARRAY_H_
#include <secondary_storage/serialization.h>

#include <boost/filesystem.hpp>
#include <fstream>
#include <memory>
#include <algorithm>

namespace secondary_storage {

using boost::filesystem::path;

/**
 * Implements an array of fixed length entries on secondary storage
 */
template<class T,
         class Serialize = serialization::serialize<T>,
         class Unserialize = serialization::unserialize<T>,
         class GetSize = serialization::size<T>>
class FixedArray {
 public:
  FixedArray(const path &file_name, size_t size)
      : in_(file_name.native(), std::ifstream::in),
        out_(file_name.native(), std::ofstream::out),
        entry_size_(size),
        tail_(new char[entry_size_]),
        size_(0) {
    std::fill(tail_, tail_ + entry_size_, '\0');
    out_.seekp(0, std::ofstream::end);
    size_ = out_.tellp()/entry_size_;
  }
  ~FixedArray() {
    delete [] tail_;
    in_.close();
    out_.close();
  }
  size_t PushBack(const T& val) {
    assert(GetSize()(val) <= entry_size_);

    Serialize serializer;
    size_t size;
    const char *serialization = serializer(val, &size);

    out_.seekp(0, std::ofstream::end);
    out_.write(serialization, size);
    out_.write(tail_, entry_size_ - size);
    size_++;
    return size_;
  }

  size_t Size() {
    return size_;
  }

  T operator[](size_t pos) {
    Unserialize unserializer;
    std::unique_ptr<char[]> buff(new char[entry_size_]);

    in_.seekg(pos*entry_size_);
    in_.read(buff.get(), entry_size_);

    return unserializer(buff.get(), entry_size_);
  }
 private:
  std::ifstream in_;
  std::ofstream out_;
  /**
   * Size in byte of each entry
   */
  size_t entry_size_;
  /**
   * Array of '\0'
   */
  char *tail_;
  /**
   * Number of elements in the array
   */
  size_t size_;
};

}  // namespace secondary_storage
#endif  // INCLUDE_SECONDARY_STORAGE_FIXED_ARRAY_H_
