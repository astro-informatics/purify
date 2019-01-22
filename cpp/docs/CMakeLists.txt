# Set some Doxygen flags
set(PURIFY_DOXY_PROJECT_NAME "PURIFY")
set(PURIFY_DOXY_PROJECT_BRIEF "Next-generation radio interferometric imaging")
set(PURIFY_DOXY_INPUT "${PROJECT_SOURCE_DIR}/cpp")

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
  ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
)

if (web)
  file(DOWNLOAD http://astro-informatics.github.io/sopt/sopt.tag ${CMAKE_CURRENT_BINARY_DIR}/sopt/sopt.tag)
endif()

add_custom_target(doc ALL
  COMMAND doxygen
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})


if (web)
   add_custom_target(docweb
     COMMAND sed -i -e 's@namespacepurify@http://astro-informatics.github.io/purify/&@g' -e 's@<filename>@&http://astro-informatics.github.io/purify/@g' ${CMAKE_CURRENT_BINARY_DIR}/html/purify.tag
     DEPENDS doc)
endif()
