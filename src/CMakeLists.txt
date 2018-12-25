add_library(sct arr.c exception.c fib.c hash.c ht.c slice.c sll.c xstdlib.c)
target_include_directories(sct PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
