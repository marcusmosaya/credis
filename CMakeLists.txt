cmake_minimum_required(VERSION 3.10)
project(credis VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include headers
include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(credis 
 src/main.cpp
 src/network/server.cpp
 src/network/Resp.cpp
 src/commands/string_command.cpp
 src/commands/command_handler.cpp
 src/commands/set_command.cpp
 src/commands/sset_command.cpp
 src/commands/keys_command.cpp
 src/commands/server_command.cpp
 src/commands/hash_command.cpp
 src/commands/list_command.cpp
 src/datastore/memory_store.cpp
 src/datastore/expire.cpp
 src/datastore/persistance/Aof_persist.cpp
 src/engine/engine.cpp
 src/utils/functions.cpp
 src/utils/sset.cpp
 src/utils/interval.cpp
)


