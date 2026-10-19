#include "manager.hpp"
#include "schema.h"

#include <cassert>
#include <sqlpp11/default_value.h>
#include <sqlpp11/insert.h>
#include <sqlpp11/null.h>
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

    Connection_config          Config;
    Connection                 Db;
    notepad_dev_db::PublicNote NoteTab;
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

void DbMan::CreateNote()
{
    assert( g_State.Db.is_connected() );

    g_State.Db( sqlpp::insert_into( g_State.NoteTab )
                    .set( g_State.NoteTab.id        = sqlpp::default_value,
                          g_State.NoteTab.localPath = "",
                          g_State.NoteTab.ownerId   = sqlpp::null ) );
}
