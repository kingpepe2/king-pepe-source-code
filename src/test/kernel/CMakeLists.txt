add_executable(test_kernel
    test_kernel.cpp
)

target_link_libraries(test_kernel
  PRIVATE
    core_interface
    bitcoinkernel
    Boost::headers
)

get_target_property(bitcoinkernel_type bitcoinkernel TYPE)
if(bitcoinkernel_type STREQUAL "SHARED_LIBRARY")
  target_link_libraries(test_kernel PRIVATE bitcoin_consensus)
endif()

add_windows_application_manifest(test_kernel)

add_test(NAME test_kernel COMMAND test_kernel)
