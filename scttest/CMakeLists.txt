add_executable(scttest arr.c fib.c hash.c ht.c sll.c test.c)
target_link_libraries(scttest LINK_PUBLIC sct)
