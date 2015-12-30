#include <tip/db/pg/sqlstates.hpp>
#include <map>

namespace tip {
namespace db {
namespace pg {
namespace sqlstate {

namespace {
const std::map<std::string, code> CODESTR_TO_STATE {
		//@{
		/** @name Class 00 — Successful Completion */
		{ "00000", successful_completion },
		//@}
		//@{
		/** @name Class 01 — Warning */
		{ "01000", warning },
		{ "0100C", dynamic_result_sets_returned },
		{ "01008", implicit_zero_bit_padding },
		{ "01003", null_value_eliminated_in_set_function },
		{ "01007", privilege_not_granted },
		{ "01006", privilege_not_revoked },
		{ "01004", string_data_right_truncation },
		{ "01P01", deprecated_feature },
		//@}
		//@{
		/** @name Class 02 — No Data (this is also a warning class per the SQL standard) */
		{ "02000", no_data },
		{ "02001", no_additional_dynamic_result_sets_returned },
		//@}
		//@{
		/** @name Class 03 — SQL Statement Not Yet Complete */
		{ "03000", sql_statement_not_yet_complete },
		//@}
		//@{
		/** @name Class 08 — Connection Exception */
		{ "08000", connection_exception },
		{ "08003", connection_does_not_exist },
		{ "08006", connection_failure },
		{ "08001", sqlclient_unable_to_establish_sqlconnection },
		{ "08004", sqlserver_rejected_establishment_of_sqlconnection },
		{ "08007", transaction_resolution_unknown },
		{ "08P01", protocol_violation },
		//@}
		//@{
		/** @name Class 09 — Triggered Action Exception */
		{ "09000", triggered_action_exception },
		//@}
		//@{
		/** @name Class 0A — Feature Not Supported */
		{ "0A000", feature_not_supported },
		//@}
		//@{
		/** @name Class 0B — Invalid Transaction Initiation */
		{ "0B000", invalid_transaction_initiation },
		//@}
		//@{
		/** @name Class 0F — Locator Exception */
		{ "0F000", locator_exception },
		{ "0F001", invalid_locator_specification },
		//@}
		//@{
		/** @name Class 0L — Invalid Grantor */
		{ "0L000", invalid_grantor },
		{ "0LP01", invalid_grant_operation },
		//@}
		//@{
		/** @name Class 0P — Invalid Role Specification */
		{ "0P000", invalid_role_specification },
		//@}
		//@{
		/** @name Class 0Z — Diagnostics Exception */
		{ "0Z000", diagnostics_exception },
		{ "0Z002", stacked_diagnostics_accessed_without_active_handler },
		//@}
		//@{
		/** @name Class 20 — Case Not Found */
		{ "20000", case_not_found },
		//@}
		//@{
		/** @name Class 21 — Cardinality Violation */
		{ "21000", cardinality_violation },
		//@}
		//@{
		/** @name Class 22 — Data Exception */
		{ "22000", data_exception },
		{ "2202E", array_subscript_error },
		{ "22021", character_not_in_repertoire },
		{ "22008", datetime_field_overflow },
		{ "22012", division_by_zero },
		{ "22005", error_in_assignment },
		{ "2200B", escape_character_conflict },
		{ "22022", indicator_overflow },
		{ "22015", interval_field_overflow },
		{ "2201E", invalid_argument_for_logarithm },
		{ "22014", invalid_argument_for_ntile_function },
		{ "22016", invalid_argument_for_nth_value_function },
		{ "2201F", invalid_argument_for_power_function },
		{ "2201G", invalid_argument_for_width_bucket_function },
		{ "22018", invalid_character_value_for_cast },
		{ "22007", invalid_datetime_format },
		{ "22019", invalid_escape_character },
		{ "2200D", invalid_escape_octet },
		{ "22025", invalid_escape_sequence },
		{ "22P06", nonstandard_use_of_escape_character },
		{ "22010", invalid_indicator_parameter_value },
		{ "22023", invalid_parameter_value },
		{ "2201B", invalid_regular_expression },
		{ "2201W", invalid_row_count_in_limit_clause },
		{ "2201X", invalid_row_count_in_result_offset_clause },
		{ "22009", invalid_time_zone_displacement_value },
		{ "2200C", invalid_use_of_escape_character },
		{ "2200G", most_specific_type_mismatch },
		{ "22004", null_value_not_allowed },
		{ "22002", null_value_no_indicator_parameter },
		{ "22003", numeric_value_out_of_range },
		{ "22026", string_data_length_mismatch },
		{ "22001", string_data_right_truncation },
		{ "22011", substring_error },
		{ "22027", trim_error },
		{ "22024", unterminated_c_string },
		{ "2200F", zero_length_character_string },
		{ "22P01", floating_point_exception },
		{ "22P02", invalid_text_representation },
		{ "22P03", invalid_binary_representation },
		{ "22P04", bad_copy_file_format },
		{ "22P05", untranslatable_character },
		{ "2200L", not_an_xml_document },
		{ "2200M", invalid_xml_document },
		{ "2200N", invalid_xml_content },
		{ "2200S", invalid_xml_comment },
		{ "2200T", invalid_xml_processing_instruction },
		//@}
		//@{
		/** @name Class 23 — Integrity Constraint Violation */
		{ "23000", integrity_constraint_violation },
		{ "23001", restrict_violation },
		{ "23502", not_null_violation },
		{ "23503", foreign_key_violation },
		{ "23505", unique_violation },
		{ "23514", check_violation },
		{ "23P01", exclusion_violation },
		//@}
		//@{
		/** @name Class 24 — Invalid Cursor State */
		{ "24000", invalid_cursor_state },
		//@}
		//@{
		/** @name Class 25 — Invalid Transaction State */
		{ "25000", invalid_transaction_state },
		{ "25001", active_sql_transaction },
		{ "25002", branch_transaction_already_active },
		{ "25008", held_cursor_requires_same_isolation_level },
		{ "25003", inappropriate_access_mode_for_branch_transaction },
		{ "25004", inappropriate_isolation_level_for_branch_transaction },
		{ "25005", no_active_sql_transaction_for_branch_transaction },
		{ "25006", read_only_sql_transaction },
		{ "25007", schema_and_data_statement_mixing_not_supported },
		{ "25P01", no_active_sql_transaction },
		{ "25P02", in_failed_sql_transaction },
		//@}
		//@{
		/** @name Class 26 — Invalid SQL Statement Name */
		{ "26000", invalid_sql_statement_name },
		//@}
		//@{
		/** @name Class 27 — Triggered Data Change Violation */
		{ "27000", triggered_data_change_violation },
		//@}
		//@{
		/** @name Class 28 — Invalid Authorization Specification */
		{ "28000", invalid_authorization_specification },
		{ "28P01", invalid_password },
		//@}
		//@{
		/** @name Class 2B — Dependent Privilege Descriptors Still Exist */
		{ "2B000", dependent_privilege_descriptors_still_exist },
		{ "2BP01", dependent_objects_still_exist },
		//@}
		//@{
		/** @name Class 2D — Invalid Transaction Termination */
		{ "2D000", invalid_transaction_termination },
		//@}
		//@{
		/** @name Class 2F — SQL Routine Exception */
		{ "2F000", sql_routine_exception },
		{ "2F005", function_executed_no_return_statement },
		{ "2F002", modifying_sql_data_not_permitted },
		{ "2F003", prohibited_sql_statement_attempted },
		{ "2F004", reading_sql_data_not_permitted },
		//@}
		//@{
		/** @name Class 34 — Invalid Cursor Name */
		{ "34000", invalid_cursor_name },
		//@}
		//@{
		/** @name Class 38 — External Routine Exception */
		{ "38000", external_routine_exception },
		{ "38001", containing_sql_not_permitted },
		{ "38002", modifying_sql_data_not_permitted },
		{ "38003", prohibited_sql_statement_attempted },
		{ "38004", reading_sql_data_not_permitted },
		//@}
		//@{
		/** @name Class 39 — External Routine Invocation Exception */
		{ "39000", external_routine_invocation_exception },
		{ "39001", invalid_sqlstate_returned },
		{ "39004", null_value_not_allowed },
		{ "39P01", trigger_protocol_violated },
		{ "39P02", srf_protocol_violated },
		//@}
		//@{
		/** @name Class 3B — Savepoint Exception */
		{ "3B000", savepoint_exception },
		{ "3B001", invalid_savepoint_specification },
		//@}
		//@{
		/** @name Class 3D — Invalid Catalog Name */
		{ "3D000", invalid_catalog_name },
		//@}
		//@{
		/** @name Class 3F — Invalid Schema Name */
		{ "3F000", invalid_schema_name },
		//@}
		//@{
		/** @name Class 40 — Transaction Rollback */
		{ "40000", transaction_rollback },
		{ "40002", transaction_integrity_constraint_violation },
		{ "40001", serialization_failure },
		{ "40003", statement_completion_unknown },
		{ "40P01", deadlock_detected },
		//@}
		//@{
		/** @name Class 42 — Syntax Error or Access Rule Violation */
		{ "42000", syntax_error_or_access_rule_violation },
		{ "42601", syntax_error },
		{ "42501", insufficient_privilege },
		{ "42846", cannot_coerce },
		{ "42803", grouping_error },
		{ "42P20", windowing_error },
		{ "42P19", invalid_recursion },
		{ "42830", invalid_foreign_key },
		{ "42602", invalid_name },
		{ "42622", name_too_long },
		{ "42939", reserved_name },
		{ "42804", datatype_mismatch },
		{ "42P18", indeterminate_datatype },
		{ "42P21", collation_mismatch },
		{ "42P22", indeterminate_collation },
		{ "42809", wrong_object_type },
		{ "42703", undefined_column },
		{ "42883", undefined_function },
		{ "42P01", undefined_table },
		{ "42P02", undefined_parameter },
		{ "42704", undefined_object },
		{ "42701", duplicate_column },
		{ "42P03", duplicate_cursor },
		{ "42P04", duplicate_database },
		{ "42723", duplicate_function },
		{ "42P05", duplicate_prepared_statement },
		{ "42P06", duplicate_schema },
		{ "42P07", duplicate_table },
		{ "42712", duplicate_alias },
		{ "42710", duplicate_object },
		{ "42702", ambiguous_column },
		{ "42725", ambiguous_function },
		{ "42P08", ambiguous_parameter },
		{ "42P09", ambiguous_alias },
		{ "42P10", invalid_column_reference },
		{ "42611", invalid_column_definition },
		{ "42P11", invalid_cursor_definition },
		{ "42P12", invalid_database_definition },
		{ "42P13", invalid_function_definition },
		{ "42P14", invalid_prepared_statement_definition },
		{ "42P15", invalid_schema_definition },
		{ "42P16", invalid_table_definition },
		{ "42P17", invalid_object_definition },
		//@}
		//@{
		/** @name Class 44 — WITH CHECK OPTION Violation */
		{ "44000", with_check_option_violation },
		//@}
		//@{
		/** @name Class 53 — Insufficient Resources */
		{ "53000", insufficient_resources },
		{ "53100", disk_full },
		{ "53200", out_of_memory },
		{ "53300", too_many_connections },
		{ "53400", configuration_limit_exceeded },
		//@}
		//@{
		/** @name Class 54 — Program Limit Exceeded */
		{ "54000", program_limit_exceeded },
		{ "54001", statement_too_complex },
		{ "54011", too_many_columns },
		{ "54023", too_many_arguments },
		//@}
		//@{
		/** @name Class 55 — Object Not In Prerequisite State */
		{ "55000", object_not_in_prerequisite_state },
		{ "55006", object_in_use },
		{ "55P02", cant_change_runtime_param },
		{ "55P03", lock_not_available },
		//@}
		//@{
		/** @name Class 57 — Operator Intervention */
		{ "57000", operator_intervention },
		{ "57014", query_canceled },
		{ "57P01", admin_shutdown },
		{ "57P02", crash_shutdown },
		{ "57P03", cannot_connect_now },
		{ "57P04", database_dropped },
		{ "58000", system_error },
		{ "58030", io_error },
		{ "58P01", undefined_file },
		{ "58P02", duplicate_file },
		//@}
		//@{
		/** @name Class F0 — Configuration File Error */
		{ "F0000", config_file_error },
		{ "F0001", lock_file_exists },
		//@}
		//@{
		/** @name Class HV — Foreign Data Wrapper Error (SQL/MED) */
		{ "HV000", fdw_error },
		{ "HV005", fdw_column_name_not_found },
		{ "HV002", fdw_dynamic_parameter_value_needed },
		{ "HV010", fdw_function_sequence_error },
		{ "HV021", fdw_inconsistent_descriptor_information },
		{ "HV024", fdw_invalid_attribute_value },
		{ "HV007", fdw_invalid_column_name },
		{ "HV008", fdw_invalid_column_number },
		{ "HV004", fdw_invalid_data_type },
		{ "HV006", fdw_invalid_data_type_descriptors },
		{ "HV091", fdw_invalid_descriptor_field_identifier },
		{ "HV00B", fdw_invalid_handle },
		{ "HV00C", fdw_invalid_option_index },
		{ "HV00D", fdw_invalid_option_name },
		{ "HV090", fdw_invalid_string_length_or_buffer_length },
		{ "HV00A", fdw_invalid_string_format },
		{ "HV009", fdw_invalid_use_of_null_pointer },
		{ "HV014", fdw_too_many_handles },
		{ "HV001", fdw_out_of_memory },
		{ "HV00P", fdw_no_schemas },
		{ "HV00J", fdw_option_name_not_found },
		{ "HV00K", fdw_reply_handle },
		{ "HV00Q", fdw_schema_not_found },
		{ "HV00R", fdw_table_not_found },
		{ "HV00L", fdw_unable_to_create_execution },
		{ "HV00M", fdw_unable_to_create_reply },
		{ "HV00N", fdw_unable_to_establish_connection },
		//@}
		//@{
		/** @name Class P0 — PL/pgSQL Error */
		{ "P0000", plpgsql_error },
		{ "P0001", raise_exception },
		{ "P0002", no_data_found },
		{ "P0003", too_many_rows },
		//@}
		//@{
		/** @name Class XX — Internal Error */
		{ "XX000", internal_error },
		{ "XX001", data_corrupted },
		{ "XX002", index_corrupted },
};

const std::map<code, std::string> STATE_TO_CODESTR {
		//@{
		/** @name Class 00 — Successful Completion */
		{ successful_completion, "00000" },
		//@}
		//@{
		/** @name Class 01 — Warning */
		{ warning, "01000" },
		{ dynamic_result_sets_returned, "0100C" },
		{ implicit_zero_bit_padding, "01008" },
		{ null_value_eliminated_in_set_function, "01003" },
		{ privilege_not_granted, "01007" },
		{ privilege_not_revoked, "01006" },
		{ string_data_right_truncation, "01004" },
		{ deprecated_feature, "01P01" },
		//@}
		//@{
		/** @name Class 02 — No Data (this is also a warning class per the SQL standard) */
		{ no_data, "02000" },
		{ no_additional_dynamic_result_sets_returned, "02001" },
		//@}
		//@{
		/** @name Class 03 — SQL Statement Not Yet Complete */
		{ sql_statement_not_yet_complete, "03000" },
		//@}
		//@{
		/** @name Class 08 — Connection Exception */
		{ connection_exception, "08000" },
		{ connection_does_not_exist, "08003" },
		{ connection_failure, "08006" },
		{ sqlclient_unable_to_establish_sqlconnection, "08001" },
		{ sqlserver_rejected_establishment_of_sqlconnection, "08004" },
		{ transaction_resolution_unknown, "08007" },
		{ protocol_violation, "08P01" },
		//@}
		//@{
		/** @name Class 09 — Triggered Action Exception */
		{ triggered_action_exception, "09000" },
		//@}
		//@{
		/** @name Class 0A — Feature Not Supported */
		{ feature_not_supported, "0A000" },
		//@}
		//@{
		/** @name Class 0B — Invalid Transaction Initiation */
		{ invalid_transaction_initiation, "0B000" },
		//@}
		//@{
		/** @name Class 0F — Locator Exception */
		{ locator_exception, "0F000" },
		{ invalid_locator_specification, "0F001" },
		//@}
		//@{
		/** @name Class 0L — Invalid Grantor */
		{ invalid_grantor, "0L000" }, { invalid_grant_operation, "0LP01" },
		//@}
		//@{
		/** @name Class 0P — Invalid Role Specification */
		{ invalid_role_specification, "0P000" },
		//@}
		//@{
		/** @name Class 0Z — Diagnostics Exception */
		{ diagnostics_exception, "0Z000" },
		{ stacked_diagnostics_accessed_without_active_handler, "0Z002" },
		//@}
		//@{
		/** @name Class 20 — Case Not Found */
		{ case_not_found, "20000" },
		//@}
		//@{
		/** @name Class 21 — Cardinality Violation */
		{ cardinality_violation, "21000" },
		//@}
		//@{
		/** @name Class 22 — Data Exception */
		{ data_exception, "22000" },
		{ array_subscript_error, "2202E" },
		{ character_not_in_repertoire, "22021" },
		{ datetime_field_overflow, "22008" },
		{ division_by_zero, "22012" },
		{ error_in_assignment, "22005" },
		{ escape_character_conflict, "2200B" },
		{ indicator_overflow, "22022" },
		{ interval_field_overflow, "22015" },
		{ invalid_argument_for_logarithm, "2201E" },
		{ invalid_argument_for_ntile_function, "22014" },
		{ invalid_argument_for_nth_value_function, "22016" },
		{ invalid_argument_for_power_function, "2201F" },
		{ invalid_argument_for_width_bucket_function, "2201G" },
		{ invalid_character_value_for_cast, "22018" },
		{ invalid_datetime_format, "22007" },
		{ invalid_escape_character, "22019" },
		{ invalid_escape_octet, "2200D" },
		{ invalid_escape_sequence, "22025" },
		{ nonstandard_use_of_escape_character, "22P06" },
		{ invalid_indicator_parameter_value, "22010" },
		{ invalid_parameter_value, "22023" },
		{ invalid_regular_expression, "2201B" },
		{ invalid_row_count_in_limit_clause, "2201W" },
		{ invalid_row_count_in_result_offset_clause, "2201X" },
		{ invalid_time_zone_displacement_value, "22009" },
		{ invalid_use_of_escape_character, "2200C" },
		{ most_specific_type_mismatch, "2200G" },
		{ null_value_not_allowed, "22004" },
		{ null_value_no_indicator_parameter, "22002" },
		{ numeric_value_out_of_range, "22003" },
		{ string_data_length_mismatch, "22026" },
		{ string_data_right_truncation, "22001" },
		{ substring_error, "22011" },
		{ trim_error, "22027" },
		{ unterminated_c_string, "22024" },
		{ zero_length_character_string, "2200F" },
		{ floating_point_exception, "22P01" },
		{ invalid_text_representation, "22P02" },
		{ invalid_binary_representation, "22P03" },
		{ bad_copy_file_format, "22P04" },
		{ untranslatable_character, "22P05" },
		{ not_an_xml_document, "2200L" },
		{ invalid_xml_document, "2200M" },
		{ invalid_xml_content, "2200N" },
		{ invalid_xml_comment, "2200S" },
		{ invalid_xml_processing_instruction, "2200T" },
		//@}
		//@{
		/** @name Class 23 — Integrity Constraint Violation */
		{ integrity_constraint_violation, "23000" },
		{ restrict_violation, "23001" },
		{ not_null_violation, "23502" },
		{ foreign_key_violation, "23503" },
		{ unique_violation, "23505" },
		{ check_violation, "23514" },
		{ exclusion_violation, "23P01" },
		//@}
		//@{
		/** @name Class 24 — Invalid Cursor State */
		{ invalid_cursor_state, "24000" },
		//@}
		//@{
		/** @name Class 25 — Invalid Transaction State */
		{ invalid_transaction_state, "25000" },
		{ active_sql_transaction, "25001" },
		{ branch_transaction_already_active, "25002" },
		{ held_cursor_requires_same_isolation_level, "25008" },
		{ inappropriate_access_mode_for_branch_transaction, "25003" },
		{ inappropriate_isolation_level_for_branch_transaction, "25004" },
		{ no_active_sql_transaction_for_branch_transaction, "25005" },
		{ read_only_sql_transaction, "25006" },
		{ schema_and_data_statement_mixing_not_supported, "25007" },
		{ no_active_sql_transaction, "25P01" },
		{ in_failed_sql_transaction, "25P02" },
		//@}
		//@{
		/** @name Class 26 — Invalid SQL Statement Name */
		{ invalid_sql_statement_name, "26000" },
		//@}
		//@{
		/** @name Class 27 — Triggered Data Change Violation */
		{ triggered_data_change_violation, "27000" },
		//@}
		//@{
		/** @name Class 28 — Invalid Authorization Specification */
		{ invalid_authorization_specification, "28000" },
		{ invalid_password, "28P01" },
		//@}
		//@{
		/** @name Class 2B — Dependent Privilege Descriptors Still Exist */
		{ dependent_privilege_descriptors_still_exist, "2B000" },
		{ dependent_objects_still_exist, "2BP01" },
		//@}
		//@{
		/** @name Class 2D — Invalid Transaction Termination */
		{ invalid_transaction_termination, "2D000" },
		//@}
		//@{
		/** @name Class 2F — SQL Routine Exception */
		{ sql_routine_exception, "2F000" },
		{ function_executed_no_return_statement, "2F005" },
		{ modifying_sql_data_not_permitted, "2F002" },
		{ prohibited_sql_statement_attempted, "2F003" },
		{ reading_sql_data_not_permitted, "2F004" },
		//@}
		//@{
		/** @name Class 34 — Invalid Cursor Name */
		{ invalid_cursor_name, "34000" },
		//@}
		//@{
		/** @name Class 38 — External Routine Exception */
		{ external_routine_exception, "38000" },
		{ containing_sql_not_permitted, "38001" },
		{ modifying_sql_data_not_permitted, "38002" },
		{ prohibited_sql_statement_attempted, "38003" },
		{ reading_sql_data_not_permitted, "38004" },
		//@}
		//@{
		/** @name Class 39 — External Routine Invocation Exception */
		{ external_routine_invocation_exception, "39000" },
		{ invalid_sqlstate_returned, "39001" },
		{ null_value_not_allowed, "39004" },
		{ trigger_protocol_violated, "39P01" },
		{ srf_protocol_violated, "39P02" },
		//@}
		//@{
		/** @name Class 3B — Savepoint Exception */
		{ savepoint_exception, "3B000" },
		{ invalid_savepoint_specification, "3B001" },
		//@}
		//@{
		/** @name Class 3D — Invalid Catalog Name */
		{ invalid_catalog_name, "3D000" },
		//@}
		//@{
		/** @name Class 3F — Invalid Schema Name */
		{ invalid_schema_name, "3F000" },
		//@}
		//@{
		/** @name Class 40 — Transaction Rollback */
		{ transaction_rollback, "40000" },
		{ transaction_integrity_constraint_violation, "40002" },
		{ serialization_failure, "40001" },
		{ statement_completion_unknown, "40003" },
		{ deadlock_detected, "40P01" },
		//@}
		//@{
		/** @name Class 42 — Syntax Error or Access Rule Violation */
		{ syntax_error_or_access_rule_violation, "42000" },
		{ syntax_error, "42601" },
		{ insufficient_privilege, "42501" },
		{ cannot_coerce, "42846" },
		{ grouping_error, "42803" },
		{ windowing_error, "42P20" },
		{ invalid_recursion, "42P19" },
		{ invalid_foreign_key, "42830" },
		{ invalid_name, "42602" },
		{ name_too_long, "42622" },
		{ reserved_name, "42939" },
		{ datatype_mismatch, "42804" },
		{ indeterminate_datatype, "42P18" },
		{ collation_mismatch, "42P21" },
		{ indeterminate_collation, "42P22" },
		{ wrong_object_type, "42809" },
		{ undefined_column, "42703" },
		{ undefined_function, "42883" },
		{ undefined_table, "42P01" },
		{ undefined_parameter, "42P02" },
		{ undefined_object, "42704" },
		{ duplicate_column, "42701" },
		{ duplicate_cursor, "42P03" },
		{ duplicate_database, "42P04" },
		{ duplicate_function, "42723" },
		{ duplicate_prepared_statement, "42P05" },
		{ duplicate_schema, "42P06" },
		{ duplicate_table, "42P07" },
		{ duplicate_alias, "42712" },
		{ duplicate_object, "42710" },
		{ ambiguous_column, "42702" },
		{ ambiguous_function, "42725" },
		{ ambiguous_parameter, "42P08" },
		{ ambiguous_alias, "42P09" },
		{ invalid_column_reference, "42P10" },
		{ invalid_column_definition, "42611" },
		{ invalid_cursor_definition, "42P11" },
		{ invalid_database_definition, "42P12" },
		{ invalid_function_definition, "42P13" },
		{ invalid_prepared_statement_definition, "42P14" },
		{ invalid_schema_definition, "42P15" },
		{ invalid_table_definition, "42P16" },
		{ invalid_object_definition, "42P17" },
		//@}
		//@{
		/** @name Class 44 — WITH CHECK OPTION Violation */
		{ with_check_option_violation, "44000" },
		//@}
		//@{
		/** @name Class 53 — Insufficient Resources */
		{ insufficient_resources, "53000" },
		{ disk_full, "53100" },
		{ out_of_memory, "53200" },
		{ too_many_connections, "53300" },
		{ configuration_limit_exceeded, "53400" },
		//@}
		//@{
		/** @name Class 54 — Program Limit Exceeded */
		{ program_limit_exceeded, "54000" },
		{ statement_too_complex, "54001" },
		{ too_many_columns, "54011" },
		{ too_many_arguments, "54023" },
		//@}
		//@{
		/** @name Class 55 — Object Not In Prerequisite State */
		{ object_not_in_prerequisite_state, "55000" },
		{ object_in_use, "55006" },
		{ cant_change_runtime_param, "55P02" },
		{ lock_not_available, "55P03" },
		//@}
		//@{
		/** @name Class 57 — Operator Intervention */
		{ operator_intervention, "57000" },
		{ query_canceled, "57014" },
		{ admin_shutdown, "57P01" },
		{ crash_shutdown, "57P02" },
		{ cannot_connect_now, "57P03" },
		{ database_dropped, "57P04" },
		{ system_error, "58000" },
		{ io_error, "58030" },
		{ undefined_file, "58P01" },
		{ duplicate_file, "58P02" },
		//@}
		//@{
		/** @name Class F0 — Configuration File Error */
		{ config_file_error, "F0000" },
		{ lock_file_exists, "F0001" },
		//@}
		//@{
		/** @name Class HV — Foreign Data Wrapper Error (SQL/MED) */
		{ fdw_error, "HV000" },
		{ fdw_column_name_not_found, "HV005" },
		{ fdw_dynamic_parameter_value_needed, "HV002" },
		{ fdw_function_sequence_error, "HV010" },
		{ fdw_inconsistent_descriptor_information, "HV021" },
		{ fdw_invalid_attribute_value, "HV024" },
		{ fdw_invalid_column_name, "HV007" },
		{ fdw_invalid_column_number, "HV008" },
		{ fdw_invalid_data_type, "HV004" },
		{ fdw_invalid_data_type_descriptors, "HV006" },
		{ fdw_invalid_descriptor_field_identifier, "HV091" },
		{ fdw_invalid_handle, "HV00B" },
		{ fdw_invalid_option_index, "HV00C" },
		{ fdw_invalid_option_name, "HV00D" },
		{ fdw_invalid_string_length_or_buffer_length, "HV090" },
		{ fdw_invalid_string_format, "HV00A" },
		{ fdw_invalid_use_of_null_pointer, "HV009" },
		{ fdw_too_many_handles, "HV014" },
		{ fdw_out_of_memory, "HV001" },
		{ fdw_no_schemas, "HV00P" },
		{ fdw_option_name_not_found, "HV00J" },
		{ fdw_reply_handle, "HV00K" },
		{ fdw_schema_not_found, "HV00Q" },
		{ fdw_table_not_found, "HV00R" },
		{ fdw_unable_to_create_execution, "HV00L" },
		{ fdw_unable_to_create_reply, "HV00M" },
		{ fdw_unable_to_establish_connection, "HV00N" },
		//@}
		//@{
		/** @name Class P0 — PL/pgSQL Error */
		{ plpgsql_error, "P0000" },
		{ raise_exception, "P0001" },
		{ no_data_found, "P0002" },
		{ too_many_rows, "P0003" },
		//@}
		//@{
		/** @name Class XX — Internal Error */
		{ internal_error, "XX000" },
		{ data_corrupted, "XX001" },
		{ index_corrupted, "XX002" },
};

} // namespace

code
code_to_state(std::string const& val)
{
	auto f = CODESTR_TO_STATE.find(val);
	if (f != CODESTR_TO_STATE.end()) {
		return f->second;
	}
	return unknown_code;
}

}  // namespace sqlstate
}  // namespace pg
}  // namespace db
}  // namespace tip
