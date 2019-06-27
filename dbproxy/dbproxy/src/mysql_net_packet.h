#ifndef _MYSQL_NET_PACKET_H_
#define _MYSQL_NET_PACKET_H_

#include "include.h"
#include "standard_serialize.h"
#include "net_pack.h"
#include "mysql_function.h"

#define MYSQL_PACKET_HEADER_LEN 4
#define MAX_PACKET_LENGTH (1024*8-1)
#define MYSQL_ERRMSG_SIZE 512
//#define MAX_PACKET_LENGTH (256L*256L*256L-1)
#define SERVER_VERSION_LENGTH 60
#define PROTOCOL_VERSION 10
#define AUTH_PLUGIN_DATA_PART_1_LENGTH 8

#define CLIENT_LONG_PASSWORD    1   /* new more secure passwords */
#define CLIENT_FOUND_ROWS   2   /* Found instead of affected rows */
#define CLIENT_LONG_FLAG    4   /* Get all column flags */
#define CLIENT_CONNECT_WITH_DB  8   /* One can specify db on connect */
#define CLIENT_NO_SCHEMA    16  /* Don't allow database.table.column */
#define CLIENT_COMPRESS     32  /* Can use compression protocol */
#define CLIENT_ODBC     64  /* Odbc client */
#define CLIENT_LOCAL_FILES  128 /* Can use LOAD DATA LOCAL */
#define CLIENT_IGNORE_SPACE 256 /* Ignore spaces before '(' */
#define CLIENT_PROTOCOL_41  512 /* New 4.1 protocol */
#define CLIENT_INTERACTIVE  1024    /* This is an interactive client */
#define CLIENT_SSL              2048    /* Switch to SSL after handshake */
#define CLIENT_IGNORE_SIGPIPE   4096    /* IGNORE sigpipes */
#define CLIENT_TRANSACTIONS 8192    /* Client knows about transactions */
#define CLIENT_RESERVED         16384   /* Old flag for 4.1 protocol  */
#define CLIENT_RESERVED2        32768   /* Old flag for 4.1 authentication */
#define CLIENT_SECURE_CONNECTION 32768  /* New 4.1 authentication */
#define CLIENT_MULTI_STATEMENTS (1UL << 16) /* Enable/disable multi-stmt support */
#define CLIENT_MULTI_RESULTS    (1UL << 17) /* Enable/disable multi-results */
#define CLIENT_PS_MULTI_RESULTS (1UL << 18) /* Multi-results in PS-protocol */

#define CLIENT_PLUGIN_AUTH  (1UL << 19) /* Client supports plugin authentication */
#define CLIENT_CONNECT_ATTRS (1UL << 20) /* Client supports connection attributes */

/* Enable authentication response packet to be larger than 255 bytes. */
#define CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA (1UL << 21)

/* Don't close the connection for a connection with expired password. */
#define CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS (1UL << 22)

/**
 *   Capable of handling server state change information. Its a hint to the
 *     server to include the state change information in Ok packet.
 *     */
#define CLIENT_SESSION_TRACK (1UL << 23)
/* Client no longer needs EOF packet */
#define CLIENT_DEPRECATE_EOF (1UL << 24)

#define CLIENT_SSL_VERIFY_SERVER_CERT (1UL << 30)
#define CLIENT_REMEMBER_OPTIONS (1UL << 31)

#ifdef HAVE_COMPRESS
#define CAN_CLIENT_COMPRESS CLIENT_COMPRESS
#else
#define CAN_CLIENT_COMPRESS 0
#endif

/* Gather all possible capabilites (flags) supported by the server */
/* #define CLIENT_ALL_FLAGS  (CLIENT_LONG_PASSWORD \
                            | CLIENT_FOUND_ROWS \
                            | CLIENT_LONG_FLAG \
                            | CLIENT_CONNECT_WITH_DB \
                            | CLIENT_NO_SCHEMA \
                            | CLIENT_COMPRESS \
                            | CLIENT_ODBC \
                            | CLIENT_LOCAL_FILES \
                            | CLIENT_IGNORE_SPACE \
                            | CLIENT_PROTOCOL_41 \
                            | CLIENT_INTERACTIVE \
                            | CLIENT_SSL \
                            | CLIENT_IGNORE_SIGPIPE \
                            | CLIENT_TRANSACTIONS \
                            | CLIENT_RESERVED \
                            | CLIENT_RESERVED2 \
                            | CLIENT_MULTI_STATEMENTS \
                            | CLIENT_MULTI_RESULTS \
                            | CLIENT_PS_MULTI_RESULTS \
                            | CLIENT_SSL_VERIFY_SERVER_CERT \
                            | CLIENT_REMEMBER_OPTIONS \
                            | CLIENT_PLUGIN_AUTH \
                            | CLIENT_CONNECT_ATTRS \
                            | CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA \
                            | CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS \
                            | CLIENT_SESSION_TRACK \
                            | CLIENT_DEPRECATE_EOF \
)*/

/*
#define CLIENT_ALL_FLAGS  (CLIENT_LONG_PASSWORD \
                            | CLIENT_FOUND_ROWS \
                            | CLIENT_LONG_FLAG \
                            | CLIENT_CONNECT_WITH_DB \
                            | CLIENT_NO_SCHEMA \
                            | CLIENT_COMPRESS \
                            | CLIENT_ODBC \
                            | CLIENT_LOCAL_FILES \
                            | CLIENT_IGNORE_SPACE \
                            | CLIENT_PROTOCOL_41 \
                            | CLIENT_INTERACTIVE \
                            | CLIENT_SSL \
                            | CLIENT_IGNORE_SIGPIPE \
                            | CLIENT_TRANSACTIONS \
                            | CLIENT_RESERVED \
                            | CLIENT_MULTI_STATEMENTS \
                            | CLIENT_MULTI_RESULTS \
                            | CLIENT_PS_MULTI_RESULTS \
                            | CLIENT_SSL_VERIFY_SERVER_CERT \
                            | CLIENT_REMEMBER_OPTIONS \
                            | CLIENT_PLUGIN_AUTH \
                            | CLIENT_CONNECT_ATTRS \
                            | CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS \
)
*/

#define CLIENT_BASIC_FLAGS (((CLIENT_ALL_FLAGS & ~CLIENT_SSL) \
                                               & ~CLIENT_COMPRESS) \
                                               & ~CLIENT_SSL_VERIFY_SERVER_CERT)
/**
 *   Is raised when a multi-statement transaction
 *     has been started, either explicitly, by means
 *       of BEGIN or COMMIT AND CHAIN, or
 *         implicitly, by the first transactional
 *           statement, when autocommit=off.
 *           */
#define SERVER_STATUS_IN_TRANS     1
#define SERVER_STATUS_AUTOCOMMIT   2    /* Server in auto_commit mode */
#define SERVER_MORE_RESULTS_EXISTS 8    /* Multi query - next query exists */
#define SERVER_QUERY_NO_GOOD_INDEX_USED 16
#define SERVER_QUERY_NO_INDEX_USED      32
/**
 *   The server was able to fulfill the clients request and opened a
 *     read-only non-scrollable cursor for a query. This flag comes
 *       in reply to COM_STMT_EXECUTE and COM_STMT_FETCH commands.
 *       */
#define SERVER_STATUS_CURSOR_EXISTS 64
/**
 *   This flag is sent when a read-only cursor is exhausted, in reply to
 *     COM_STMT_FETCH command.
 *     */
#define SERVER_STATUS_LAST_ROW_SENT 128
#define SERVER_STATUS_DB_DROPPED        256 /* A database was dropped */
#define SERVER_STATUS_NO_BACKSLASH_ESCAPES 512
/**
 *   Sent to the client if after a prepared statement reprepare
 *     we discovered that the new statement returns a different
 *       number of result set columns.
 *       */
#define SERVER_STATUS_METADATA_CHANGED 1024
#define SERVER_QUERY_WAS_SLOW          2048

/**
 *   To mark ResultSet containing output parameter values.
 *   */
#define SERVER_PS_OUT_PARAMS            4096

/**
 *   Set at the same time as SERVER_STATUS_IN_TRANS if the started
 *     multi-statement transaction is a read-only transaction. Cleared
 *       when the transaction commits or aborts. Since this flag is sent
 *         to clients in OK and EOF packets, the flag indicates the
 *           transaction status at the end of command execution.
 *           */
#define SERVER_STATUS_IN_TRANS_READONLY 8192

/**
 *   This status flag, when on, implies that one of the state information has
 *     changed on the server because of the execution of the last statement.
 *     */
#define SERVER_SESSION_STATE_CHANGED (1UL << 14)

/**
 *   Server status flags that must be cleared when starting
 *     execution of a new SQL statement.
 *       Flags from this set are only added to the
 *         current server status by the execution engine, but
 *           never removed -- the execution engine expects them
 *             to disappear automagically by the next command.
 *             */
/* #define SERVER_STATUS_CLEAR_SET (SERVER_QUERY_NO_GOOD_INDEX_USED| \
SERVER_QUERY_NO_INDEX_USED|\
SERVER_MORE_RESULTS_EXISTS|\
SERVER_STATUS_METADATA_CHANGED |\
SERVER_QUERY_WAS_SLOW |\
SERVER_STATUS_DB_DROPPED |\
SERVER_STATUS_CURSOR_EXISTS|\
SERVER_STATUS_LAST_ROW_SENT|\
SERVER_SESSION_STATE_CHANGED)
*/

#define MYSQL_ERRMSG_SIZE   512
#define NET_READ_TIMEOUT    30      /* Timeout on read */
#define NET_WRITE_TIMEOUT   60      /* Timeout on write */
#define NET_WAIT_TIMEOUT    8*60*60     /* Wait for new query */

#define ONLY_KILL_QUERY         1


namespace dbproxy
{

class CMySQLNetPack : public CNetPack
{
public:
    CMySQLNetPack()
    {
        _min_pack_size = 4;
        _max_pack_size = MAX_PACKET_LENGTH;
    }
    virtual ~CMySQLNetPack() {}
    
    int Pack(const char* in_buffer, const uint32_t in_length, char* out_buffer, uint32_t& out_length);
    int Unpack(const char* in_buffer, const uint32_t in_length, char* out_buffer, uint32_t& out_buffer_length, uint32_t& out_data_length);
    
    bool CheckPack()
    {
        if((recv_pack_length <= (_max_pack_size - _min_pack_size)) &&
                (recv_pack_length >= 2)) {
            return true;
        } else {
            TRACE(1, "check pack length = " << recv_pack_length);
            return false;
        }
        
        return true;
    }
    
private:
    int Serialize(CStandardSerialize& aoStandardSerialize);
    
public:
    uint32_t send_pack_length;
    uint8_t send_seq;
    uint32_t recv_pack_length;
    uint8_t recv_seq;
};

} //end of namespace dbproxy

#endif //_MYSQL_NET_PACKET_H_











