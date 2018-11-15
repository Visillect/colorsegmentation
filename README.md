# A collection of C++ libraries for image segmentation

## About

## Getting started

### Installation

Practically, the code is not platform specific but it is tested only on Linux.
The official support is curently provided only for Linux too. 

To configure and compile the project run the following command:

    git clone https://github.com/Visillect/segmentation
    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make 

## Image Segmentation Libraries 

[remseg](https://github.com/Visillect/segmentation/tree/master/vi_packages/remseg) — region adgacency graph (RAG) framework 

[colorseg](https://github.com/Visillect/segmentation/tree/master/vi_packages/colorseg) — color-based segmentation

## Support

Please, use the GitHub issue tracker at https://github.com/Visillect/segmentation for bug reports, feature requests, etc.

## Contribution

Contributions (bug reports, bug fixes, improvements, etc.) are very welcome and should be submitted in the form of new issues and/or pull requests on GitHub.

## License

This project (except [remseg](https://github.com/Visillect/segmentation/tree/master/vi_packages/remseg) package) is licensed under the 2-Clause BSD License -- see doc/license.txt files in [vi_packages](https://github.com/Visillect/segmentation/tree/master/vi_packages) and [minsubsystem](https://github.com/Visillect/segmentation/tree/master/minsubsystem) packages.

The [remseg](https://github.com/Visillect/segmentation/tree/master/vi_packages/remseg) package is licensed under MIT license.
