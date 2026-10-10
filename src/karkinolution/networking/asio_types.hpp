#include <asio/ip/tcp.hpp>

using Acceptor      = asio::ip::tcp::acceptor;
using Socket        = asio::ip::tcp::socket;
using IoContext     = asio::io_context;
using Port          = unsigned short;
using AsioErrorCode = asio::error_code;
using EndPoint      = asio::ip::tcp::endpoint;
using AsioResolver  = asio::ip::tcp::resolver;