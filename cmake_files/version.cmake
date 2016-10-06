include(VersionAndGitRef)
set_version(2.0.0)
get_gitref()

set(version ${Purify_VERSION})
string(REGEX REPLACE "\\." ";" version "${Purify_VERSION}")
list(GET version 0 Purify_VERSION_MAJOR)
list(GET version 1 Purify_VERSION_MINOR)
list(GET version 2 Purify_VERSION_PATCH)

