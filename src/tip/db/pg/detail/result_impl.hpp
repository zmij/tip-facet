/*
 * result_impl.hpp
 *
 *  Created on: 12 июля 2015 г.
 *     @author: zmij
 */

#ifndef TIP_DB_PG_DETAIL_RESULT_IMPL_HPP_
#define TIP_DB_PG_DETAIL_RESULT_IMPL_HPP_

#include <tip/db/pg/common.hpp>
#include <tip/db/pg/detail/protocol.hpp>
#include <vector>

namespace tip {
namespace db {
namespace pg {
namespace detail {

class result_impl {
public:
	typedef std::vector<row_data> row_set_type;
public:
	result_impl();

	row_description_type&
	row_description()
	{ return row_description_; }
	row_description_type const&
	row_description() const
	{ return row_description_; }

	row_set_type&
	rows()
	{ return rows_; }

	size_t
	size() const;

	bool
	empty() const;

	field_buffer
	at(uinteger row, usmallint col) const;

	row_data::data_buffer_bounds
	buffer_bounds(uinteger row, usmallint col) const;

	bool
	is_null(uinteger row, usmallint col) const;
private:
	void
	check_row_index(uinteger row) const;
	row_description_type row_description_;
	row_set_type rows_;
};

} /* namespace detail */
} /* namespace pg */
} /* namespace db */
} /* namespace tip */

#endif /* TIP_DB_PG_DETAIL_RESULT_IMPL_HPP_ */
