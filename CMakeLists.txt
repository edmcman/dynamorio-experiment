set (CMAKE_CXX_STANDARD 17)
add_library(myclient SHARED myclient.cpp helpers.cpp options.cpp gen-cpp/ConcreteEvaluator_server.cpp gen-cpp/ConcreteEvaluator.cpp gen-cpp/concolic_constants.cpp gen-cpp/concolic_types.cpp)

find_package(Thrift)
if (NOT Thrift_FOUND)
  message(FATAL_ERROR "thrift package required to build")
endif(NOT Thrift_FOUND)

find_package(DynamoRIO)
if (NOT DynamoRIO_FOUND)
  message(FATAL_ERROR "DynamoRIO package required to build")
endif(NOT DynamoRIO_FOUND)
configure_DynamoRIO_client(myclient)
use_DynamoRIO_extension(myclient droption)

target_link_libraries(myclient thrift::thrift)
