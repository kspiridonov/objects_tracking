file(GLOB SRC
     "../src/*/*.cpp"
     "../src/*/*/*.cpp" 
     "../src/*/*/*/*.cpp"
     "../src/*/*/*/*/*.cpp"
     "../src/*.cpp"
)
FOREACH(item ${SRC})
  IF(${item} MATCHES "src/main.cpp")
    LIST(REMOVE_ITEM SRC ${item})
  ENDIF(${item} MATCHES "src/main.cpp")
ENDFOREACH(item)

add_executable(q_string_test q_string_test.cpp ${SRC})

target_link_libraries(q_string_test 
        Qt::Core
        Qt::Widgets
        Qt::Test
        ${OpenCV_LIBS}
        fmt::fmt
        cpptrace::cpptrace
        log4cplus::log4cplus)

add_executable(movement_rects_frame_processor movement_rects_frame_processor_test.cpp ${SRC})
target_link_libraries(movement_rects_frame_processor
        Qt::Core
        Qt::Widgets
        Qt::Test
        ${OpenCV_LIBS}
        fmt::fmt
        cpptrace::cpptrace
        log4cplus::log4cplus)


add_test(NAME MyClassTest COMMAND q_string_test)
add_test(NAME MovementRectsFrameProcessor COMMAND movement_rects_frame_processor)