file(REMOVE_RECURSE
  "CMakeFiles/code-coverage"
  "code-coverage.base"
  "code-coverage.capture"
  "code-coverage.info"
  "code-coverage.total"
  "code-coverage/index.html"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/code-coverage.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
