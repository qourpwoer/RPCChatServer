# 设置可执行文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/../../protos)
include_directories(${PROJECT_SOURCE_DIR}/../third/mysql)
include_directories(${PROJECT_SOURCE_DIR}/../third/redis)
include_directories(${PROJECT_SOURCE_DIR}/../third/zookeeper)
include_directories(${PROJECT_SOURCE_DIR}/../third/mymuduo)
include_directories(${PROJECT_SOURCE_DIR}/../third/mprpc/proto)
include_directories(${PROJECT_SOURCE_DIR}/../third/mprpc)
include_directories(${PROJECT_SOURCE_DIR}/include)

# 添加到SRC_LIST
aux_source_directory(${PROJECT_SOURCE_DIR}/../third/mprpc/mymuduo SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/../../protos SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/../third/mysql SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/../third/redis SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/../third/zookeeper SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/../third/mymuduo SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/../third/mprpc/proto SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/../third/mprpc SRC_LIST)
aux_source_directory(. SRC_LIST)

# 指定生成可执行文件
add_executable(GroupService ${SRC_LIST})

# 指定链接的目录
target_link_libraries(GroupService protobuf pthread zookeeper_mt hiredis mysqlclient)
