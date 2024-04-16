cmake --build cmake_build --target "${{ github.event.inputs.target }}"
cat b85_ble_sdk/cmake_build/CMakeFiles/825x_ble_sample.dir/linkLibs.rsp
exit 0
