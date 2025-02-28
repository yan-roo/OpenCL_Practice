# Tutorial: Setting Up and Using OpenCL in Xcode

This tutorial will guide you from scratch on how to use OpenCL in the Xcode environment on macOS, using the C++ wrapper (`opencl.hpp`) as an example. The ultimate goal is to compile and run an OpenCL program in Xcode.

---

## Table of Contents
1. [Prerequisites](#1-prerequisites)
2. [Creating an Xcode Project](#2-creating-an-xcode-project)
3. [Creating or Adding a Target](#3-creating-or-adding-a-target)
4. [Adding the OpenCL C++ Wrapper (cl.hpp or opencl.hpp)](#4-adding-the-opencl-c-wrapper-clhpp-or-openclhpp)
5. [Setting Header Search Paths](#5-setting-header-search-paths)
6. [Link Binary with Libraries](#6-link-binary-with-libraries)
7. [Setting the OpenCL Version](#7-setting-the-opencl-version)
8. [Compilation and Testing](#8-compilation-and-testing)
9. [Limitations of Xcode for OpenCL Versions](#9-limitations-of-xcode-for-opencl-versions)
10. [FAQ and Additional Information](#10-faq-and-additional-information)

---

## 1. Prerequisites
- **macOS** system (includes OpenCL.framework, supports up to version 1.2).
- **Xcode** (a newer version is recommended for easier project and target management).
- **GitHub (optional):** To use the C++ wrapper provided by Khronos, you can obtain it from the [OpenCL-CLHPP](https://github.com/KhronosGroup/OpenCL-CLHPP) repository.
- **Homebrew (optional):** If you need `clang-format` or other tools, you can install them via Homebrew.

---

## 2. Creating an Xcode Project

1. Open Xcode → `File` → `New` → `Project...`
2. In the macOS category, select **Command Line Tool**.
3. Enter a project name (e.g., `OpenCL_project`) and select C++ as the language.
4. Choose a location to save the project and click **Create**.

A basic project containing a `main.cpp` file will be generated.

---

## 3. Creating or Adding a Target
If you plan to have multiple executable programs (multiple `main.cpp` files) within the same project, you can create a separate **Target** for each program:

1. Select the project file (the blue icon on the left).
2. In the Targets section, click the `+` button and choose **Command Line Tool** or another template.
3. Name the new Target and complete the setup.
4. In the project navigator, ensure the corresponding `main.cpp` is checked under **Target Membership**.  
   - You can do this in the File Inspector by checking or unchecking the appropriate Target.

This allows you to compile and run different `main.cpp` files by switching schemes (using the dropdown menu in the top left).

---

## 4. Adding the OpenCL C++ Wrapper (cl.hpp or opencl.hpp)
macOS only includes the C version of OpenCL (`#include <OpenCL/opencl.h>`) and does not provide Khronos's C++ wrapper. Therefore, you need to download it manually.

1. Go to the [OpenCL-CLHPP](https://github.com/KhronosGroup/OpenCL-CLHPP) repository.
2. Download or clone the repository and locate the file you need (e.g., `opencl.hpp` or `cl2.hpp`).
3. Create a folder named `CL` (or any name you prefer) in your project directory, and place `opencl.hpp` inside it.
4. In the Xcode Project Navigator, drag this folder into your project and check **“Copy items if needed”**.

---

## 5. Setting Header Search Paths
Let the compiler know where to find `opencl.hpp`:

1. Select your project → choose your Target → **Build Settings**.
2. Search for **Header Search Paths**.
3. Add a new path, for example: `$(PROJECT_DIR)/CL` (Assuming the `CL` folder is at the same level as the `.xcodeproj` file.)
4. If you need to support subdirectories, set the option to **recursive**.

In your code, you can include the header like this:
```cpp
#include <CL/opencl.hpp>  // if the file name is opencl.hpp

// or

#include "CL/opencl.hpp"

// (Depending on whether you use angle brackets or quotes, and based on your path settings)
```

---

## 6. Link Binary with Libraries
On macOS, OpenCL is provided as OpenCL.framework, which must be linked manually:

1.    Select your Target → **Build Phases** → **Link Binary With Libraries**.
2.    Click the + button, search for OpenCL.framework, and add it.
3.    Ensure that the framework appears in the list for that phase.

---

## 7. Setting the OpenCL Version
macOS only supports OpenCL 1.2. If you are using Khronos’s `opencl.hpp`, it is recommended to define the target version before including the header:
```cpp
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#include <CL/opencl.hpp>
```
This prevents the inclusion of symbols from OpenCL 2.x or 3.0, which could cause compilation errors.

---

## 8. Compilation and Testing
In your main.cpp (or another file), you can try the following simple program:
```cpp
#include <iostream>
#include <vector>

#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#include <CL/opencl.hpp>

int main() {
    std::cout << "Hello, OpenCL!" << std::endl;

    // Retrieve platform list
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if (platforms.empty()) {
        std::cout << "No OpenCL platforms found." << std::endl;
        return 1;
    }

    std::cout << "Found " << platforms.size() << " platform(s)." << std::endl;
    return 0;
}
```
1.    Select the corresponding Target in the Scheme menu.
2.    Click **Run (▶)** or choose **Product → Run**.
3.    If the code compiles successfully and the Console displays “Hello, OpenCL!” along with platform information, the setup is complete.

---

## 9. Limitations of Xcode for OpenCL Versions
- macOS officially supports only up to OpenCL 1.2.
- Khronos’s C++ wrappers (opencl.hpp, cl2.hpp) may default to OpenCL 2.0 or 3.0, so you need to manually specify `#define CL_HPP_TARGET_OPENCL_VERSION 120`.
- Without these definitions, you may encounter compilation errors such as “Use of undeclared identifier”.

---

## 10. FAQ and Additional Information
#### Q1: Why does `#include <CL/opencl.hpp>` report “file not found”?
Please check:
1. Whether the Header Search Paths include `$(PROJECT_DIR)/CL` (or the corresponding path).
2. Whether the file structure is correct, e.g., that `CL/opencl.hpp` truly exists.

#### Q2: Why does my program only use OpenCL 1.2?
The native OpenCL.framework on macOS supports only up to version 1.2 and has not been updated to 2.0 or 3.0.

---

## Conclusion
By following the steps outlined above, you should now be able to set up and run an OpenCL program in Xcode on macOS. Remember to:
- Place the `opencl.hpp` file in a designated folder (e.g., `CL`) and configure the Header Search Paths accordingly.
- Link the OpenCL.framework manually.
- Define the target OpenCL version as 120 to avoid compilation issues.

For further assistance, please refer to the [OpenCL-CLHPP](https://github.com/KhronosGroup/OpenCL-CLHPP) repository or consult the macOS developer community. Happy coding!
