# CMakeList.txt : CMake project for Unreal, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.22)

project(WinD3D CXX)

list(APPEND CMAKE_MODULE_PATH ${PLUGINS_DIR})
set(include_dir "${CMAKE_CURRENT_SOURCE_DIR}/include")

find_package(assimp REQUIRED)

add_library (${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME}
	PRIVATE 
        "cpp.hint"
        "src/Engine/Graphics.cpp"
        "src/Engine/Bindable/BlendState.cpp"
        "src/Engine/Bindable/ConstantBuffer.cpp"
        "src/Engine/Bindable/CubeTexture.cpp"
        "src/Engine/Bindable/DepthStencil.cpp"
        "src/Engine/Bindable/DomainShader.cpp"
        "src/Engine/Bindable/GraphicsResource.cpp"
        "src/Engine/Bindable/HullShader.cpp"
        "src/Engine/Bindable/IndexBuffer.cpp"
        "src/Engine/Bindable/InputLayout.cpp"
        "src/Engine/Bindable/Light.cpp"
        "src/Engine/Bindable/NullPixelShader.cpp"
        "src/Engine/Bindable/PixelShader.cpp"
        "src/Engine/Bindable/RasterizerState.cpp"
        "src/Engine/Bindable/RenderTarget.cpp"
        "src/Engine/Bindable/Sampler.cpp"
        "src/Engine/Bindable/ShadewRasterizer.cpp"
        "src/Engine/Bindable/ShadowSampler.cpp"
        "src/Engine/Bindable/Stencil.cpp"
        "src/Engine/Bindable/Texture.cpp"
        "src/Engine/Bindable/TransformCBuf.cpp"
        "src/Engine/Bindable/VertexBuffer.cpp"
        "src/Engine/Bindable/VertexShader.cpp"

        "src/Engine/Dynamic/DynamicConstant.cpp"
        "src/Engine/Dynamic/LayoutCodex.cpp"
        "src/Engine/Dynamic/VertexLayout.cpp"

        "src/Engine/Graph/BlurOutlineRenderGraph.cpp"
        "src/Engine/Graph/DeferredRenderGraph.cpp"
        "src/Engine/Graph/GalaxyRenderGraph.cpp"

        "src/Engine/Loading/Image.cpp"
        "src/Engine/Loading/Material.cpp"
        "src/Engine/Loading/Mesh.cpp"
        "src/Engine/Loading/Model.cpp"
        "src/Engine/Loading/Node.cpp"

        "src/Engine/Pass/AmbientPass.cpp"
        "src/Engine/Pass/BindingPass.cpp"
        "src/Engine/Pass/BufferClearPass.cpp"
        "src/Engine/Pass/FilteringPass.cpp"
        "src/Engine/Pass/FullscreenPass.cpp"
        "src/Engine/Pass/GbufferPass.cpp"
        "src/Engine/Pass/HorizontalBlurPass.cpp"
        "src/Engine/Pass/LightingPass.cpp"
        "src/Engine/Pass/RenderQueuePass.cpp"
        "src/Engine/Pass/VerticalBlurPass.cpp"

        "src/Engine/Rendering/Drawable.cpp"
        "src/Engine/Rendering/Job.cpp"
        "src/Engine/Rendering/Pass.cpp"
        "src/Engine/Rendering/RenderGraph.cpp"
        "src/Engine/Rendering/Sink.cpp"
        "src/Engine/Rendering/Source.cpp"
        "src/Engine/Rendering/Step.cpp"
        "src/Engine/Rendering/Technique.cpp"

        "src/Engine/Scene/BillboardComponent.cpp"
        "src/Engine/Scene/Skybox.cpp"
        "src/Engine/Scene/SolidSphere2.cpp"
        "src/Engine/Scene/Cursor.cpp"


        "src/Engine/Util/ImGUIManager.cpp"
        "src/Engine/Util/RenderGraphException.cpp"
        "src/Engine/Util/VFileDialog.cpp"

	PUBLIC FILE_SET HEADERS 
        BASE_DIRS ${include_dir}
        FILES 
		    "include/Engine/Graphics.h"
            "include/Engine/Bindable/AsyncResource.h"
            "include/Engine/Bindable/Bindable.h"
            "include/Engine/Bindable/BindableCommons.h"
            "include/Engine/Bindable/BlendState.h"
            "include/Engine/Bindable/BufferResource.h"
            "include/Engine/Bindable/Codex.h"
            "include/Engine/Bindable/ConstantBuffer.h"
            "include/Engine/Bindable/ConstantBuffersEX.h"
            "include/Engine/Bindable/CubeTexture.h"
            "include/Engine/Bindable/DepthStencil.h"
            "include/Engine/Bindable/DomainShader.h"
            "include/Engine/Bindable/GraphicsResource.h"
            "include/Engine/Bindable/HullShader.h"
            "include/Engine/Bindable/IndexBuffer.h"
            "include/Engine/Bindable/InputLayout.h"
            "include/Engine/Bindable/Light.h"
            "include/Engine/Bindable/NullPixelShader.h"
            "include/Engine/Bindable/PixelShader.h"
            "include/Engine/Bindable/RasterizerState.h"
            "include/Engine/Bindable/RenderTarget.h"
            "include/Engine/Bindable/Sampler.h"
            "include/Engine/Bindable/ShadowRasterizer.h"
            "include/Engine/Bindable/ShadowSampler.h"
            "include/Engine/Bindable/Stencil.h"
            "include/Engine/Bindable/Texture.h"
            "include/Engine/Bindable/Topology.h"
            "include/Engine/Bindable/TransformCBuf.h"
            "include/Engine/Bindable/VertexBuffer.h"
            "include/Engine/Bindable/VertexShader.h"
            "include/Engine/Bindable/Viewport.h"

            "include/Engine/Dynamic/DynamicConstant.h"
            "include/Engine/Dynamic/LayoutCodex.h"
            "include/Engine/Dynamic/VertexBuffer.h"
            "include/Engine/Dynamic/VertexInfo.h"
            "include/Engine/Dynamic/VertexLayout.h"

            "include/Engine/Graph/BlurOutlineRenderGraph.h"
            "include/Engine/Graph/DeferredRenderGraph.h"
            "include/Engine/Graph/GalaxyRenderGraph.h"
            "include/Engine/Graph/StereoGraph.h"

            "include/Engine/Loading/Image.h"
            "include/Engine/Loading/Material.h"
            "include/Engine/Loading/Mesh.h"
            "include/Engine/Loading/Model.h"
            "include/Engine/Loading/Node.h"

            "include/Engine/Pass/AmbientPass.h"
            "include/Engine/Pass/BindingPass.h"
            "include/Engine/Pass/BlurOutlineDrawingPass.h"
            "include/Engine/Pass/BufferClearPass.h"
            "include/Engine/Pass/FilteringPass.h"
            "include/Engine/Pass/FullscreenEffect.h"
            "include/Engine/Pass/FullscreenPass.h"
            "include/Engine/Pass/GbufferPass.h"
            "include/Engine/Pass/HorizontalBlurPass.h"
            "include/Engine/Pass/LambertianPass.h"
            "include/Engine/Pass/LightingPass.h"
            "include/Engine/Pass/OutlineMaskGenerationPass.h"
            "include/Engine/Pass/RenderQueuePass.h"
            "include/Engine/Pass/ShadowPass.h"
            "include/Engine/Pass/VerticalBlurPass.h"

            "include/Engine/Probes/ModelProbe.h"
            "include/Engine/Probes/TechniqueProbe.h"
            "include/Engine/Probes/TestModelProbe.h"

            "include/Engine/Rendering/Drawable.h"
            "include/Engine/Rendering/IDrawable.h"
            "include/Engine/Rendering/Job.h"
            "include/Engine/Rendering/Pass.h"
            "include/Engine/Rendering/RenderGraph.h"
            "include/Engine/Rendering/Sink.h"
            "include/Engine/Rendering/SinksT.h"
            "include/Engine/Rendering/Source.h"
            "include/Engine/Rendering/SourcesT.h"
            "include/Engine/Rendering/Step.h"
            "include/Engine/Rendering/Technique.h"

            "include/Engine/Scene/BillboardComponent.h"
            "include/Engine/Scene/Cube.h"
            "include/Engine/Scene/IndexedTriangleList.h"
            "include/Engine/Scene/Plane.h"
            "include/Engine/Scene/Skybox.h"
            "include/Engine/Scene/SolidSphere2.h"
            "include/Engine/Scene/Sphere.h"

            "include/Engine/Util/ImGUIManager.h"
            "include/Engine/Util/MathHelper.h"
            "include/Engine/Util/RenderGraphException.h"
            "include/Engine/Util/scoped_semaphore.h"
            "include/Engine/Util/Utility.h"
            "include/Engine/Util/VFileDialog.h"
 "include/Engine/Pass/CursorPass.h" "include/Engine/Bindable/CursorTransform.h" "include/Engine/Scene/Cursor.h" )


target_link_libraries(${PROJECT_NAME} 
    PUBLIC 
        assimp::assimp

        Shared
        DXTEX
        HBAO
        d3d11
        dxguid
        imguizmo
) 

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_precompile_headers(${PROJECT_NAME} PUBLIC "include/pch.h")
set_target_properties(${PROJECT_NAME} 
    PROPERTIES
        CXX_STANDARD 23
)
add_dependencies(${PROJECT_NAME}
    compile_shaders
)


install(TARGETS "${PROJECT_NAME}" EXPORT WinD3DTargets
    FILE_SET HEADERS DESTINATION include
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)

install(EXPORT WinD3DTargets
     FILE "WinD3DTargets.cmake"
     NAMESPACE WinD3D::
     DESTINATION "lib/cmake/${PROJECT_NAME}"
)

export(EXPORT WinD3DTargets
       FILE "WinD3DTargets.cmake"
       NAMESPACE WinD3D::
)