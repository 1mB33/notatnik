#pragma once

#include "import_export.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>

namespace DbMan
{

_STATE_API bool InitializeState();

_STATE_API bool CreateNewNote( const std::string &noteName, int64_t ownerId, int64_t *noteId );

_STATE_API bool ListNotes( std::unordered_map<std::string, int64_t> &notes, int64_t ownerId );

_STATE_API bool ReadNote( int64_t noteId, int64_t ownerId, std::string &content );

_STATE_API bool UpdateNote( int64_t noteName, int64_t ownerId, const std::string &content );

_STATE_API bool DeleteNote( const std::string &noteName, int64_t ownerId );

} // namespace DbMan
