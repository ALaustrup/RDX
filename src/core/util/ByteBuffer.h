#ifndef RDX_BYTEBUFFER_H
#define RDX_BYTEBUFFER_H

#include <vector>
#include <cstddef>
#include <span>
#include <cstring>

namespace rdx::core {

class ByteBuffer {
public:
    ByteBuffer() = default;
    explicit ByteBuffer(std::size_t initialCapacity);
    explicit ByteBuffer(std::span<const std::byte> data);
    
    void append(std::span<const std::byte> data);
    void append(const void* data, std::size_t size);
    void appendByte(std::byte b);
    
    void reserve(std::size_t capacity);
    void clear();
    
    std::size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }
    
    std::span<const std::byte> data() const { return data_; }
    std::span<std::byte> mutableData() { return data_; }
    
    const std::byte* dataPtr() const { return data_.data(); }
    std::byte* mutableDataPtr() { return data_.data(); }
    
    void resize(std::size_t newSize);
    
private:
    std::vector<std::byte> data_;
};

} // namespace rdx::core

#endif // RDX_BYTEBUFFER_H

