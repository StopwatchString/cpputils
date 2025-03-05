# Pitchfork Layout Template

![Linux Tests](https://github.com/stopwatchstring/pitchfork-template/actions/workflows/linux_tests.yml/badge.svg)
![Windows Tests](https://github.com/stopwatchstring/pitchfork-template/actions/workflows/windows_tests.yml/badge.svg)
![Docs](https://github.com/stopwatchstring/pitchfork-template/actions/workflows/doxygen.yml/badge.svg)

## Description

A C++ CMake project template mostly based on the [pitchfork](https://joholl.github.io/pitchfork-website/) template schema, with some opinionated additional specifications.

The goal of this template is to provide a consistent framework for generating exports, documentation, and testing around any C++ project. I created it out of frustration with managing a myriad of different methods between my own libraries. Adherence to this template asserts these facts to be true:

- Private headers, all source files, and all test files are stored adjacently within ```/src```
- Public headers which could be included in an external project are stored within ```/include```
- Namespaces are doubly represented by folder structure
- All external code and dependencies is stored in ```/external```, no exceptions. Given a project ```PROJECT```, its contents are stored under ```external/PROJECT``` such that its inclusion in the primary ```CMakeLists.txt``` is never different than ```add_subdirectory(external/PROJECT)```. If required, an 'adapter' ```CMakeLists.txt``` may be created such that the real project is stored in ```external/PROJECT/PROJECT``` while files required for making an adapter live in the folder above.
- Google Test is the test framework
- ```CMakeLists.txt``` will generate *at least* 2 targets native to this project. ```TARGET_NAME_lib```, and ```TARGET_NAME_tests```. If the project exports an executable as well, then ```TARGET_NAME``` will be a 3rd target. When linking against the primary project of this template, you will link against ```TARGET_NAME_lib```, which will include public headers.
- If an executable target is able to be generated, then its entry point is contained in ```src/main/main.cpp``` (this isolates ```main.cpp``` from the rest of the source tree in terms of relative pathing)
- All code files have documentation adhering to Doxygen codegen specifications (chosen subset yet to be determined). Build scripts are provided to generate modern Sphinx documentation. This documentation style derived from [this article](https://devblogs.microsoft.com/cppblog/clear-functional-c-documentation-with-sphinx-breathe-doxygen-cmake/).

## Usage

### Tools Needed
- [CMake](https://cmake.org/download/)
- [Doxygen](https://www.doxygen.nl/download.html)
- [Python](https://www.python.org/downloads/)
    - [Sphinx](https://www.sphinx-doc.org/en/master/usage/installation.html)
        - [Read-The-Docs Theme](https://sphinx-themes.org/sample-sites/sphinx-rtd-theme/) > pip install sphinx_rtd_theme
        - [Breathe](https://www.breathe-doc.org/) > pip install breathe

In ```CMakeLists.txt```, change ```TargetName``` and ```ProjectName``` to align with the project being created.

In ```docs/Doxyfile``` change ```PROJECT_NAME``` to align with project being created.

Any source files in ```/include``` and ```/src``` are included for the sake of example, and should be deleted.