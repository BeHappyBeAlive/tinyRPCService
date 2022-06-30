set(SRC_LIST userservice.cc ../user.pb.cc)

add_executable(ServiceProvider ${SRC_LIST})

target_link_libraries(ServiceProvider tinyRPC protobuf)
