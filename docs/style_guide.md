### naming conventions

# variables
    regular variables
        - Snake case
            - ```my_variable```
    constant variables
        - lowercase k prefixing then pascal case
            - ```kMyConstantVariable```

# program structure
    types (class, structs, type alias, enums, template parameters)
        - Pascal case
            - ```MyClass```, ```MyStruct``` etc

    namespaces
        - Snake case
            - ```my_namespace```
        - should be general to the subsystem and hardware

    functions
        regular functions
            - Pascal case
                - ```MyFunction()```
        accessor and mutator (getters and setter)
            - Snake case
                - ```get_my_variable()```

# file structure
    directories
        - Snake case
            - ```parent_directory/sub_directory```    
    files
        - Snake case
            - ```my_generic_file```
    cpp source files
        - Snake case, with extension .cpp
            - ```my_cpp_source.cpp```
    cpp header files
        - Snake case, with extension .h
            - ```my_cpp_header.h```
