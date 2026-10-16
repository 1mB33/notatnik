#include "manager.hpp"

#include <sqlpp11/postgresql/connection.h>
#include <sqlpp11/postgresql/postgresql.h>
#include <sqlpp11/sqlpp11.h>

struct State
{
    using connection        = sqlpp::postgresql::connection;
    using connection_config = sqlpp::postgresql::connection_config;

    State();
    ~State() noexcept = default;

    State( State && ) noexcept                 = default;
    State &operator=( State && ) noexcept      = default;
    State( const State & ) noexcept            = delete;
    State &operator=( const State & ) noexcept = delete;

    connection_config m_Config;
    connection        m_Db;
};

State::State() {}

static State g_State = {};

int fn()
{
    return 1;
}
