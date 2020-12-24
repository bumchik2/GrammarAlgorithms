project(01-simple-library)

set(01-source-files 
	"${PROJECT_SOURCE_DIR}/chomsky_to_greybuh.cpp"
	"${PROJECT_SOURCE_DIR}/earley.cpp"
	"${PROJECT_SOURCE_DIR}/grammar.cpp" CACHE INTERNAL "")

add_library(01-simple-library SHARED ${01-source-files} 
    ${Testing_SOURCE_DIR}/include/chomsky_to_greybuh.h
    ${Testing_SOURCE_DIR}/include/earley.h
    ${Testing_SOURCE_DIR}/include/grammar.h)



