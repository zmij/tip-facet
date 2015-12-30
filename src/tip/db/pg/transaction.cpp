/*
 * connection_lock.cpp
 *
 *  Created on: 14 июля 2015 г.
 *     @author: zmij
 */

#include <tip/db/pg/transaction.hpp>
#include <tip/db/pg/resultset.hpp>
#include <tip/db/pg/detail/basic_connection.hpp>
#include <tip/db/pg/log.hpp>

namespace tip {
namespace db {
namespace pg {

LOCAL_LOGGING_FACILITY_CFG(PGTRAN, config::QUERY_LOG);

transaction::transaction(connection_ptr conn)
	: connection_(conn), finished_(false)
{
}

transaction::~transaction()
{
	if (!finished_ && connection_->in_transaction()) {
		local_log(logger::WARNING) << "Transaction object abandoned, rolling back";
		connection_->rollback();
	}
}

dbalias const&
transaction::alias() const
{
	return connection_->alias();
}

bool
transaction::in_transaction() const
{
	return connection_->in_transaction();
}

void
transaction::commit(notification_callback cb)
{
	finished_ = true;
	connection_->commit(cb);
}

void
transaction::rollback(notification_callback cb)
{
	finished_ = true;
	connection_->rollback(cb);
}
void
transaction::execute(std::string const& query, query_result_callback result,
		query_error_callback error)
{
	connection_->execute(events::execute{
		query,
		std::bind(&transaction::handle_results, shared_from_this(),
				std::placeholders::_1, std::placeholders::_2, result),
		std::bind(&transaction::handle_query_error, shared_from_this(),
				std::placeholders::_1, error)
	});
}
void
transaction::execute(std::string const& query, type_oid_sequence const& param_types,
		std::vector< byte > params_buffer,
		query_result_callback result, query_error_callback error)
{
	connection_->execute(events::execute_prepared{
		query, param_types, params_buffer,
		std::bind(&transaction::handle_results, shared_from_this(),
				std::placeholders::_1, std::placeholders::_2, result),
		std::bind(&transaction::handle_query_error, shared_from_this(),
				std::placeholders::_1, error)
	});
}

void
transaction::handle_results(resultset r, bool complete, query_result_callback result)
{
	if (result) {
		result(shared_from_this(), r, complete);
	}
}

void
transaction::handle_query_error(error::query_error const& e, query_error_callback error)
{
	if (error) {
		error(e);
	}
}

} /* namespace pg */
} /* namespace db */
} /* namespace tip */
