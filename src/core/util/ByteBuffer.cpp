#include "util/ByteBuffer.h"

namespace rdx::core {

ByteBuffer::ByteBuffer(std::size_t initialCapacity) {
    data_.reserve(initialCapacity);
}

ByteBuffer::ByteBuffer(std::span<const std::byte> data) {
    data_.reserve(data.size());
    for (const std::byte& b : data) {
        data_.push_back(b);
    }
}

void ByteBuffer::append(std::span<const std::byte> data) {
    data_.insert(data_.end(), data.begin(), data.end());
}

void ByteBuffer::append(const void* data, std::size_t size) {
    const auto* bytes = static_cast<const std::byte*>(data);
    data_.insert(data_.end(), bytes, bytes + size);
}

void ByteBuffer::appendByte(std::byte b) {
    data_.push_back(b);
}

void ByteBuffer::reserve(std::size_t capacity) {
    data_.reserve(capacity);
}

void ByteBuffer::clear() {
    data_.clear();
}

void ByteBuffer::resize(std::size_t newSize) {
    data_.resize(newSize);
}

} // namespace rdx::core

