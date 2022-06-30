set(SRC_LIST calluserservice.cc ../user.pb.cc)

add_executable(ServiceConsumer ${SRC_LIST})

target_link_libraries(ServiceConsumer tinyRPC protobuf)
