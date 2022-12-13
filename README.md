# **Film Grain Synthesis Library**
## **Introduction**
This library implements Film Grain Synthesis based on SMPTE-RDD5 [frequency-filtering method]. Following are the features of the library 

*  Bit exact implementation of film grain synthesis as per RDD5 [frequency-filtering method] with extensions to support up to 10-bit and 4k resolutions
*  Optimized on ARM and Intel platforms 
*  Library is multi thread enabled 
*  Validated with AVC, HEVC and VVC decoders on android smartphone and browser framework 
*  Repository contains source and header files for film grain synthesis along with sample application files 

## FGS Build steps

### **Prerequisites**
* cmake >= 3.25
* gcc / clang toolchain (For x86-64 and aarch64 Linux builds)
* Visual Studio (For Windows builds)

### ***After Cloning***
* To download all submodules - 
  ```
    libfgsPath=<Path to libfgs repo>
    cd ${libfgsPath}
    git submodule update --init --recursive
  ```
### **Building for x86-64 Linux**
1. Create a buildtree
  ```
    libfgsPath=<Path to libfgs repo>
    buildTree=<Name of buildTree folder>
    installPath=<Path to install folder>
    cd ${libfgsPath}
    cmake . -B ${buildTree} -DCMAKE_INSTALL_BINDIR:STRING=${installPath}
  ```
2. Build using buildtree
  ```      
    cmake --build ${buildTree}
  ```
3. Install (Installs to 'bin' folder)
  ```      
    cmake --install ${buildTree}
  ```
4. Command to run the executable
  ```      
    ./fgsapp <path of configuration file>
  ```
### **Building for aarch64 Linux**
1. Create a buildtree
  ```
    toolchainPath=<Path to aarch64 toolchain>
    libfgsPath=<Path to libfgs repo>
    buildTree=<Name of buildTree folder>
    installPath=<Path to install folder>
    cd ${libfgsPath}
    PATH=${toolchainPath}:${PATH} cmake . -B ${buildTree}\
    -DCMAKE_TOOLCHAIN_FILE=./cmake/toolchains/aarch64_toolchain.cmake\
    -DCMAKE_INSTALL_BINDIR:STRING=${installPath}
  ```
2. Build using buildtree
  ```      
    cmake --build ${buildTree}
  ```
3. Install (Installs to 'bin' folder)
  ```      
    cmake --install ${buildTree}
  ```
4. Command to run the executable
  ```      
    ./fgsapp <path of configuration file>
  ```
### **Building for x86-64 Windows**
1. Open git bash
2. Create a buildtree
  ```
    libfgsPath=<Path to libfgs repo>
    buildTree=<Name of buildTree folder>
    installPath=<Path to install folder>
    cd ${libfgsPath}
    cmake . -B ${buildTree} -A x64\
    -DCMAKE_INSTALL_BINDIR:STRING=${installPath}
  ```
2. Build using buildtree
  ```      
    cmake --build ${buildTree} -- -p:Configuration="Release"
  ```
3. Install (Installs to 'bin' folder)
  ```      
    cmake --install ${buildTree}
  ```
4. Command to run the executable
  ```      
    ./fgsapp.exe <path of configuration file>
  ```
### Guidelines working with GitHub
Please refer to the [guidelines](GUIDELINES.md) for the following processes
- Reporting issues
- Creating Pull Request
- Commit message standards
