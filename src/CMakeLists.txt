set(SRC_LIST 
    tinyRPCApplication.cc 
    tinyRPCProvider.cc 
    tinyRPCConfig.cc
    tinyRPCHeader.pb.cc
    tinyRPCChannel.cc
)
#aux_source_directory(. SRC_LIST)
# SHARED 动态库 add_library(tinyRPC SHARED ${SRC_LIST})
add_library(tinyRPC ${SRC_LIST})
# 依赖关系顺序需要正确
target_link_libraries(tinyRPC muduo_net muduo_base pthread)