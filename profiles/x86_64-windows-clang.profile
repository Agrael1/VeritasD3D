[settings]
os=Windows
arch=x86_64
build_type=Release
compiler=clang
compiler.version=22
compiler.cppstd=26
compiler.runtime=dynamic
compiler.runtime_type=Release
compiler.runtime_version=v145

[conf]
tools.cmake.cmaketoolchain:generator=Ninja
tools.build:compiler_executables={"c": "clang", "cpp": "clang++"}