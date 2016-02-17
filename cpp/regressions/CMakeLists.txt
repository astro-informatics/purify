include(AddCatchTest)

function(add_regression targetname)
  cmake_parse_arguments(regr "" "" "LABELS" ${ARGN})
  add_catch_test(${targetname} ${regr_UNPARSED_ARGUMENTS} LABELS ${regr_LABELS} "regression")
  target_include_directories(test_${targetname} PUBLIC
    "${PROJECT_SOURCE_DIR}/cpp" ${Sopt_INCLUDE_DIRS})
endfunction()

add_regression(sdmm LIBRARIES libpurify cppurify ${Sopt_CPP_LIBRARY} ${Sopt_C_LIBRARY} )
