file(GLOB chess_SRC
     "../src/*/*.cpp"
     "../src/*/*/*.cpp" 
     "../src/*/*/*/*.cpp"      
     "../src/*.cpp"
)
FOREACH(item ${chess_SRC})
  IF(${item} MATCHES "src/main.cpp")
    LIST(REMOVE_ITEM chess_SRC ${item})
  ENDIF(${item} MATCHES "src/main.cpp")
ENDFOREACH(item)

add_executable(q_string_test q_string_test.cpp ${chess_SRC})

target_link_libraries(q_string_test 
        Qt::Core
        Qt::Widgets
        Qt::Test
        ${OpenCV_LIBS}
        fmt::fmt
        cpptrace::cpptrace
        log4cplus::log4cplus)

add_test(NAME MyClassTest COMMAND q_string_test)        