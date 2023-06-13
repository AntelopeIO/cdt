#pragma once

#include "version.hpp"
#include "utils.hpp"
#include "rpc-utils.hpp"

#include <native/eosio/intrinsics.hpp>
#include <eosio/datastream.hpp>
#include <bluegrass/meta/for_each.hpp>
#include <pb/antler-run.pb.h>

// macro defined in eosiolib collides with enum value in boost::beast
#ifdef TABLE
#pragma push_macro("TABLE")
#undef TABLE
#endif

#include <boost/beast.hpp>
#include <boost/asio.hpp>

#include <chrono>
#include <memory>
#include <utility>
#include <future>
#include <functional>
#include <thread>
#include <unordered_map>
#include <atomic>
#include <mutex>
#include <type_traits>
#include <condition_variable>
#include <optional>
#include <signal.h>

// restoring eosiolib macro
#ifndef TABLE
#pragma pop_macro("TABLE")
#endif

namespace eosio { namespace testing {

class sync_call_traits {
public:
    template<typename Callback>
    using callback = std::packaged_task<Callback>;
};

/// @brief connection class that connects to nodeos and processes debug execution messages
///        interface is synchroneous, actual reads and writes are done in a separate thread
///        class designed to always listen for incoming messages and write them to read queue
///        every request and response must have same request_id
///        in case server sends response for a different request, class leaves it in the read queue
/// @tparam CallbackTraits 
template<typename CallbackTraits = sync_call_traits>
class connection : public std::enable_shared_from_this<connection<CallbackTraits>>{
    using io_context        = boost::asio::io_context;
    using asio_strand       = boost::asio::strand<boost::asio::io_context::executor_type>;
    using tcp_stream        = boost::beast::websocket::stream<boost::beast::tcp_stream>;
    using ip_resolver       = boost::asio::ip::tcp::resolver;
    using error_code        = boost::beast::error_code;
    using stream_base       = boost::beast::websocket::stream_base;
    using data_buffer       = boost::beast::flat_buffer;
    using request_type      = boost::beast::websocket::request_type;
    using close_code        = boost::beast::websocket::close_code;
    using read_queue_type   = std::unordered_map<uint64_t, antler_rpc::antler_run_response>;
    using on_event          = typename CallbackTraits::callback<void()>;

    //order is important as fields are using one another in constructor
    io_context              context;
    ip_resolver             resolver;
    tcp_stream              stream;
    data_buffer             write_buffer;
    data_buffer             read_buffer;
    std::string             host;
    std::string             port;
    on_event                connected;
    std::mutex              reading;
    std::condition_variable new_response;
    std::mutex              writing;
    read_queue_type         read_queue;
    std::mutex              read_queue_mutex;
public:
    using on_connected = on_event;

    connection(const std::string& host_, const std::string& port_, on_connected&& connected_) 
    : resolver(context.get_executor())
             , stream(context.get_executor())
             , host(host_)
             , port(port_)
             , connected(std::move(connected_)) {}

    template<typename ReqType, typename = std::enable_if_t<std::is_base_of_v<google::protobuf::Message, ReqType>> >
    auto sync_request(ReqType&& req) {
        ANTLER_ASSERT( !in_context_thread(), "code must be called outside connection thread" );

        auto req_id = req.req_id();
        boost::asio::post(context.get_executor(), 
                                [r = std::move(req), c = shared_from_this()](){ 
                                    c->write(r); 
                                });
        return get_response(req_id);
    }

    std::optional<antler_rpc::antler_run_response> wait_for_divert_flow() {
        using namespace std::chrono_literals;

        ANTLER_ASSERT( !in_context_thread(), "code must be called outside connection thread" );
        
        while (!closing()) {
            {
                std::lock_guard<decltype(read_queue_mutex)> lock(read_queue_mutex);
                for (auto it = read_queue.begin(); it != read_queue.end(); ++it) {
                    if (it->second.has_divert_flow_result()) {
                        antler_rpc::antler_run_response ret = std::move(it->second);
                        read_queue.erase(it);
                        return std::optional(std::move(ret));
                    }
                }
            }
            std::unique_lock<decltype(read_queue_mutex)> wait_lock(read_queue_mutex);
            new_response.wait_for(wait_lock, 1s, [&](){ return !read_queue.empty(); });
        }

        // we are here if connection was closed
        return {};
    }

    void close() {
        ANTLER_ASSERT( !in_context_thread(), "code must be called outside connection thread" );
        if (closing()) {
            ANTLER_INFO("close called on non-opened or already closed connection, doing nothing");
            return;
        }

        boost::asio::post(context.get_executor(),
                            [c = shared_from_this()](){
                                c->stream.close(close_code::normal);
                            });
        // get_response returns control flow when stream is closed. return value is empty optional<>
        // so we ensure here we got empty response
        ANTLER_ASSERT(!get_response(0).has_value(), "no response expected");
    }

    inline bool closing() {
        return stream.is_open();
    }

    void run() {
        // blocking until closed
        context.run();
    }
    void start_connection() {
        boost::asio::post(context.get_executor(), 
                         [c = shared_from_this()](){
                            c->connect();
                         });
    }
private:

    template<typename Handler>
    inline auto bind_front_this(Handler&& f) {
        return boost::beast::bind_front_handler(std::forward<Handler>(f), shared_from_this());
    }

    void on_resolve(error_code ec, ip_resolver::results_type results){
        ANTLER_ASSERT(!ec, "{}", ec.message());

        // Set the timeout for the operation
        boost::beast::get_lowest_layer(stream).expires_after(std::chrono::seconds(30));

        // Make the connection on the IP address we get from a lookup
        boost::beast::get_lowest_layer(stream).async_connect(results, 
                                                             bind_front_this(&connection::on_connect));
    }
    void on_connect(error_code ec, ip_resolver::results_type::endpoint_type ep){
        ANTLER_ASSERT(!ec, "{}", ec.message());

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
                                                        bind_front_this(&connection::on_handshake));
    }
    void on_handshake(error_code ec) {
        ANTLER_ASSERT(!ec, "{}", ec.message());

        start_read();
        connected();
    }
    template <typename ReqType>
    void write(const ReqType& req) {
        ANTLER_ASSERT( in_context_thread(), "code must be called inside connection thread" );
        ANTLER_ASSERT( writing.try_lock(), "current implementation is single-threaded so \
                                            only one write operation is permitted at the same time. \
                                            If you need this functionality you should implement write queue \
                                            or multi-socket connection (new thread = new socket)" );
         
        const auto msg_size = req.ByteSizeLong();
        {
            zero_copy_output_stream<decltype(write_buffer)> ostream(write_buffer, msg_size);
            req.SerializeToZeroCopyStream(&ostream);
        }

        stream.async_write(write_buffer.data(), bind_front_this(&connection::on_write));
    }
    void on_write(error_code ec, size_t) {
        std::unique_lock<decltype(writing)>(writing, std::adopt_lock);
        ANTLER_ASSERT(!ec, "{}", ec.message());

        write_buffer.consume(write_buffer.size());
    }
    void on_read(error_code ec, std::size_t) {
        antler_rpc::antler_run_response response;
        {
            std::unique_lock<decltype(reading)>(reading, std::adopt_lock);
            ANTLER_ASSERT(!ec, "{}", ec.message());

            zero_copy_input_stream<decltype(read_buffer)> istream(read_buffer);
            response.ParseFromZeroCopyStream(&istream);
        }
        {
            std::lock_guard<decltype(read_queue_mutex)> lock(read_queue_mutex);
            read_queue[response.req_id()] = std::move(response);
        }
        new_response.notify_all();

        start_read();
    }
    void start_read() {
        ANTLER_ASSERT( in_context_thread(), "code must be called inside connection thread" );
        ANTLER_ASSERT( reading.try_lock(), "current implementation is single-threaded so \
                                            only one read operation is permitted at the same time. \
                                            If you need this functionality you should implement read queue \
                                            or multi-socket connection (new thread = new socket)" );
        stream.async_read(read_buffer, bind_front_this(&connection::on_read));
    }
    template<typename ReqType>
    auto post_request(ReqType&& req) {
        return boost::asio::post(context.get_executor(), 
                                [r = std::move(req), c = shared_from_this()](){ 
                                    c->write(r); 
                                });
    }
    void connect() {
        ANTLER_ASSERT( !stream.is_open(), "already connected" );
        ANTLER_ASSERT( in_context_thread(), "code must be called inside connection thread" );
        // Look up the domain name
        resolver.async_resolve(
            host,
            port,
            bind_front_this(&connection::on_resolve));
    }
    inline bool in_context_thread() {
        return context.get_executor().running_in_this_thread();
    }

    std::optional<antler_rpc::antler_run_response> get_response(uint64_t id) {
        using namespace std::chrono_literals;

        ANTLER_ASSERT( !in_context_thread(), "code must be called outside connection thread" );
        
        while (!closing()) {
            {
                std::lock_guard<decltype(read_queue_mutex)> lock(read_queue_mutex);
                auto it = read_queue.find(id);
                if (it != read_queue.end()) {
                    antler_rpc::antler_run_response ret = std::move(it->second);
                    read_queue.erase(it);
                    return std::optional(std::move(ret));
                }
            }
            std::unique_lock<decltype(read_queue_mutex)> wait_lock(read_queue_mutex);
            new_response.wait_for(wait_lock, 1s, [&](){ return !read_queue.empty(); });
        }

        // we are here if connection was closed
        return {};
    }
};

/// @brief client that exposes debug RPC.
///        it maintains single thread for network operations where connection class is running
class node_client {
private:
    using connection = connection<>;
    std::shared_ptr<connection>      node_connection;
    std::thread                      context_thread;
    eosio::name                      registered_name;
    sighandler_t                     previous_signal;

    node_client() : previous_signal(0x0) {}

    static inline uint64_t generate_req_id() {
        static uint64_t counter = 0;
        return ++counter;
    }
    static void sigint_handler(int sig) {
        node_client::get().unregister_account(node_client::get().registered_name);
        node_client::get().close();

        if (node_client::get().previous_signal)
            node_client::get().previous_signal(sig);
    }
    void run() {
        context_thread = std::thread([&](){
            // blocks until connection is closed
            node_connection->run();
        });
    }
public:
    
    void init(const std::string& host, const std::string& port) {
        connection::on_connected connected;
        auto future = connected.get_future();

        node_connection.reset( new connection(host, port, std::move(connected)) );
        run();
        node_connection->start_connection();

        //TODO: test this
        auto signal_ret = signal(SIGINT, &node_client::sigint_handler);
        ANTLER_ASSERT(signal_ret != SIG_ERR, "error setting signal: {}", std::strerror(errno));
        // if noone set signal, return address supposed to be zero. this warning indicates that we override someone else's signal
        if (signal_ret != 0x0) {
            previous_signal = signal_ret;
            ANTLER_INFO("Overriding previously set SIGINT signal. Previous handler will be called at the end of current one");
        }

        //wait until handshake
        future.get();
    }
    static inline node_client& get() {
        static node_client client;
        return client;
    }

    void register_account(eosio::name acc_name) {
        auto request = create_generic_request();

        request.mutable_register_message()->set_account(acc_name.value);

        //sync_request will check return code, as long as it doesn't throw we good to return
        std::ignore = sync_request(std::move(request));

        registered_name = acc_name;
    }

    void unregister_account(eosio::name acc_name) {
        auto request = create_generic_request();

        request.mutable_unregister_message()->set_account(acc_name.value);

        //sync_request will check return code, as long as it doesn't throw we good to return
        std::ignore = sync_request(std::move(request));
    }

    inline std::optional<antler_rpc::antler_run_response> wait_for_divert_flow() {
        return node_connection->wait_for_divert_flow();
    }

    void set_time(int64_t new_time) {
        auto request = create_generic_request();

        request.mutable_set_time_message()->set_time(new_time);

        //sync_request will check return code, as long as it doesn't throw we good to return
        std::ignore = sync_request(std::move(request));
    }

    int64_t get_time() {
        auto request = create_generic_request();

        std::ignore = request.mutable_get_time_message();

        auto response = sync_request(std::move(request));
        ANTLER_ASSERT(response.has_get_time_result(), "get time result should be in place");

        return response.get_time_result().time();
    }

    template<int64_t ID, typename Ret, typename... Args>
    Ret call_intrinsic(Args... args) {
        auto request = create_generic_request();

        request.set_req_id(generate_req_id());
        request.mutable_call_intrinsic_message()->set_id(ID);
        request.mutable_call_intrinsic_message()->set_data(serialize_args(args...));
        
        //call CallHostFunction RPC synchronously and get response
        auto response = sync_request(std::move(request));
        ANTLER_ASSERT(response.has_call_intrinsic_result(), "call intrinsic result is absent");

        const auto& intrinsic_result = response.call_intrinsic_result();
        const auto& output_params    = intrinsic_result.data();
        if (!output_params.empty()) {
            //deserialize response back into args if any
            deserialize_output_parameters(output_params.data(), std::make_tuple(std::forward<Args>(args)...));   
        }

        if constexpr (!std::is_same_v<void, Ret>) {
            //deserialize intrinsic return value if any
            return deserialize_return_value<Ret>(intrinsic_result.ret_val());
        }
    }

    template<typename Ret, typename... Args>
    Ret call_action(Args... args) {
        auto request = create_generic_request();
        request.mutable_call_action_message()->set_data(serialize_args(args...));

        //call CallAction RPC synchronously and get response
        auto response = sync_request(std::move(request));
        ANTLER_ASSERT(response.has_call_action_result(), "call action result is absent");

        const auto& action_result = response.call_action_result();
        const auto& output_params = action_result.data();
        if (!output_params.empty()) {
            //deserialize response back into args if any
            deserialize_output_parameters(output_params.data(), std::make_tuple(std::forward<Args>(args)...));   
        }

        if constexpr (!std::is_same_v<void, Ret>) {
            //deserialize intrinsic return value if any
            return deserialize_return_value<Ret>(action_result.ret_val());
        }
    }

    void return_control_flow(uint64_t id) {
        antler_rpc::antler_run_request request;
        // id should match req_id we got from divert_flow
        request.set_req_id(id);

        std::ignore = sync_request(std::move(request));
    }

    void close() {
        //syncroneous
        node_connection->close();
    }

private:
    template<typename ReqType>
    antler_rpc::antler_run_response sync_request(ReqType&& request) {
        auto response = node_connection->sync_request(std::move(request));

        ANTLER_QUIT(!node_connection->closing(), "connection to node was closed, exiting");
        ANTLER_ASSERT(response, "error getting response from node");
        ANTLER_ASSERT(response->ret_code() == 0, "request {} call has failed", response->DebugString());

        return *response;
    }

    antler_rpc::antler_run_request create_generic_request() const {
        antler_rpc::antler_run_request request;
        request.set_req_id(generate_req_id());

        return request;
    }

    template<typename... Args>
    std::string serialize_args(Args... args) {
        std::string tmp_buf;
        size_t  buf_size = 0;
        if constexpr (std::index_sequence_for<Args...>::size()) {
            buf_size = (eosio::pack_size(args) + ...);
            tmp_buf.resize(buf_size);
            
            eosio::datastream<const char*> ds(tmp_buf.data(), tmp_buf.size());
            //serialize args into vector of bytes
            (ds << ... << args);
        }

        return tmp_buf;
    }

    template<typename... Args>
    void deserialize_output_parameters(const std::string& data, std::tuple<Args...>&& t) {
        eosio::datastream<const char*> ds(data.data(), data.size());
        bluegrass::meta::for_each(t, [&](auto v){
            if constexpr (std::is_pointer_v<decltype(v)> && !std::is_const_v<std::remove_pointer_t<decltype(v)>>) {
                //void* and char* are exceptions, everything else is serialized via datastream
                // if you'll encounter errors here, update datastream to deserialize type you have issues with
                if constexpr (std::is_same_v<void*, decltype(v)> || std::is_same_v<char*, decltype(v)>) {
                    std::vector<char> tmp;
                    ds >> tmp;
                    memcpy((void*)v, tmp.data(), tmp.size());
                } else {
                    ds >> *v;
                }
            }
        });
    }

    template<typename Ret>
    Ret deserialize_return_value(const std::string& serialized_data) {
        eosio::datastream<const char*> ds(serialized_data.data(), serialized_data.size());
        Ret ret_val;
        ds >> ret_val;

        return std::move(ret_val);
    }
};

}} //eosio::testing
