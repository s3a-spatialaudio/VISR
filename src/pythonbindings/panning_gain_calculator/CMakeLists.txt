# compile mex

set(CPP_FILE panning_gain_calculator)

set(PROJECT_NAME panning_gain_calculator)

set( ADDITIONAL_CPP_FILES )

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIR})

set(CPP_FILE_NAME ${CPP_FILE}.cpp)

add_library(${PROJECT_NAME} SHARED ${CPP_FILE_NAME} ${ADDITIONAL_CPP_FILES} )

# Add any extras dependencies
target_link_libraries(${PROJECT_NAME} panning efl )
target_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARY} )

install(TARGETS ${PROJECT_NAME} DESTINATION ../python)
