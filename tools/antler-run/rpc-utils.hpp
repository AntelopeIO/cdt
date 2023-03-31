#pragma once

#include "utils.hpp"

#include <google/protobuf/io/zero_copy_stream.h>
#include <boost/asio/buffer.hpp>
#include <boost/beast/core/buffer_traits.hpp>


namespace eosio { namespace testing {
/**
 * idea taken from https://github.com/boostorg/beast/issues/1180 
 * Implements ZeroCopyOutputStream around a AsioBuf.
 * AsioBuf matches the public interface defined by boost::asio::streambuf.
 * @tparam AsioBuf A type meeting the requirements of AsioBuf.
*/
template <class AsioBuf>
class zero_copy_output_stream : public google::protobuf::io::ZeroCopyOutputStream {
private:
    using mutable_buffers_type = typename AsioBuf::mutable_buffers_type;
    using iterator = typename boost::beast::buffers_iterator_type<mutable_buffers_type>;

    AsioBuf& streambuf;
    std::size_t mutable_buf_size;
    google::protobuf::int64 commited = 0;
    std::size_t bytes_to_commit = 0;
    iterator cur_pos = nullptr;

    inline void commit() {
        if (bytes_to_commit != 0) {
            streambuf.commit(bytes_to_commit);
            commited += bytes_to_commit;
            bytes_to_commit = 0;
        }
    }
public:
    explicit zero_copy_output_stream(AsioBuf& streambuf, std::size_t blockSize): 
          streambuf(streambuf)
        , mutable_buf_size(blockSize) {}
    ~zero_copy_output_stream() { commit(); }

    bool Next(void** data, int* size) override {
        ANTLER_ASSERT(data, "data pointer is null");
        ANTLER_ASSERT(size, "{}: size pointer is null");

        commit();

        auto cur_pos = boost::asio::buffer_sequence_begin(streambuf.prepare(mutable_buf_size));

        *data = boost::asio::buffer_cast<void*>(*cur_pos);
        *size = static_cast<int>(boost::asio::buffer_size(*cur_pos));
        bytes_to_commit = *size;
        
        return true;
    }

    void BackUp(int count) override {
        ANTLER_ASSERT(count <= bytes_to_commit, "count greater than bytes to commit");
        bytes_to_commit -= count;
        commit();
    }

    google::protobuf::int64 ByteCount() const override {
        return commited;
    }
};

template <class AsioBuf>
class zero_copy_input_stream : public google::protobuf::io::ZeroCopyInputStream {
private:
    using mutable_buffers_type = typename AsioBuf::mutable_buffers_type;
    using iterator = typename boost::beast::buffers_iterator_type<mutable_buffers_type>;

    AsioBuf& streambuf;
    size_t bytes_to_consume = 0;
    google::protobuf::int64 total_consumed = 0;
    inline void consume(bool skip = false) {
        if (bytes_to_consume) {
            streambuf.consume(bytes_to_consume);
            if (!skip)
                total_consumed += bytes_to_consume;
            bytes_to_consume = 0;
        }
    }
public:
    explicit zero_copy_input_stream(AsioBuf& buf) : streambuf(buf) {}
    ~zero_copy_input_stream() { consume(); }

    bool Next(const void** data, int* size) override {
        ANTLER_ASSERT(data, "data pointer is null");
        ANTLER_ASSERT(size, "{}: size pointer is null");

        consume();

        auto cur_pos = boost::asio::buffer_sequence_begin(streambuf.data());

        *data = boost::asio::buffer_cast<void*>(*cur_pos);
        *size = static_cast<int>(boost::asio::buffer_size(*cur_pos));
        bytes_to_consume = *size;

        if (++cur_pos == boost::asio::buffer_sequence_end(streambuf.data())) {
            return false;
        }

        return true;
    }
    void BackUp(int count) override {
        ANTLER_ASSERT(count <= bytes_to_consume, "count greater than bytes to consume");
        bytes_to_consume -= count;
        consume();
    }
    bool Skip(int count) override {
        consume();
        bytes_to_consume = count;
        consume(true);
    }
    int64_t ByteCount() const override {
        return total_consumed;
    }
};

}} // eosio::testing