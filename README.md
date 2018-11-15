# A collection of C++ libraries for image segmentation

Here we publish a set of image segmentation libraries developed for the research in a [Vision System Laboratory at IITP RAS](http://iitp.ru/en/researchlabs/281.htm). Currently this not very big collection includes the following:

* [remseg](https://github.com/Visillect/segmentation/tree/master/vi_packages/remseg) — region adgacency graph (RAG) framework 
* [colorseg](https://github.com/Visillect/segmentation/tree/master/vi_packages/colorseg) — color-based segmentation

## Getting started

### Installation

Practically, the code is not platform specific but it is tested only on Linux.
The official support is curently provided only for Linux too. 

First, install `libjsocpp-dev` package, for example, in Ubuntu run

    sudo apt install libjsoncpp-dev

To configure and compile the project run the following command:


    git clone https://github.com/Visillect/segmentation
    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make 
    
### Image data stucture manipulations

For compatility with [OpenCV](https://www.opencv.org/) image types the [vi_cvt](https://github.com/Visillect/segmentation/tree/master/vi_packages/vi_cvt) package is provided.

The convertion from OpenCV `cv::Mat` to `MinImg` could be done as

    #include <vi_oct/ocv.h>
    
    cv::Mat cv_image = ...
    ...
    MinImg min_image = vi::cvt::ocv::as_minimg(cv_image);

and to convert `MinImg` to `cv::Mat` use

    MinImage* min_image = ...
    ...
    cv::Mat cv_image = vi::cvt::ocv::as_cv_mat(min_image);
   
Also, for `MinImg` smart pointers wrappers are provided in [mximg](https://github.com/Visillect/segmentation/tree/master/vi_packages/mximg) package.
    
## Support

Please, use the GitHub issue tracker at https://github.com/Visillect/segmentation for bug reports, feature requests, etc.

## Contribution

Contributions (bug reports, bug fixes, improvements, etc.) are very welcome and should be submitted in the form of new issues and/or pull requests on GitHub.

## License

This project (except [remseg](https://github.com/Visillect/segmentation/tree/master/vi_packages/remseg) package) is licensed under the 2-Clause BSD License -- see doc/license.txt files in [vi_packages](https://github.com/Visillect/segmentation/tree/master/vi_packages) and [minsubsystem](https://github.com/Visillect/segmentation/tree/master/minsubsystem) packages.

The [remseg](https://github.com/Visillect/segmentation/tree/master/vi_packages/remseg) package is licensed under MIT license.
