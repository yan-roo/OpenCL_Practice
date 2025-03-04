# Tutorial: Setting Up and Using OpenCL in Xcode
_(Including Installation and Configuration for OpenCV C++)_
This tutorial will guide you from scratch on how to use OpenCL in an Xcode environment on macOS using the C++ wrapper (e.g., `opencl.hpp`) as an example. Additionally, it covers how to install and integrate the OpenCV C++ package. The ultimate goal is to compile and run a program that utilizes both OpenCL and OpenCV in Xcode.
---

## Table of Contents
1. [Prerequisites](#1-prerequisites)
2. [Creating an Xcode Project](#2-creating-an-xcode-project)
- [2.1. (Optional) Installing OpenCV via Homebrew](#21-optional-installing-opencv-via-homebrew)
3. [Creating or Adding a Target](#3-creating-or-adding-a-target)
4. [Adding the OpenCL C++ Wrapper (cl.hpp or opencl.hpp)](#4-adding-the-opencl-c-wrapper-clhpp-or-openclhpp)
5. [Setting Header Search Paths](#5-setting-header-search-paths)
6. [Setting Library Search Paths & Linking Binary With Libraries](#6-setting-library-search-paths-and-linking-binary-with-libraries)
7. [Setting the OpenCL Version](#7-setting-the-opencl-version)
8. [Compilation and Testing](#8-compilation-and-testing)
- [8.1. (Optional) Setting a Custom Working Directory](#81-optional-setting-a-custom-working-directory) 
9. [Limitations of Xcode for OpenCL Versions](#9-limitations-of-xcode-for-opencl-versions)
10. [FAQ and Additional Information](#10-faq-and-additional-information)

---

## 1. Prerequisites
- **macOS** system (includes OpenCL.framework, supports up to version 1.2).
- **Xcode** (a newer version is recommended for easier project and target management).
- **GitHub (optional):** To use the C++ wrapper provided by Khronos, you can obtain it from the [OpenCL-CLHPP](https://github.com/KhronosGroup/OpenCL-CLHPP) repository.
- **Homebrew (optional):** To install additional tools like `clang-format` or even OpenCV (see below).

---

## 2. Creating an Xcode Project

1. Open Xcode → `File` → `New` → `Project...`
2. In the macOS category, select **Command Line Tool**.
3. Enter a project name (e.g., `OpenCL_project`) and select C++ as the language.
4. Choose a location to save the project and click **Create**.

A basic project containing a `main.cpp` file will be generated.

### 2.1. (Optional) Installing OpenCV via Homebrew

If you plan to use OpenCV’s C++ package in your project, you can install it via Homebrew:
1. Open Terminal and update Homebrew:
```bash
brew update
```
2. Install OpenCV:
```bash
brew install opencv
```

Homebrew will automatically download and install OpenCV along with all its dependencies. Once installed, the files are stored in `/opt/homebrew/Cellar/opencv/<version>`. Homebrew also creates symlinks in `/opt/homebrew/include` and `/opt/homebrew/lib` to make it easier for compilers and linkers to find the necessary files.

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
Header Search Paths tell the compiler where to look for header files (such as `opencl.hpp` or OpenCV headers):
1.    Select your project → choose your Target → go to **Build Settings**.
2.    Search for **Header Search Paths**.
3.    Add a new path, for example:
- If your OpenCL wrapper is placed in `$(PROJECT_DIR)/CL`, add:
```
$(PROJECT_DIR)/CL
```
- If you need to include OpenCV headers from Homebrew, also add:
```
/opt/homebrew/include
```
4. If you need to support subdirectories, set the option to **recursive**.

Then in your source code, you can include the headers:
```cpp
#include "CL/opencl.hpp"
#include "opencv2/opencv.hpp"
```

---
## 6. Setting Library Search Paths & Linking Binary With Libraries
These settings ensure that during the linking phase, the linker finds the correct library files.

### Library Search Paths
1.    In your Target’s Build Settings, search for Library Search Paths.
2.    Add the following path:
```
/opt/homebrew/lib
```
This directory contains the Homebrew-managed symlinks to the actual library files (including OpenCV libraries).
### Link Binary With Libraries
1.    Select your Target → **Build Phases** → **Link Binary With Libraries**.
2.    Click the + button and add:
- OpenCL.framework (which is provided by macOS).
- For OpenCV: Click **Add Other…**, navigate to `/opt/homebrew/lib`, and select the specific OpenCV library files you require (e.g., `libopencv_core.dylib, libopencv_imgproc.dylib, etc.).

In summary:
- **Header Search Paths**: Tells the compiler where to find the header files.
- **Library Search Paths**: Informs the linker where to look for library files.
- **Link Binary With Libraries**: Specifies exactly which libraries should be linked into your final executable.

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
1. Select the corresponding Target in the Scheme menu.
2. Click **Run (▶)** or choose **Product → Run**.
3. If the code compiles successfully and the Console displays “Hello, OpenCL!” along with platform information, the setup is complete.

### 8.1. (Optional) Setting a Custom Working Directory

If you need to load external files (like `.cl` kernels) at runtime, Xcode’s default working directory may not be where your files are located. You can:
1. Go to **Product → Scheme → Edit Scheme…**.
2. In the left panel, select Run. Then click the **Options** tab on the right.
3. Enable Use **custom working directory** and set it to a folder containing your `.cl` files (e.g., `$(PROJECT_DIR)`).
4. This ensures that when your program runs, it can find files such as `simple_add.cl` using a relative path (e.g., "`simple_add.cl`" or "`Environment/simple_add.cl`").
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
By following the steps outlined above, you should be able to set up and run an OpenCL project in Xcode on macOS with additional integration of the OpenCV C++ package. Remember to:
-    Install OpenCV via Homebrew (if needed).
-    Properly configure Header and Library Search Paths in Xcode.
-    Link the required frameworks and libraries in the Link Binary With Libraries phase.
-    Define the correct OpenCL version to avoid compatibility issues.
