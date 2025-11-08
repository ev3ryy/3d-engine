file(GLOB_RECURSE HLSL_FILES LIST_DIRECTORIES false "*.hlsl")

add_custom_target(CompileShaders ALL)

function(COMPILE_HLSL_SHADER BASE_NAME SOURCE_FILE)
    get_filename_component(FILE_NAME ${SOURCE_FILE} NAME)
    
    # VS SPIR-V
    set(TARGET_VS_SPV "${SHADERS_CACHE_DIR}/${BASE_NAME}_vert_Vulkan_0.spv")
    add_custom_command(
        OUTPUT  ${TARGET_VS_SPV}
        COMMAND ${DXC_EXECUTABLE} 
        ARGS    -T vs_6_2 -E VSMain 
                -spirv -fspv-target-env=vulkan1.1 
                ${SOURCE_FILE} 
                -Fo ${TARGET_VS_SPV}
        DEPENDS ${SOURCE_FILE}
        COMMENT "Compiling VS (Vulkan): ${FILE_NAME}"
        VERBATIM
        USES_TERMINAL
    )
    list(APPEND COMPILED_SHADERS ${TARGET_VS_SPV})
    
    # PS SPIR-V
    set(TARGET_PS_SPV "${SHADERS_CACHE_DIR}/${BASE_NAME}_frag_Vulkan_0.spv")
    add_custom_command(
        OUTPUT  ${TARGET_PS_SPV}
        COMMAND ${DXC_EXECUTABLE} 
        ARGS    -T ps_6_2 -E PSMain 
                -spirv -fspv-target-env=vulkan1.1
                ${SOURCE_FILE} 
                -Fo ${TARGET_PS_SPV}
        DEPENDS ${SOURCE_FILE}
        COMMENT "Compiling PS (Vulkan): ${FILE_NAME}"
        VERBATIM
        USES_TERMINAL
    )
    list(APPEND COMPILED_SHADERS ${TARGET_PS_SPV})

    
    # VS DXIL (why not?)
    set(TARGET_DXIL_VERT "${SHADERS_CACHE_DIR}/${BASE_NAME}_vert_D3D12_0.dxil")
    add_custom_command(
        OUTPUT ${TARGET_DXIL_VERT}
        COMMAND ${DXC_EXECUTABLE} 
        ARGS    -T vs_6_2 -E VSMain ${SOURCE_FILE} -Fo ${TARGET_DXIL_VERT}
        DEPENDS ${SOURCE_FILE}
        COMMENT "Compiling VS (D3D12): ${FILE_NAME}"
    )
    list(APPEND COMPILED_SHADERS ${TARGET_DXIL_VERT})
    
    # PS DXIL (why not?:D)
    set(TARGET_DXIL_FRAG "${SHADERS_CACHE_DIR}/${BASE_NAME}_frag_D3D12_0.dxil")
    add_custom_command(
        OUTPUT ${TARGET_DXIL_FRAG}
        COMMAND ${DXC_EXECUTABLE} 
        ARGS    -T ps_6_2 -E PSMain ${SOURCE_FILE} -Fo ${TARGET_DXIL_FRAG}
        DEPENDS ${SOURCE_FILE}
        COMMENT "Compiling PS (D3D12): ${FILE_NAME}"
    )
    list(APPEND COMPILED_SHADERS ${TARGET_DXIL_FRAG})

    add_custom_target(${BASE_NAME}_shaders DEPENDS ${COMPILED_SHADERS})
    add_dependencies(CompileShaders ${BASE_NAME}_shaders)
endfunction()

foreach(SHADER_FILE ${HLSL_FILES})
    get_filename_component(BASE_NAME ${SHADER_FILE} NAME_WE)
    COMPILE_HLSL_SHADER(${BASE_NAME} ${SHADER_FILE})
endforeach()