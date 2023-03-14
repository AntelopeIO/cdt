#pragma once

#include "version.hpp"
#include "utils.hpp"

#include <eosio/intrinsics.hpp>

// macro defined in eosiolib collides with enum value in boost::beast
#ifdef TABLE
#pragma push_macro("TABLE")
#undef TABLE
#endif

#include <boost/beast.hpp>
#include <boost/asio.hpp>

#include <chrono>

// restoring eosiolib macro
#ifndef TABLE
#pragma pop_macro("TABLE")
#endif

class node_client  : public std::enable_shared_from_this<node_client>{
    using io_context        = boost::asio::io_context;
    using tcp_stream        = boost::beast::websocket::stream<boost::beast::tcp_stream>;
    using ip_resolver       = boost::asio::ip::tcp::resolver;
    using data_buffer       = boost::beast::flat_buffer;
    using error_code        = boost::beast::error_code;
    using stream_base       = boost::beast::websocket::stream_base;
    using request_type      = boost::beast::websocket::request_type;

    io_context    context;
    ip_resolver   resolver;
    tcp_stream    stream;
    data_buffer   buffer;
    std::string   host;
    std::string   port;

    template<typename Handler>
    inline auto bind_front_this(Handler&& f) {
        return boost::beast::bind_front_handler(std::forward<Handler>(f), shared_from_this());
    }
public:
    using on_connected = std::function<void ()>;
    node_client(const std::string& host_, const std::string& port_) 
    : resolver(boost::asio::make_strand(context)),
      stream(boost::asio::make_strand(context)),
      host(host_),
      port(port_) {
    }

    void connect() {
        // Look up the domain name
        resolver.async_resolve(
            host,
            port,
            bind_front_this(&node_client::on_resolve));
    }

    template<int64_t ID, typename... Args>
    auto call_intrinsic(Args... args) -> decltype(declval(eosio::native::intrinsics::get_intrinsic<ID>())::result_type) {
        //serialize args into vector of bytes
        //call CallHostFunction RPC synchronously
        //deserialize response into args

        return {};
    }

private:
    void on_resolve(error_code ec, ip_resolver::results_type results){
        ANTLER_ASSERT(!ec, "{}: {}", __func__, ec.message());

        // Set the timeout for the operation
        boost::beast::get_lowest_layer(stream).expires_after(std::chrono::seconds(30));

        // Make the connection on the IP address we get from a lookup
        boost::beast::get_lowest_layer(stream)
                     .async_connect(results,
                                    bind_front_this(&node_client::on_connect));
    }

    void on_connect(error_code ec, ip_resolver::results_type::endpoint_type ep){
        ANTLER_ASSERT(!ec, "{}: {}", __func__, ec.message());

        // Turn off the timeout on the tcp_stream, because
        // the websocket stream has its own timeout system.
        boost::beast::get_lowest_layer(stream).expires_never();

        // Set suggested timeout settings for the websocket
        stream.set_option(stream_base::timeout::suggested(boost::beast::role_type::client));

        // Set a decorator to change the User-Agent of the handshake
        stream.set_option(stream_base::decorator(
            [](request_type& req)
            {
                req.set(boost::beast::http::field::user_agent,
                        eosio::utils::log_to_str("antler-run client {}", app_version()));
            }));

        // Perform the websocket handshake
        stream.async_handshake(eosio::utils::log_to_str("{}:{}", host, ep.port()), 
                                                        "/",
                                                        bind_front_this(&node_client::on_handshake));
    }

    void on_handshake(error_code ec) {
        ANTLER_ASSERT(!ec, "{}: {}", __func__, ec.message());


    }
};