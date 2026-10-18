#include "manager.hpp"

#include <sqlpp11/postgresql/connection.h>
#include <sqlpp11/postgresql/postgresql.h>
#include <sqlpp11/sqlpp11.h>

struct State
{
    using Connection        = sqlpp::postgresql::connection;
    using Connection_config = sqlpp::postgresql::connection_config;

    State();
    ~State() noexcept = default;

    State( State && ) noexcept                 = default;
    State &operator=( State && ) noexcept      = default;
    State( const State & ) noexcept            = delete;
    State &operator=( const State & ) noexcept = delete;

    Connection_config Config;
    Connection        Db;
};

State::State() {}

static State g_State = {};

void DbMan::InitializeState()
{
    g_State.Config.dbname   = "notepad_dev_db";
    g_State.Config.user     = "dev";
    g_State.Config.password = "dev";
    g_State.Config.debug    = true;

    g_State.Db = State::Connection( g_State.Config );
}
