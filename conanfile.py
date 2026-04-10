import json
from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMakeToolchain, CMakeDeps
from conan.tools.files import load, save

class MyProject(ConanFile):
    settings = "os", "compiler", "build_type", "arch"

    def requirements(self):
        self.requires("wisdom/0.7.0")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        
        tc = CMakeToolchain(self)
        # Redirect Conan's preset output so CMake doesn't load it as the default User file
        tc.user_presets_path = "ConanPresets.json"
        tc.generate()

        # 3. Intercept the generated JSON and hide the presets
        presets_path = "CMakePresets.json"
        presets_content = load(self, presets_path)
        presets_json = json.loads(presets_content)

        # Loop through configure, build, and test presets and hide all of them
        for preset_type in ["configurePresets", "buildPresets", "testPresets"]:
            if preset_type in presets_json:
                for preset in presets_json[preset_type]:
                    preset["hidden"] = True

        # 4. Save the modified JSON back to the file
        save(self, presets_path, json.dumps(presets_json, indent=2))
