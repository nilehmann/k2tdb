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
#include <iostream>

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
  FixedArray(const path &file_name, size_t size, bool trunc = false)
      : stream_(),
        entry_size_(size),
        tail_(new char[entry_size_]),
        size_(0) {
    // Create if it doesn't exists, append if does.
    auto ops = std::ios::out | std::ios::binary;
    if (!trunc)
      ops |= std::ios::app;
    stream_.open(file_name.native(), ops);
    stream_.close();

    // Open for read and write
    stream_.open(file_name.native(),
                 std::ios::out | std::ios::in | std::ios::binary);

    std::fill(tail_, tail_ + entry_size_, '\0');
    stream_.seekg(0, std::ofstream::end);
    size_ = stream_.tellg()/entry_size_;
  }
  ~FixedArray() {
    delete [] tail_;
    stream_.close();
  }
  size_t PushBack(const T& val) {
    assert(GetSize()(val) <= entry_size_);

    Serialize serializer;
    size_t size;
    const char *serialization = serializer(val, &size);

    stream_.seekp(0, std::ofstream::end);
    stream_.write(serialization, size);
    stream_.write(tail_, entry_size_ - size);
    size_++;

    return size_;
  }

  size_t Size() {
    return size_;
  }

  T operator[](size_t pos) const {
    Unserialize unserializer;
    std::unique_ptr<char[]> buff(new char[entry_size_]);

    const_cast<std::fstream&>(stream_).seekg(pos*entry_size_);
    const_cast<std::fstream&>(stream_).read(buff.get(), entry_size_);

    return unserializer(buff.get(), entry_size_);
  }

 private:
  std::fstream stream_;
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
