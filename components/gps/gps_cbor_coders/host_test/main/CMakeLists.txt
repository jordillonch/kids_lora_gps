idf_component_register(
    SRCS "gps_cbor_coders_test.cpp"
    "../../gps_cbor_coders.c"
    INCLUDE_DIRS
    "."
    "../../include"
    PRIV_INCLUDE_DIRS
    REQUIRES cmock cbor)

target_compile_options(${COMPONENT_LIB} PUBLIC --coverage)
target_link_libraries(${COMPONENT_LIB} --coverage)
