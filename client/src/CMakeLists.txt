# 设置可执行文件搜索路径
include_directories(../../protos)

# 添加到SRC_LIST
aux_source_directory(../../protos SRC_LIST)
aux_source_directory(. SRC_LIST)

# 指定生成可执行文件
add_executable(client ${SRC_LIST})

# 指定链接的目录
target_link_libraries(client protobuf pthread)
