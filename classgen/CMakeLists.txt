find_package(Python3 REQUIRED COMPONENTS Interpreter)

function(build_classes filename)
    # The generators print UTF-8 (they emit a Chinese banner). Capturing that
    # through execute_process would decode it with the host code page (cp936 on
    # a Chinese Windows) and mangle it, so run_classgen.py writes the .inl
    # itself, with the encoding stated in Python.
    execute_process(
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/run_classgen.py
                ${CMAKE_CURRENT_SOURCE_DIR}/${filename}.py
                ${CMAKE_CURRENT_SOURCE_DIR}/../source/packfile/${filename}.inl
        RESULT_VARIABLE result)
    if(NOT result EQUAL 0)
        message(FATAL_ERROR "classgen failed for ${filename} (exit ${result})")
    endif()
endfunction()

build_classes(hk_base)
build_classes(hka_skeleton)
build_classes(hkx_environment)
build_classes(hk_rootlevelcontainer)
build_classes(hka_animation)
build_classes(hka_animation_binding)
build_classes(hka_animation_container)
build_classes(hka_animation_interleaved)
build_classes(hka_animated_reference_frame)
build_classes(hka_animation_delta)
build_classes(hka_animation_spline)
build_classes(hka_animation_wavelet)
build_classes(custom/hka_animation_lossless_compressed)
build_classes(hka_animated_reference_frame_default)
