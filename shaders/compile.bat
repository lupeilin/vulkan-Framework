D:\VulkanSDK\1.3.283.0\Bin\glslangValidator.exe  -V lessionShader.vert -o vs.spv
D:\VulkanSDK\1.3.283.0\Bin\glslangValidator.exe  -V lessionShader.frag -o fs.spv



xcopy "*.spv" "..\out\build\x64-Debug\shaders\" /Y


pause