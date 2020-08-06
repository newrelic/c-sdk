/**
 * @file libnewrelic.h
 *
 * @brief This is the New Relic C SDK! If your application does not use other
 * New Relic APM agent languages, you can use the C SDK to take advantage of
 * New Relic's monitoring capabilities and features to instrument a wide range
 * of applications.
 *
 * See accompanying GUIDE.md and LICENSE.txt for more information.
 */
#ifndef LIBNEWRELIC_H
#define LIBNEWRELIC_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Datastore product names with query instrumentation support
 *
 * The product field of the newrelic_datastore_segment_params_t specifies
 * the datastore type of a datastore segment. For example, "MySQL" indicates
 * that the segment's query is against a MySQL database. New Relic recommends
 * using the predefined NEWRELIC_DATASTORE_FIREBIRD through
 * NEWRELIC_DATASTORE_SYBASE constants for this field.
 *
 * For the SQL-like datastores which are supported by the C SDK, when the
 * record_sql field of the newrelic_transaction_tracer_config_t is set
 * to NEWRELIC_SQL_RAW or NEWRELIC_SQL_OBFUSCATED, the query param of the
 * newrelic_datastore_segment_config_t is reported to
 * New Relic.
 */
//!@{
#define NEWRELIC_DATASTORE_FIREBIRD "Firebird"
#define NEWRELIC_DATASTORE_INFORMIX "Informix"
#define NEWRELIC_DATASTORE_MSSQL "MSSQL"
#define NEWRELIC_DATASTORE_MYSQL "MySQL"
#define NEWRELIC_DATASTORE_ORACLE "Oracle"
#define NEWRELIC_DATASTORE_POSTGRES "Postgres"
#define NEWRELIC_DATASTORE_SQLITE "SQLite"
#define NEWRELIC_DATASTORE_SYBASE "Sybase"
//@}

/**
 * @name Datastore product names without query instrumentation support
 *
 * The product field of newrelic_datastore_segment_params_t specifies
 * the datastore type of a datastore segment.
 *
 * If the product field points to a string that is one of
 * NEWRELIC_DATASTORE_MEMCACHE through NEWRELIC_DATASTORE_OTHER, the resulting
 * datastore segment shall be instrumented as an unsupported datastore; this
 * datastore's query language has no obfuscation support and its query cannot
 * securely be reported to New Relic.
 */
//!@{
#define NEWRELIC_DATASTORE_MEMCACHE "Memcached"
#define NEWRELIC_DATASTORE_MONGODB "MongoDB"
#define NEWRELIC_DATASTORE_ODBC "ODBC"
#define NEWRELIC_DATASTORE_REDIS "Redis"
#define NEWRELIC_DATASTORE_OTHER "Other"
//@}

/**
 * @name Transport types for distributed tracing instrumentation
 *
 * The function newrelic_accept_distributed_trace_payload() accepts
 * a transport type which represents the type of connection used to
 * transmit the trace payload. These transport names are used across
 * New Relic agents and are used by the UI to group traces.
 */
//!@{
#define NEWRELIC_TRANSPORT_TYPE_UNKNOWN "Unknown"
#define NEWRELIC_TRANSPORT_TYPE_HTTP "HTTP"
#define NEWRELIC_TRANSPORT_TYPE_HTTPS "HTTPS"
#define NEWRELIC_TRANSPORT_TYPE_KAFKA "Kafka"
#define NEWRELIC_TRANSPORT_TYPE_JMS "JMS"
#define NEWRELIC_TRANSPORT_TYPE_IRONMQ "IronMQ"
#define NEWRELIC_TRANSPORT_TYPE_AMQP "AMQP"
#define NEWRELIC_TRANSPORT_TYPE_QUEUE "Queue"
#define NEWRELIC_TRANSPORT_TYPE_OTHER "Other"
//@}

/**
 * @brief A New Relic application. Once an application configuration is
 * created with newrelic_create_app_config(), call newrelic_create_app()
 * to create an application to report data to the daemon; the daemon,
 * in turn, reports data to New Relic.
 *
 * @see newrelic_create_app().
 */
typedef struct _nr_app_and_info_t newrelic_app_t;

/**
 * @brief A New Relic transaction.
 *
 * A transaction is started using newrelic_start_web_transaction() or
 * newrelic_start_non_web_transaction(). A started, or active, transaction is
 * stopped using newrelic_end_transaction(). One may modify a transaction
 * by adding custom attributes or recording errors only after it has been
 * started.
 */
typedef struct _newrelic_txn_t newrelic_txn_t;

/**
 * @brief A time, measured in microseconds.
 *
 * C SDK configuration and API calls with time-related parameters expect this
 * type. For an example configuration, see
 * newrelic_transaction_tracer_config_t.
 */
typedef uint64_t newrelic_time_us_t;

/**
 * @brief Log levels.
 *
 * An enumeration of the possible log levels for an SDK configuration, or
 * newrelic_app_config_t. The highest priority loglevel is NEWRELIC_LOG_ERROR.
 * The level NEWRELIC_LOG_DEBUG offers the greatest verbosity.
 *
 * @see newrelic_app_config_t
 */
typedef enum _newrelic_loglevel_t {

  /** The highest-priority loglevel; only errors are logged. */
  NEWRELIC_LOG_ERROR,

  /** The loglevel for warnings and errors. */
  NEWRELIC_LOG_WARNING,

  /** The loglevel for informational logs, warnings, and errors. */
  NEWRELIC_LOG_INFO,

  /** The highest-verbosity loglevel. */
  NEWRELIC_LOG_DEBUG,
} newrelic_loglevel_t;

/**
 * @brief Configuration values used to configure how SQL queries
 * are recorded and reported to New Relic.
 *
 * @see newrelic_transaction_tracer_config_t
 */
typedef enum _newrelic_tt_recordsql_t {
  /**
   * When the record_sql field of the newrelic_transaction_tracer_config_t
   * is set to NEWRELIC_SQL_OFF, no queries are reported to New Relic.
   */
  NEWRELIC_SQL_OFF,

  /**
   * For the SQL-like datastores which are supported by the C SDK, when the
   * record_sql field of the newrelic_transaction_tracer_config_t is set
   * to NEWRELIC_SQL_RAW the query param of the
   * newrelic_datastore_segment_config_t is reported as-is to New Relic.
   *
   * @warning This setting is not recommended.
   */
  NEWRELIC_SQL_RAW,

  /**
   * For the SQL-like datastores which are supported by the C SDK, when the
   * record_sql field of the newrelic_transaction_tracer_config_t is set
   * to NEWRELIC_SQL_RAW the query param of the
   * newrelic_datastore_segment_config_t is reported to New Relic with
   * alphanumeric characters set to '?'.
   */
  NEWRELIC_SQL_OBFUSCATED
} newrelic_tt_recordsql_t;

/**
 * @brief Configuration values used to configure the behaviour of the
 * transaction tracer.
 *
 * @see newrelic_transaction_tracer_config_t
 */
typedef enum _newrelic_transaction_tracer_threshold_t {
  /**
   * Use 4*apdex(T) as the minimum time a transaction must take before it is
   * eligible for a transaction trace.
   */
  NEWRELIC_THRESHOLD_IS_APDEX_FAILING,

  /**
   * Use the value given in the duration_us field as the minimum time a
   * transaction must take before it is eligible for a transaction trace.
   */
  NEWRELIC_THRESHOLD_IS_OVER_DURATION,
} newrelic_transaction_tracer_threshold_t;

/** @brief Configuration used to configure transaction tracing. */
typedef struct _newrelic_transaction_tracer_config_t {
  /**
   * @brief Whether to enable transaction traces.
   *
   * Default: true.
   */
  bool enabled;

  /**
   * @brief Whether to consider transactions for trace generation based on the
   * apdex configuration or a specific duration.
   *
   * Default: NEWRELIC_THRESHOLD_IS_APDEX_FAILING.
   */
  newrelic_transaction_tracer_threshold_t threshold;

  /**
   * @brief If the SDK configuration threshold is set to
   * NEWRELIC_THRESHOLD_IS_OVER_DURATION, this field specifies the minimum
   * transaction time before a trace may be generated, in microseconds.
   *
   * Default: 0.
   */
  newrelic_time_us_t duration_us;

  /**
   * @brief Sets the threshold above which the New Relic SDK will record a
   * stack trace for a transaction trace, in microseconds.
   *
   * Default: 500000, or 0.5 seconds.
   */
  newrelic_time_us_t stack_trace_threshold_us;

  /**
   * @brief The datastore_reporting field of
   * newrelic_transaction_tracer_config_t is a collection of configuration
   * values that control how certain characteristics of datastore queries are
   * recorded.
   */
  struct {
    /**
     * @brief Controls whether slow datastore queries are recorded.
     *
     * If set to true for a transaction, the transaction tracer records
     * the top-10 slowest queries along with a stack trace of where the
     * call occurred.
     *
     * Default: true.
     */
    bool enabled;

    /**
     * @brief Controls the format of the sql put into transaction traces for
     * supported sql-like products.
     *
     * Only relevant if the datastore_reporting.enabled field is set to true.
     *
     * - If set to NEWRELIC_SQL_OFF, transaction traces have no sql in them.
     * - If set to NEWRELIC_SQL_RAW, the sql is added to the transaction
     *   trace as-is.
     * - If set to NEWRELIC_SQL_OBFUSCATED, alphanumeric characters are set
     *   to '?'. For example 'SELECT * FROM table WHERE foo = 42' is reported
     *   as 'SELECT * FROM table WHERE foo = ?'. These obfuscated queries are
     *   added to the transaction trace for supported datastore products.
     *
     * @warning New Relic highly discourages the use of the NEWRELIC_SQL_RAW
     * setting in production environments.
     *
     * Default: NEWRELIC_SQL_OBFUSCATED.
     */
    newrelic_tt_recordsql_t record_sql;

    /**
     * @brief Specify the threshold above which a datastore query is considered
     * "slow", in microseconds.
     *
     * Only relevant if the datastore_reporting.enabled field is set to true.
     *
     * Default: 500000, or 0.5 seconds.
     */
    newrelic_time_us_t threshold_us;
  } datastore_reporting;

} newrelic_transaction_tracer_config_t;

/**
 * @brief Configuration used to configure how datastore segments
 * are recorded in a transaction.
 */
typedef struct _newrelic_datastore_segment_config_t {
  /**
   * @brief Configuration which controls whether datastore instance
   * names are reported to New Relic.
   *
   * If set to true for a transaction, instance names are reported to New
   * Relic. More specifically, the host and port_path_or_id fields in a
   * newrelic_datastore_segment_params_t passed to
   * newrelic_datastore_start_segment() is reported when the
   * corresponding transaction is reported.
   */
  bool instance_reporting;

  /**
   * @brief Configuration which controls whether datastore database
   * names are reported to New Relic.
   *
   * If set to true for a transaction, database names are reported to New
   * Relic. More specifically, the database_name field in a
   * newrelic_datastore_segment_params_t passed to
   * newrelic_datastore_start_segment() is reported when the
   * corresponding transaction is reported.
   */
  bool database_name_reporting;

} newrelic_datastore_segment_config_t;

/**
 * @brief Configuration used for distributed tracing.
 *
 */
typedef struct _newrelic_distributed_tracing_config_t {
  /**
   * @brief Specifies whether or not distributed tracing is enabled.
   *
   * When set to true, distributed tracing is enabled for the C SDK. The
   * default configuration returned by newrelic_create_app_config() sets
   * this value to false.
   */
  bool enabled;
} newrelic_distributed_tracing_config_t;

/**
 * @brief Configuration used for span events.
 *
 */
typedef struct _newrelic_span_event_config_t {
  /**
   * @brief Specifies whether or not span events are generated.
   *
   * When set to true, span events are generated by the C SDK. The default
   * configuration returned by newrelic_create_app_config() sets this value
   * to true.
   */
  bool enabled;
} newrelic_span_event_config_t;

/**
 * @brief Configuration used to describe application name, license key, as
 * well as optional transaction tracer and datastore configuration.
 *
 * @see newrelic_create_app_config().
 */
typedef struct _newrelic_app_config_t {
  /**
   * @brief Specifies the name of the application to which data shall be
   * reported.
   */
  char app_name[255];

  /**
   * @brief Specifies the New Relic license key to use.
   */
  char license_key[255];

  /**
   * @brief Optional. Specifies the New Relic provided host. There is little
   * reason to ever change this from the default.
   */
  char redirect_collector[100];

  /**
   * @brief Optional. Specifies the file to be used for C SDK logs.
   *
   * If no filename is provided, no logging shall occur.
   */
  char log_filename[512];

  /**
   * @brief Optional. Specifies the logfile's level of detail.
   *
   * There is little reason to change this from the default value except
   * in troubleshooting scenarios.
   *
   * Must be one of the following values: NEWRELIC_LOG_ERROR,
   * NEWRELIC_LOG_WARNING NEWRELIC_LOG_INFO (default), NEWRELIC_LOG_DEBUG.
   */
  newrelic_loglevel_t log_level;

  /**
   * @brief Optional. The transaction tracer configuration.
   *
   * By default, the configuration returned by newrelic_create_app_config()
   * enables transaction traces, with the threshold set to
   * NEWRELIC_THRESHOLD_IS_APDEX_FAILING.
   */
  newrelic_transaction_tracer_config_t transaction_tracer;

  /**
   * @brief Optional. The datastore tracer configuration.
   *
   * By default, the configuration returned by newrelic_create_app_config()
   * enables datastore segments with instance_reporting and
   * database_name_reporting set to true.
   */
  newrelic_datastore_segment_config_t datastore_tracer;

  /**
   * @brief Optional. Distributed tracing configuration.
   *
   * By default, the configuration returned by newrelic_create_app_config()
   * disables the distributed tracing feature by setting
   * distributed_tracing.enabled to true.
   */
  newrelic_distributed_tracing_config_t distributed_tracing;

  /**
   * @brief Optional. Span event configuration.
   *
   * By default, the configuration returned by newrelic_create_app_config()
   * enables span event creation for the C SDK by setting span_events.enabled
   * to true.
   */
  newrelic_span_event_config_t span_events;

} newrelic_app_config_t;

/**
 * @brief Segment configuration used to instrument calls to databases and object
 * stores.
 */
typedef struct _newrelic_datastore_segment_params_t {
  /**
   * @brief Specifies the datastore type, e.g., "MySQL", to indicate that the
   * segment represents a query against a MySQL database.
   *
   * New Relic recommends using the predefined NEWRELIC_DATASTORE_FIREBIRD
   * through NEWRELIC_DATASTORE_SYBASE constants for this field. If this field
   * points to a string that is not one of NEWRELIC_DATASTORE_FIREBIRD through
   * NEWRELIC_DATASTORE_SYBASE, the resulting datastore segment shall be
   * instrumented as an unsupported datastore.
   *
   * For SQL-like datastores supported by the C SDK, when the
   * record_sql field of the newrelic_transaction_tracer_config_t is set
   * to NEWRELIC_SQL_RAW or NEWRELIC_SQL_OBFUSCATED, the query param of the
   * newrelic_datastore_segment_config_t is reported to New Relic.
   *
   * This field is required to be a non-empty, null-terminated string that does
   * not include any slash characters. Empty strings are replaced with the
   * string NEWRELIC_DATASTORE_OTHER.
   */
  char* product;

  /**
   * @brief Optional. Specifies the table or collection being used or queried
   * against.
   *
   * If provided, this field is required to be a null-terminated string that
   * does not include any slash characters. It is also valid to use the default
   * NULL value, in which case the default string of "other" will be attached
   * to the datastore segment.
   */
  char* collection;

  /**
   * @brief Optional. Specifies the operation being performed: for example,
   * "select" for an SQL SELECT query, or "set" for a Memcached set operation.
   * While operations may be specified with any case, New Relic suggests
   * using lowercase.
   *
   * If provided, this field is required to be a null-terminated string that
   * does not include any slash characters. It is also valid to use the default
   * NULL value, in which case the default string of "other" will be attached
   * to the datastore segment.
   */
  char* operation;

  /**
   * @brief Optional. Specifies the datahost host name.
   *
   * If provided, this field is required to be a null-terminated string that
   * does not include any slash characters. It is also valid to use the default
   * NULL value, in which case the default string of "other" will be attached
   * to the datastore segment.
   */
  char* host;

  /**
   * @brief Optional. Specifies the port or socket used to connect to the
   * datastore.
   *
   * If provided, this field is required to be a null-terminated string.
   */
  char* port_path_or_id;

  /**
   * @brief Optional. Specifies the database name or number in use.
   *
   * If provided, this field is required to be a null-terminated string.
   */
  char* database_name;

  /**
   * @brief Optional. Specifies the database query that was sent to the server.
   *
   * For security reasons, this value is only used if you set product to
   * a supported sql-like datastore, NEWRELIC_DATASTORE_FIREBIRD,
   * NEWRELIC_DATASTORE_INFORMIX, NEWRELIC_DATASTORE_MSSQL, etc. This
   * allows the SDK to correctly obfuscate the query. When the product
   * is set otherwise, no query information is reported to New Relic.
   *
   * If provided, this field is required to be a null-terminated string.
   */
  char* query;
} newrelic_datastore_segment_params_t;

/** @brief Segment configuration used to instrument external calls. */
typedef struct _newrelic_external_segment_params_t {
  /**
   * @brief The URI that was loaded; it cannot be NULL.
   *
   * This field is required to be a null-terminated
   * string containing a valid URI, and cannot be NULL.
   */
  char* uri;

  /**
   * @brief The procedure used to load the external resource.
   *
   * In HTTP contexts, this will usually be the request method (eg `GET`,
   * `POST`, et al). For non-HTTP requests, or protocols that encode more
   * specific semantics on top of HTTP like SOAP, you may wish to use a
   * different value that more precisely encodes how the resource was
   * requested.
   *
   * If provided, this field is required to be a null-terminated string that
   * does not include any slash characters. It is also valid to provide NULL,
   * in which case no procedure will be attached to the external segment.
   */
  char* procedure;

  /**
   * @brief The library used to load the external resource.
   *
   * If provided, this field is required to be a null-terminated string that
   * does not include any slash characters. It is also valid to provide NULL,
   * in which case no library will be attached to the external segment.
   */
  char* library;
} newrelic_external_segment_params_t;

/**
 * @brief Configure the C SDK's logging system.
 *
 * If the logging system was previously initialized (either by a prior call to
 * newrelic_configure_log() or implicitly by a call to newrelic_init() or
 * newrelic_create_app()), then invoking this function will close the previous
 * log file.
 *
 * @param [in] filename   The path to the file to write logs to. If this is the
 *                        literal string "stdout" or "stderr", then logs will be
 *                        written to standard output or standard error,
 *                        respectively.
 * @param [in] level      The lowest level of log message that will be output.
 *
 * @return true on success; false otherwise.
 */
bool newrelic_configure_log(const char* filename, newrelic_loglevel_t level);

/**
 * @brief Initialise the C SDK with non-default settings.
 *
 * Generally, this function only needs to be called explicitly if the daemon
 * socket location needs to be customised. By default, "/tmp/.newrelic.sock" is
 * used, which matches the default socket location used by newrelic-daemon if
 * one isn't given.
 *
 * The daemon socket location can be specified in four different ways:
 *
 * - To use a specified file as a UNIX domain socket (UDS), provide an absolute
 *    path name as a string.
 * - To use a standard TCP port, specify a number in the range 1 to 65534.
 * - To use an abstract socket, prefix the socket name with '@'.
 * - To connect to a daemon that is running on a different host, set this value
 *   to '<host>:<port>', where '<host>' denotes either a host name or an IP
 *   address, and '<port>' denotes a valid port number. Both IPv4 and IPv6 are
 *   supported.
 *
 * If an explicit call to this function is required, it must occur before the
 * first call to newrelic_create_app().
 *
 * Subsequent calls to this function after a successful call to newrelic_init()
 * or newrelic_create_app() will fail.
 *
 * @param [in] daemon_socket  The path to the daemon socket. If this is NULL,
 *                            then the default will be used, which is to look
 *                            for a UNIX domain socket at /tmp/.newrelic.sock.
 * @param [in] time_limit_ms  The amount of time, in milliseconds, that the C
 *                            SDK will wait for a response from the daemon
 *                            before considering initialization to have failed.
 *                            If this is 0, then a default value will be used.
 *
 * @return true on success; false otherwise.
 */
bool newrelic_init(const char* daemon_socket, int time_limit_ms);

/**
 * @brief Create a populated application configuration.
 *
 * Given an application name and license key, this method returns an SDK
 * configuration. Specifically, it returns a pointer to a newrelic_app_config_t
 * with initialized app_name and license_key fields along with default values
 * for the remaining fields. After the application has been created with
 * newrelic_create_app(), the caller should free the configuration using
 * newrelic_destroy_app_config().
 *
 * @param [in] app_name The name of the application.
 * @param [in] license_key A valid license key supplied by New Relic.
 *
 * @return An application configuration populated with app_name and
 * license_key; all other fields are initialized to their defaults.
 */
newrelic_app_config_t* newrelic_create_app_config(const char* app_name,
                                                  const char* license_key);

/**
 * @brief Destroy the application configuration.
 *
 * Given an allocated application configuration, newrelic_destroy_app_config()
 * frees the configuration.
 *
 * @param [in] config The address of the pointer to the allocated application
 *             configuration.
 *
 * @return false if config is NULL or points to NULL; true otherwise.
 *
 * @warning This function must only be called once for a given application
 * configuration.
 */
bool newrelic_destroy_app_config(newrelic_app_config_t** config);

/**
 * @brief Create an application.
 *
 * Given a configuration, newrelic_create_app() returns a pointer to the
 * newly allocated application, or NULL if there was an error. If successful,
 * the caller should destroy the application with the supplied
 * newrelic_destroy_app() when finished.
 *
 * @param [in] config     An application configuration created by
 *                        newrelic_create_app_config().
 * @param [in] timeout_ms Specifies the maximum time to wait for a connection to
 *                        be established; a value of 0 causes the method to make
 *                        only one attempt at connecting to the daemon.
 *
 * @return A pointer to an allocated application, or NULL on error; any errors
 * resulting from a badly-formed configuration are logged.
 */
newrelic_app_t* newrelic_create_app(const newrelic_app_config_t* config,
                                    unsigned short timeout_ms);

/**
 * @brief Destroy the application.
 *
 * Given an allocated application, newrelic_destroy_app() closes the logfile
 * handle and frees any memory used by app to describe the application.
 *
 * @param [in] app The address of the pointer to the allocated application.
 *
 * @return false if app is NULL or points to NULL; true otherwise.
 *
 * @warning This function must only be called once for a given application.
 */
bool newrelic_destroy_app(newrelic_app_t** app);

/**
 * @brief Start a web based transaction.
 *
 * Given an application pointer and transaction name, this function begins
 * timing a new transaction. It returns a valid pointer to an active New Relic
 * transaction, newrelic_txn_t. The return value of this function may be
 * used as an input parameter to functions that modify an active transaction.
 *
 * @param [in] app A pointer to an allocation application.
 * @param [in] name The name for the transaction.
 *
 * @return A pointer to the transaction.
 */
newrelic_txn_t* newrelic_start_web_transaction(newrelic_app_t* app,
                                               const char* name);

/**
 * @brief Start a non-web based transaction.
 *
 * Given a valid application and transaction name, this function begins timing
 * a new transaction and returns a valid pointer to a New Relic transaction,
 * newrelic_txn_t. The return value of this function may be used as an input
 * parameter to functions that modify an active transaction.
 *
 * @param [in] app A pointer to an allocation application.
 * @param [in] name The name for the transaction.
 *
 * @return A pointer to the transaction.
 */
newrelic_txn_t* newrelic_start_non_web_transaction(newrelic_app_t* app,
                                                   const char* name);

/**
 * @brief Override the timing for the given transaction.
 *
 * Transactions are normally timed automatically based on when they were started
 * and ended. Calling this function disables the automatic timing, and uses the
 * times given instead.
 *
 * Note that this may cause unusual looking transaction traces. This function
 * manually alters a transaction's start time and duration, but it does not
 * alter any timing for the segments belonging to the transaction. As a result,
 * the sum of all segment durations may be substantively greater or less than
 * the total duration of the transaction.
 *
 * It is likely that users of this function will also want to use
 * newrelic_set_segment_timing() to manually time their segments.
 *
 * @param [in] transaction    The transaction to manually time.
 * @param [in] start_time     The start time for the segment, in microseconds
 *                            since the UNIX Epoch.
 * @param [in] duration       The duration of the transaction in microseconds.
 *
 * @return true if the segment timing was changed; false otherwise.
 */
bool newrelic_set_transaction_timing(newrelic_txn_t* transaction,
                                     newrelic_time_us_t start_time,
                                     newrelic_time_us_t duration);

/**
 * @brief End a transaction.
 *
 * Given an active transaction, this function stops the transaction's
 * timing, sends any data to the New Relic daemon, and destroys the transaction.
 *
 * @param [in] transaction_ptr  The address of a pointer to an active
 *                              transaction.
 *
 * @return false if transaction is NULL or points to NULL; false if data cannot
 * be sent to newrelic; true otherwise.
 *
 * @warning This function must only be called once for a given transaction.
 */
bool newrelic_end_transaction(newrelic_txn_t** transaction_ptr);

/**
 * @brief Add a custom integer attribute to a transaction.
 *
 * Given an active transaction, this function appends an
 * integer attribute to the transaction.
 *
 * @param [in] transaction An active transaction.
 * @param [in] key The name of the attribute.
 * @param [in] value The integer value of the attribute.
 *
 * @return true if successful; false otherwise.
 */
bool newrelic_add_attribute_int(newrelic_txn_t* transaction,
                                const char* key,
                                const int value);

/**
 * @brief Add a custom long attribute to a transaction.
 *
 * Given an active transaction, this function appends a
 * long attribute to the transaction.
 *
 * @param [in] transaction An active transaction.
 * @param [in] key The name of the attribute.
 * @param [in] value The long value of the attribute.
 *
 * @return true if successful; false otherwise.
 */
bool newrelic_add_attribute_long(newrelic_txn_t* transaction,
                                 const char* key,
                                 const long value);

/**
 * @brief Add a custom double attribute to a transaction.
 *
 * Given an active transaction, this function appends a
 * double attribute to the transaction.
 *
 * @param [in] transaction An active transaction.
 * @param [in] key The name of the attribute.
 * @param [in] value The double value of the attribute.
 *
 * @return true if successful; false otherwise.
 */
bool newrelic_add_attribute_double(newrelic_txn_t* transaction,
                                   const char* key,
                                   const double value);

/**
 * @brief Add a custom string attribute to a transaction.
 *
 * Given an active transaction, this function appends a
 * string attribute to the transaction.
 *
 * @param [in] transaction An active transaction.
 * @param [in] key The name of the attribute.
 * @param [in] value The string value of the attribute.
 *
 * @return true if successful; false otherwise.
 */
bool newrelic_add_attribute_string(newrelic_txn_t* transaction,
                                   const char* key,
                                   const char* value);

/**
 * @brief Record an error in a transaction.
 *
 * Given an active transaction, this function records an error
 * inside of the transaction.
 *
 * @param [in]  transaction An active transaction.
 * @param [in]  priority The error's priority. The C SDK sends up one error per
 *              transaction.  If multiple calls to this function are made during
 *              a single transaction, the error with the highest priority is
 *              reported to New Relic.
 * @param [in]  errmsg A string comprising the error message.
 * @param [in]  errclass A string comprising the error class.
 */
void newrelic_notice_error(newrelic_txn_t* transaction,
                           int priority,
                           const char* errmsg,
                           const char* errclass);

/**
 * @brief Record an error in a transaction, passing a custom stacktrace
 *
 * Given an active transaction, this function records an error
 * inside of the transaction. A custom stacktrace must be passed as a string
 * in JSON format like: "[\"first line\",\"second line\",\"third line\"]"
 *
 * @param [in]  transaction An active transaction.
 * @param [in]  priority The error's priority. The C SDK sends up one error per
 *              transaction.  If multiple calls to this function are made during
 *              a single transaction, the error with the highest priority is
 *              reported to New Relic.
 * @param [in]  errmsg A string comprising the error message.
 * @param [in]  errclass A string comprising the error class.
 * @param [in]  errstacktrace A string comprising the error stacktrace, in NewRelic's JSON format.
 */
void newrelic_notice_error_with_stacktrace(newrelic_txn_t* transaction,
                           int priority,
                           const char* errmsg,
                           const char* errclass,
                           const char* errstacktrace);

/**
 * @brief A segment within a transaction.
 *
 * Within an active transaction, instrument additional segments for greater
 * granularity.
 *
 * -For external calls:   newrelic_start_external_segment().
 * -For datastore calls:  newrelic_start_datastore_segment().
 * -For arbitrary code:   newrelic_start_segment().
 *
 * All segments must be ended with newrelic_end_segment(). Examples of
 * instrumenting segments are available in the examples directory.
 */
typedef struct _newrelic_segment_t newrelic_segment_t;

/**
 * @brief A Custom Event.
 *
 * The C SDK provides a Custom Events API that allows one to send custom events
 * to New Relic Insights. To send an event, start a transaction and use the
 * newrelic_create_custom_event() and newrelic_record_custom_event()
 * functions. Examples of sending custom events are available in the
 * examples directory.
 */
typedef struct _newrelic_custom_event_t newrelic_custom_event_t;

/**
 * @brief Record the start of a custom segment in a transaction.
 *
 * Given an active transaction this function creates a custom segment to be
 * recorded as part of the transaction. A subsequent call to
 * newrelic_end_segment() records the end of the segment.
 *
 * @param [in] transaction  An active transaction.
 * @param [in] name         The segment name. If NULL or an invalid name is
 *                          passed, this defaults to "Unnamed segment".
 * @param [in] category     The segment category. If NULL or an invalid
 *                          category is passed, this defaults to "Custom".
 *
 * @return A pointer to a valid custom segment; NULL otherwise.
 *
 */
newrelic_segment_t* newrelic_start_segment(newrelic_txn_t* transaction,
                                           const char* name,
                                           const char* category);

/**
 * @brief Record the start of a datastore segment in a transaction.
 *
 * Given an active transaction and valid parameters, this function creates a
 * datastore segment to be recorded as part of the transaction. A subsequent
 * call to newrelic_end_segment() records the end of the segment.
 *
 * @param [in] transaction An active transaction.
 * @param [in] params Valid parameters describing a datastore segment.
 *
 * @return A pointer to a valid datastore segment; NULL otherwise.
 *
 */
newrelic_segment_t* newrelic_start_datastore_segment(
    newrelic_txn_t* transaction,
    const newrelic_datastore_segment_params_t* params);

/**
 * @brief Start recording an external segment within a transaction.
 *
 * Given an active transaction, this function creates an external segment
 * inside of the transaction and marks it as having been started. An external
 * segment is generally used to represent a HTTP or RPC request.
 *
 * @param [in] transaction An active transaction.
 * @param [in] params      The parameters describing the external request. All
 *                         parameters are copied, and no references to the
 *                         pointers provided are kept after this function
 *                         returns.
 *
 * @return A pointer to an external segment, which may then be provided to
 *         newrelic_end_segment() when the external request is complete. If an
 *         error occurs when creating the external segment, NULL is returned,
 *         and a log message will be written to the SDK log at LOG_ERROR
 *         level.
 */
newrelic_segment_t* newrelic_start_external_segment(
    newrelic_txn_t* transaction,
    const newrelic_external_segment_params_t* params);

/**
 * @brief Set the parent for the given segment.
 *
 * This function changes the parent for the given segment to another segment.
 * Both segments must exist on the same transaction, and must not have ended.
 *
 * @param [in] segment The segment to reparent.
 * @param [in] parent  The new parent segment.
 *
 * @return true if the segment was successfully reparented; false otherwise.
 *
 * @warning Do not attempt to use a segment that has had newrelic_end_segment()
 * called on it as a segment or parent: this will result in a use-after-free
 * scenario, and likely a crash.
 */
bool newrelic_set_segment_parent(newrelic_segment_t* segment,
                                 newrelic_segment_t* parent);

/**
 * @brief Set the transaction's root as the parent for the given segment.
 *
 * Transactions are represented by a collection of segments. Segments are
 * created by calls to newrelic_start_segment(),
 * newrelic_start_datastore_segment() and newrelic_start_external_segment().
 * In addition, a transaction has an automatically-created root segment that
 * represents the entrypoint of the transaction. In some cases, users may want
 * to manually parent their segments with the transaction's root segment.
 *
 * @param [in] segment  The segment to be parented.
 *
 * @return true if the segment was successfully reparented; false otherwise.
 *
 * @warning Do not attempt to use a segment that has had newrelic_end_segment()
 * called on it as a segment or parent: this will result in a use-after-free
 * scenario, and likely a crash.
 */
bool newrelic_set_segment_parent_root(newrelic_segment_t* segment);

/**
 * @brief Override the timing for the given segment.
 *
 * Segments are normally timed automatically based on when they were started
 * and ended. Calling this function disables the automatic timing, and uses the
 * times given instead.
 *
 * Note that this may cause unusual looking transaction traces, as this
 * function does not change the parent segment. It is likely that users of this
 * function will also want to use newrelic_set_segment_parent() to manually
 * parent their segments.
 *
 * @param [in] segment    The segment to manually time.
 * @param [in] start_time The start time for the segment, in microseconds since
 *                        the start of the transaction.
 * @param [in] duration   The duration of the segment in microseconds.
 *
 * @return true if the segment timing was changed; false otherwise.
 */
bool newrelic_set_segment_timing(newrelic_segment_t* segment,
                                 newrelic_time_us_t start_time,
                                 newrelic_time_us_t duration);

/**
 * @brief Record the completion of a segment in a transaction.
 *
 * Given an active transaction, this function records the segment's metrics
 * on the transaction.
 *
 * @param [in] transaction An active transaction.
 * @param [in,out] segment_ptr The address of a valid segment.
 * Before the function returns, any segment_ptr memory is freed;
 * segment_ptr is set to NULL to avoid any potential double free errors.
 *
 * @return true if the parameters represented an active transaction
 * and custom segment to record as complete; false otherwise.
 * If an error occurred, a log message will be written to the
 * SDK log at LOG_ERROR level.
 */
bool newrelic_end_segment(newrelic_txn_t* transaction,
                          newrelic_segment_t** segment_ptr);

/**
 * @brief Creates a custom event
 *
 * Attributes can be added to the custom event using the
 * newrelic_custom_event_add_* family of functions. When the required attributes
 * have been added, the custom event can be recorded using
 * newrelic_record_custom_event().
 *
 * When passed to newrelic_record_custom_event, the custom event will be freed.
 * If you can't pass an allocated event to newrelic_record_custom_event, use the
 * newrelic_discard_custom_event function to free the event.
 *
 * @param [in] event_type The type/name of the event
 *
 * @return A pointer to a custom event; NULL otherwise.
 */
newrelic_custom_event_t* newrelic_create_custom_event(const char* event_type);

/**
 * @brief Frees the memory for custom events created via the
 * newrelic_create_custom_event function
 *
 * This function is here in case there's an allocated newrelic_custom_event_t
 * that ends up not being recorded as a custom event, but still needs to be
 * freed
 *
 * @param [in] event The address of a valid custom event created by
 * newrelic_create_custom_event().
 */
void newrelic_discard_custom_event(newrelic_custom_event_t** event);

/**
 * @brief Records the custom event.
 *
 * Given an active transaction, this function adds the custom event to the
 * transaction and timestamps it, ensuring the event will be sent to New Relic.
 *
 * @param [in] transaction An active transaction
 * @param [in] event The address of a valid custom event created by
 *             newrelic_create_custom_event()
 *
 * newrelic_create_custom_event
 */
void newrelic_record_custom_event(newrelic_txn_t* transaction,
                                  newrelic_custom_event_t** event);

/**
 * @brief Adds an int key/value pair to the custom event's attributes
 *
 * Given a custom event, this function adds an integer attributes to the event.
 *
 * @param [in] event A valid custom event, @see newrelic_create_custom_event()
 * @param [in] key the string key for the key/value pair
 * @param [in] value the integer value of the key/value pair
 *
 * @return false indicates the attribute could not be added
 */
bool newrelic_custom_event_add_attribute_int(newrelic_custom_event_t* event,
                                             const char* key,
                                             int value);

/**
 * @brief Adds a long key/value pair to the custom event's attributes
 *
 * Given a custom event, this function adds a long attribute to the event.
 *
 * @param [in] event A valid custom event created by
 * newrelic_create_custom_event()
 * @param [in] key the string key for the key/value pair
 * @param [in] value the long value of the key/value pair
 *
 * @return false indicates the attribute could not be added
 */
bool newrelic_custom_event_add_attribute_long(newrelic_custom_event_t* event,
                                              const char* key,
                                              long value);

/**
 * @brief Adds a double key/value pair to the custom event's attributes
 *
 * Given a custom event, this function adds a double attribute to the event.
 *
 * @param [in] event A valid custom event created by
 * newrelic_create_custom_event()
 * @param [in] key the string key for the key/value pair
 * @param [in] value the double value of the key/value pair
 *
 * @return false indicates the attribute could not be added
 */
bool newrelic_custom_event_add_attribute_double(newrelic_custom_event_t* event,
                                                const char* key,
                                                double value);

/**
 * @brief Adds a string key/value pair to the custom event's attributes
 *
 * Given a custom event, this function adds a char* (string) attribute to the
 * event.
 *
 * @param [in] event A valid custom event created by
 * newrelic_create_custom_event().
 * @param [in] key the string key for the key/value pair
 * @param [in] value the string value of the key/value pair
 *
 * @return false indicates the attribute could not be added
 */
bool newrelic_custom_event_add_attribute_string(newrelic_custom_event_t* event,
                                                const char* key,
                                                const char* value);

/**
 * @brief Get the SDK version.
 *
 * @return A NULL-terminated string containing the C SDK version number. If
 * the version number is unavailable, the string "NEWRELIC_VERSION" will be
 * returned.
 *
 * @warning This string is owned by the SDK, and must not be freed or modified.
 */
const char* newrelic_version(void);

/**
 * @brief Generate a custom metric.
 *
 * Given an active transaction and valid parameters, this function creates a
 * custom metric to be recorded as part of the transaction.
 *
 * @param [in] transaction An active transaction.
 * @param [in] metric_name The name/identifier for the metric.
 * @param [in] milliseconds The amount of time the metric will
 *             record, in milliseconds.
 *
 * @return true on success.
 */
bool newrelic_record_custom_metric(newrelic_txn_t* transaction,
                                   const char* metric_name,
                                   double milliseconds);

/**
 * @brief Ignore the current transaction
 *
 * Given a transaction, this function instructs the C SDK to not send data to
 * New Relic for that transaction.
 *
 * @warning Even when newrelic_ignore_transaction() is called, one must still
 * call newrelic_end_transaction() to free the memory used by the transaction
 * and avoid a memory leak.
 *
 * @param [in] transaction An active transaction.
 *
 * @return true on success.
 */
bool newrelic_ignore_transaction(newrelic_txn_t* transaction);

/**
 * @brief Create a distributed trace payload.
 *
 * Create a newrelic header, or a payload, to add to a service's outbound
 * requests. This header contains the metadata necessary to link spans together
 * for a complete distributed trace. The metadata includes: the trace ID number,
 * the span ID number, New Relic account ID number, and sampling information.
 * Note that a payload must be created within an active transaction.
 *
 * @param [in] transaction    An active transaction. This value cannot be NULL.
 * @param [in] segment        An active segment in which the distributed trace
 *                            payload is being created, or NULL to indicate
 *                            that the payload is created for the root
 *                            segment.
 *
 * @return If successful, a string to manually add to a service's outbound
 * requests. If the instrumented application has not established a connection
 * to the daemon or if distributed tracing is not enabled in the
 * newrelic_app_config_t, this function returns NULL. The caller is responsible
 * for invoking free() on the returned string.
 */
char* newrelic_create_distributed_trace_payload(newrelic_txn_t* transaction,
                                                newrelic_segment_t* segment);

/**
 * @brief Accept a distributed trace payload.
 *
 * Accept newrelic headers, or a payload, created with
 * newrelic_create_distributed_trace_payload(). Such headers are manually added
 * to a service's outbound request. The receiving service gets the newrelic
 * header from the incoming request and uses this function to accept the payload
 * and link corresponding spans together for a complete distributed trace. Note
 * that a payload must be accepted within an active transaction.
 *
 * @param [in] transaction    An active transaction. This value cannot be NULL.
 * @param [in] payload        A string created by
 *                            newrelic_create_distributed_trace_payload().
 *                            This value cannot be NULL.
 * @param [in] transport_type Transport type used for communicating the external
 *                            call. It is strongly recommended that one of
 *                            NEWRELIC_TRANSPORT_TYPE_UNKNOWN
 *                            through NEWRELIC_TRANSPORT_TYPE_OTHER are used
 *                            for this value.  If NULL is supplied for this
 *                            parameter, an info-level message is logged and
 *                            the default value of
 *                            NEWRELIC_TRANSPORT_TYPE_UNKNOWN is used.
 *
 * @return true on success.
 */
bool newrelic_accept_distributed_trace_payload(newrelic_txn_t* transaction,
                                               const char* payload,
                                               const char* transport_type);

/**
 * @brief Create a distributed trace payload, an http-safe, base64-encoded
 * string.
 *
 * This function offers the same behaviour as
 * newrelic_create_distributed_trace_payload() but creates a base64-encoded
 * string for the payload. The caller is responsible for invoking free()
 * on the returned string.
 *
 * @see newrelic_create_distributed_trace_payload()
 *
 */
char* newrelic_create_distributed_trace_payload_httpsafe(
    newrelic_txn_t* transaction,
    newrelic_segment_t* segment);

/**
 * @brief Accept a distributed trace payload, an http-safe, base64-encoded
 * string.
 *
 * This function offers the same behaviour as
 * newrelic_accept_distributed_trace_payload() but accepts a base64-encoded
 * string for the payload.
 *
 * @see newrelic_accept_distributed_trace_payload()
 *
 */
bool newrelic_accept_distributed_trace_payload_httpsafe(
    newrelic_txn_t* transaction,
    const char* payload,
    const char* transport_type);

/**
 * @brief Set a transaction name
 *
 * Given an active transaction and a name, this function sets the transaction
 * name to the given name.
 *
 * @param [in] transaction  An active transaction.
 * @param [in] name         Name for the transaction.
 *
 * @return true on success.
 *
 * @warning Do not use brackets [] at the end of your transaction name.
 * New Relic automatically strips brackets from the name. Instead, use
 * parentheses () or other symbols if needed.
 *
 */
bool newrelic_set_transaction_name(newrelic_txn_t* transaction,
                                   const char* transaction_name);

/**
 * A list of examples for Doxygen to cross-reference. If a function in
 * libnewrelic.h appears in one of these examples, the example source
 * file appears under the "Examples" header.
 *
 * \example ex_custom.c
 * \example ex_datastore.c
 * \example ex_distributed_tracing_client.c
 * \example ex_distributed_tracing_server.c
 * \example ex_external.c
 * \example ex_notice_error.c
 * \example ex_segment.c
 * \example ex_simple.c
 * \example ex_timing.c
 *
 */

#ifdef __cplusplus
}
#endif

#endif /* LIBNEWRELIC_H */
