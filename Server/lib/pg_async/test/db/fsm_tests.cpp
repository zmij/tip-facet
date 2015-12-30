/*
 * fsm_tests.cpp
 *
 *  Created on: Jul 29, 2015
 *      Author: zmij
 */

#include <gtest/gtest.h>
#include <tip/db/pg/detail/connection_fsm.hpp>
#include <tip/db/pg/detail/transport.hpp>
#include <tip/db/pg/query.hpp>

#include <tip/db/pg/log.hpp>
#include "db/config.hpp"
#include "test-environment.hpp"

namespace asio_config = tip::db::pg::asio_config;

LOCAL_LOGGING_FACILITY_FUNC(PGFSM, TRACE, test_log);

namespace tip {
namespace db {
namespace pg {
namespace detail {

struct dummy_transport {
	typedef asio_config::io_service_ptr io_service_ptr;
	typedef std::function< void (asio_config::error_code const&) > connect_callback;

	dummy_transport(io_service_ptr svc) {}

	void
	connect_async(connection_options const&, connect_callback cb)
	{
		asio_config::error_code ec;
		cb(ec);
	}

	bool
	connected() const
	{
		return true;
	}

	void
	close()
	{

	}

	template < typename BufferType, typename HandlerType >
	void
	async_read(BufferType& buffer, HandlerType handler)
	{
		test_log() << "Dummy async read";
	}

	template < typename BufferType, typename HandlerType >
	void
	async_write(BufferType const& buffer, HandlerType handler)
	{
		test_log() << "Dummy async write";
	}

};
}  // namespace detail
}  // namespace pg
}  // namespace db
}  // namespace tip

using namespace tip::db::pg::detail;
using namespace tip::db::pg::events;

typedef concrete_connection<dummy_transport> fsm;
typedef std::shared_ptr<fsm> fsm_ptr;
tip::db::pg::client_options_type client_options {
	{"client_encoding", 		"UTF8"},
	{"application_name", 		"pg_async"},
	//{"autocommit", 				"off"},
	{"client_min_messages", 	"debug5"}
};

TEST(DummyFSM, NormalFlow)
{
	::asio_config::io_service_ptr svc(std::make_shared<::asio_config::io_service>());
	fsm_ptr c( new fsm(svc, client_options, {}) );
	c->start();
	//	Connection
	c->process_event("main=tcp://user:password@localhost:5432[db]"_pg);		// unplugged 	-> t_conn
	c->process_event(ready_for_query());	// authn		-> idle

	// Begin transaction
	c->process_event(begin());		// idle			-> transaction::starting
	c->process_event(ready_for_query());	// transaction::starting -> transaction::idle

	// Commit transaction
	c->process_event(commit());		// transaction::idle	-> transaction::exiting
	c->process_event(ready_for_query());	// transaction::exiting	-> idle

	// Begin transaction
	c->process_event(begin());		// idle			-> transaction::starting
	c->process_event(ready_for_query());	// transaction::starting -> transaction::idle

	c->process_event(complete());	// transaction::exiting	-> idle

	// Rollback transaction
	c->process_event(rollback());	// transaction::idle	-> transaction::exiting
	c->process_event(ready_for_query());	// transaction::exiting	-> idle

	// Terminate connection
	c->process_event(terminate());	// idle -> X
}

TEST(DummyFSM, TerminateTran)
{
	::asio_config::io_service_ptr svc(std::make_shared<::asio_config::io_service>());
	fsm_ptr c( new fsm(svc, client_options, {}) );
	c->start();
	//	Connection
	c->process_event("main=tcp://user:password@localhost:5432[db]"_pg);		// unplugged 	-> t_conn
	c->process_event(ready_for_query());	// authn		-> idle

	// Begin transaction
	c->process_event(begin());		// idle			-> transaction::starting
	c->process_event(ready_for_query());	// transaction::starting -> transaction::idle

	c->process_event(terminate());	// deferred event

	c->process_event(rollback());	// transaction::idle -> transaction::exiting
	c->process_event(ready_for_query());	// transaction::exiting -> idle
}

TEST(DummyFSM, SimpleQueryMode)
{
	::asio_config::io_service_ptr svc(std::make_shared<::asio_config::io_service>());
	fsm_ptr c( new fsm(svc, client_options, {}) );
	c->start();
	//	Connection
	c->process_event("main=tcp://user:password@localhost:5432[db]"_pg);		// unplugged 	-> t_conn
	c->process_event(ready_for_query());	// authn		-> idle

	// Begin transaction
	c->process_event(begin());		// idle			-> transaction::starting
	c->process_event(ready_for_query());	// transaction::starting -> transaction::idle

	c->process_event(execute{ "bla" });		// transaction -> simple query
	c->process_event(row_description()); // waiting -> fetch_data
	for (int i = 0; i < 10; ++i) {
		c->process_event(row_data());
	}
	c->process_event(complete());	// fetch_data -> waiting

	c->process_event(row_description()); // waiting -> fetch_data
	for (int i = 0; i < 10; ++i) {
		c->process_event(row_data());
	}
	c->process_event(complete());	// fetch_data -> waiting

	c->process_event(ready_for_query()); // simple query -> transaction::idle
	c->process_event(commit());		// transaction::idle -> transaction::exiting
	c->process_event(ready_for_query());	// transaction -> idle
}

TEST(DummyFSM, ExtendedQueryMode)
{
	::asio_config::io_service_ptr svc(std::make_shared<::asio_config::io_service>());
	fsm_ptr c( new fsm(svc, client_options, {}) );
	c->start();
	//	Connection
	c->process_event("main=tcp://user:password@localhost:5432[db]"_pg);		// unplugged 	-> t_conn
	c->process_event(ready_for_query());	// authn		-> idle

	// Begin transaction
	c->process_event(begin());		// idle			-> transaction::starting
	c->process_event(ready_for_query());	// transaction::starting -> transaction::idle

	// Start extended query mode
	c->process_event(execute_prepared()); // transaction::idle -> eqm::prepare -> parse
	c->process_event(complete());	// parse -> bind
	c->process_event(complete());	// bind -> exec
}

template < typename TransportType >
void
test_normal_flow(tip::db::pg::connection_options const& opts)
{
	using namespace tip::db::pg;
	typedef concrete_connection< TransportType > fsm_type;
	typedef std::shared_ptr< fsm_type > fsm_ptr;

	::asio_config::io_service_ptr svc(std::make_shared<::asio_config::io_service>());
	fsm_ptr c(new fsm_type(svc, client_options, {}));

	c->start();
	c->process_event(opts);
	c->process_event(begin());
	c->process_event(execute{ "select * from pg_catalog.pg_type; select * from pg_catalog.pg_class" });
	c->process_event(execute{ "create temporary table dummy (id bigint)" });
	c->process_event(commit());
	c->process_event(terminate());

	svc->run();
}

TEST(FSM, NormalFlow)
{
	using namespace tip::db::pg;
	if (! test::environment::test_database.empty() ) {
		connection_options opts = connection_options::parse(test::environment::test_database);

		if (opts.schema == "tcp") {
			test_normal_flow< tcp_transport >(opts);
		} else if (opts.schema == "socket") {
			test_normal_flow< socket_transport >(opts);
		}
	}
}

template < typename TransportType >
void
test_preliminary_terminate(tip::db::pg::connection_options const& opts)
{
	using namespace tip::db::pg;
	typedef concrete_connection< TransportType > fsm_type;
	typedef std::shared_ptr< fsm_type > fsm_ptr;

	::asio_config::io_service_ptr svc(std::make_shared<::asio_config::io_service>());
	fsm_ptr c(new fsm_type(svc, client_options, {}));

	c->start();
	c->process_event(opts);
	c->process_event(begin());
	c->process_event(terminate());
	c->process_event(rollback());

	svc->run();
}

TEST(FSM, PreliminaryTerminate)
{
	using namespace tip::db::pg;
	if (! test::environment::test_database.empty() ) {
		connection_options opts = connection_options::parse(test::environment::test_database);

		if (opts.schema == "tcp") {
			test_preliminary_terminate< tcp_transport >(opts);
		} else if (opts.schema == "socket") {
			test_preliminary_terminate< socket_transport >(opts);
		}
	}
}

template < typename TransportType >
void
test_error_in_query(tip::db::pg::connection_options const& opts)
{
	using namespace tip::db::pg;
	typedef concrete_connection< TransportType > fsm_type;
	typedef std::shared_ptr< fsm_type > fsm_ptr;

	::asio_config::io_service_ptr svc(std::make_shared<::asio_config::io_service>());
	fsm_ptr c(new fsm_type(svc, client_options, {}));

	c->start();
	c->process_event(opts);
	c->process_event(begin());
	c->process_event(begin());
	c->process_event(execute{ "select * from _shouldnt_be_there_" });
	c->process_event(terminate());

	svc->run();
}

TEST(FSM, ErrorInSimpleQuery)
{
	using namespace tip::db::pg;
	if (! test::environment::test_database.empty() ) {
		connection_options opts = connection_options::parse(test::environment::test_database);

		if (opts.schema == "tcp") {
			test_error_in_query< tcp_transport >(opts);
		} else if (opts.schema == "socket") {
			test_error_in_query< socket_transport >(opts);
		}
	}
}

template < typename TransportType >
void
test_exec_prepared(tip::db::pg::connection_options const& opts)
{
	using namespace tip::db::pg;
	typedef concrete_connection< TransportType > fsm_type;
	typedef std::shared_ptr< fsm_type > fsm_ptr;
	typedef std::vector< char > buffer_type;

	::asio_config::io_service_ptr svc(std::make_shared<::asio_config::io_service>());
	fsm_ptr c(new fsm_type(svc, client_options, {}));

	c->start();
	c->process_event(opts);
	c->process_event(begin());

	c->process_event(execute_prepared{
		"select * from pg_catalog.pg_type"
	});

	c->process_event(events::execute{
		"create temporary table test_exec_prepared (id bigint, name text)"
	});

	{
		type_oid_sequence param_types;
		buffer_type params;
		tip::db::pg::detail::write_params(param_types, params, 100500, std::string("foo"));
		c->process_event(execute_prepared{
			"insert into test_exec_prepared(id, name) values ($1, $2)",
			param_types,
			params
		});
	}
	{
		type_oid_sequence param_types;
		buffer_type params;
		tip::db::pg::detail::write_params(param_types, params, 100501, std::string("bar"));
		c->process_event(execute_prepared{
			"insert into test_exec_prepared(id, name) values ($1, $2)",
			param_types,
			params
		});
	}

	c->process_event(commit());
	c->process_event(terminate());

	svc->run();
}

TEST(FSM, ExecPrepared)
{
	using namespace tip::db::pg;
	if (! test::environment::test_database.empty() ) {
		connection_options opts = connection_options::parse(test::environment::test_database);

		if (opts.schema == "tcp") {
			test_exec_prepared< tcp_transport >(opts);
		} else if (opts.schema == "socket") {
			test_exec_prepared< socket_transport >(opts);
		}
	}
}

