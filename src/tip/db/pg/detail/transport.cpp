/*
 * transport.cpp
 *
 *  Created on: Jul 30, 2015
 *      Author: zmij
 */

#include <tip/db/pg/detail/transport.hpp>
#include <tip/db/pg/error.hpp>

#include <tip/db/pg/log.hpp>

#include <boost/bind.hpp>

namespace tip {
namespace db {
namespace pg {
namespace detail {

LOCAL_LOGGING_FACILITY_CFG(POSTGRE, config::CONNECTION_LOG);

//****************************************************************************
// tcp_layer
tcp_transport::tcp_transport(io_service_ptr service) :
		resolver_(*service), socket(*service)
{
}

void
tcp_transport::connect_async(connection_options const& conn, connect_callback cb)
{
	if (conn.uri.empty()) {
		throw error::connection_error("No connection uri!");
	}
	if (conn.schema != "tcp") {
		throw error::connection_error("Wrong connection schema for TCP transport");
	}
	std::string host = conn.uri;
	std::string svc = "5432";
	std::string::size_type pos = conn.uri.find(":");
	if (pos != std::string::npos) {
		host = conn.uri.substr(0, pos);
		svc = conn.uri.substr(pos + 1);
	}

	tcp::resolver::query query (host, svc);
	resolver_.async_resolve(query, boost::bind(&tcp_transport::handle_resolve,
				this, _1, _2, cb ));

}

void
tcp_transport::handle_resolve(error_code const& ec,
			tcp::resolver::iterator endpoint_iterator, connect_callback cb)
{
	if (!ec) {
		ASIO_NAMESPACE::async_connect(socket, endpoint_iterator,
				boost::bind( &tcp_transport::handle_connect,
						this, _1, cb));
	} else {
		cb(ec);
	}
}

void
tcp_transport::handle_connect(error_code const& ec, connect_callback cb)
{
	cb(ec);
}

bool
tcp_transport::connected() const
{
	return socket.is_open();
}

void
tcp_transport::close()
{
	if (socket.is_open())
		socket.close();
}

//----------------------------------------------------------------------------
// socket_transport implementation
//----------------------------------------------------------------------------
socket_transport::socket_transport(io_service_ptr service)
	: socket(*service)
{
}

void
socket_transport::connect_async(connection_options const& conn,
		connect_callback cb)
{
	using asio_config::stream_protocol;
	if (conn.schema != "socket") {
		throw error::connection_error("Wrong connection schema for TCP transport");
	}
	std::string uri = conn.uri;

	if (uri.empty()) {
		local_log(logger::WARNING) << "Socket name is empty. Trying default";
		uri = "/tmp/.s.PGSQL.5432";
	}
	socket.async_connect(stream_protocol::endpoint(uri),
			[cb](error_code const& ec) { cb(ec); });
}

bool
socket_transport::connected() const
{
	return socket.is_open();
}

void
socket_transport::close()
{
	if (socket.is_open())
		socket.close();
}

}  // namespace detail
}  // namespace pg
}  // namespace db
}  // namespace tip


