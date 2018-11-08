/*

Copyright (c) 2011, Smart Engines Limited. All rights reserved.

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of copyright holders.

*/

/**
 * @file   minimgio.h
 * @brief  MinImgIO library application programming interface.
 */

/**
 * @mainpage Overview
 * <b>MinImgIO</b> is an open-source platform-independent library for reading and
 * writing image files. The library does not contain any implementations of
 * encode/decode algorithms. Rather than do this, it uses third party open-source
 * cross-platform libraries. The following table provides a summary of the
 * supported image formats and respective libraries:
 *
 * <table>
 *   <tr>
 *     <th>Format</th>
 *     <th>Description</th>
 *     <th>Library</th>
 *   </tr>
 *   <tr>
 *     <td>TIFF</td>
 *     <td>Tagged Image File Format</td>
 *     <td>libtiff</td>
 *   </tr>
 *   <tr>
 *     <td>JPEG</td>
 *     <td>Joint Photographic Experts Group</td>
 *     <td>libjpeg</td>
 * </table>
 *
 * For the internal representation of images is used cross-platform open-source
 * container - MinImg (see @ref MinUtils_MinImg section for more information).
 * The advantages of this container are the using a minimal number of fields
 * needed to represent the bitmap image and the easy way to cast it to other
 * standard and popular containers (for instance, Windows DIB, GDI+ BitmapData,
 * Intel/OpenCV IplImage).
 *
 * <b>MinImgIO</b> library allows write/read images to/from both file system and
 * memory block (see @ref MinImgIOTutorial_Memory for more information).
 *
 * The library is written in C++ and can be compiled under Linux (GCC) and
 * Windows (MSVC 8 and later). Though the library has been written in C++, it
 * has C interface, so it can be embedded in different systems.
 */

/**
 * @page MinImgIOTutorial Quick Tutorial
 * This tutorial is intended to get you start using <b>MinImgIO</b> library to
 * simply read and write images, therefore the tutorial is not a complete or
 * detailed documentation of the library. Note also, that some secondary
 * operations will be purposely omitted for brevity.
 *
 * @section MinImgIOTutorial_Reading Reading and Writing Images
 * Let @c szImagePath is a null-terminated string that contains the physical
 * path of the image. At the first step we should get image properties (size,
 * channel number, depth and other). The following code shows how to open the
 * image and retrieve the properties of the first page:
 *
 * @code
 * MinImg image = {0};
 * PROPAGATE_ERROR(GetMinImageFileProps(&image, szImagePath, 0));
 * @endcode
 *
 * Then we will allocate the memory for the image data. To do that we use @c
 * AllocMinImage() function from <b>MinImgAPI</b> library:
 *
 * @code
 * PROPAGATE_ERROR(AllocMinImage(&image, 16));
 * @endcode
 *
 * Now we are ready to read image data. The following code demonstrates loading
 * the first page of the image:
 *
 * @code
 * PROPAGATE_ERROR(LoadMinImage(&image, szImagePath, 0));
 * @endcode
 *
 * Now move to writing images. Let we have already some @c image object and want
 * to save it to the file @c szImagePath. The following code shows how to do that:
 *
 * @code
 * PROPAGATE_ERROR(SaveMinImage(szImagePath, &image, 0));
 * @endcode
 *
 * Note that @c SaveMinImage() function determines the proper file format based
 * on the filename extension. If you want to specify it manually, you should use
 * more comprehensive function @c SaveMinImageEx().
 *
 * @section MinImgIOTutorial_Memory Loading Images from Memory
 * Sometimes it is not practical or even possible to load an image from disk. For
 * such situations LoadMinImage() allows to read an image from memory block which
 * contains valid image format. To do this, you should use a special format of
 * filename:
 *
 * @code
 *   mem://<pointer-to-memory-block>.<size-of-memory-block>
 * @endcode
 *
 * Let @c pImageData is a pointer to the image in memory and @c imageSize is the
 * size the image. The following example demonstrates a way to generate filename
 * to the image in memory:
 *
 * @code
 *   char szImageMemPath[250] = {0};
 *   sprintf(szImageMemPath, "mem://%p.%lu", pImageData, imageSize);
 * @endcode
 */

/**
 * @page MinImgIOLicese MinImgIO License Agreements
 * @section MinImgIOLicese_Library Library License Agreement
 * <b>MinImgIO</b> is released under FreeBSD License. It is free for both
 * academic and commercial use.
 * @include license.txt
 *
 * @section MinImgIOLicese_Documentation Documentation License Agreement
 * This documentation is released under FreeBSD Documentation License. It is
 * free for both academic and commercial use.
 * @include doc/license.txt
 */

#pragma once
#ifndef MINIMGIO_MINIMGIO_H_INCLUDED
#define MINIMGIO_MINIMGIO_H_INCLUDED

#include <minbase/crossplat.h>
#include <minimgio/define.h>
#include <minbase/minimg.h>

#ifdef __cplusplus
  extern "C" {
#endif

/**
 * @defgroup MinImgIOAPI MinImgIO Library API
 * @brief    This section describes an application programming interface (API) of
 *           <b>MinImgIO</b> library. Though <b>MinImgIO</b> has been written in
 *           C++, it has C interface to make it easy embedding the library in
 *           different systems.
 */

/**
 * @brief   Specifies supported file formats.
 * @details The enum specifies all supported input/output file formats.
 * @ingroup MinImgIOAPI
 */
typedef enum
{
  IFF_UNKNOWN,  ///< Unknown file format.
  IFF_TIFF,     ///< Tagged image file format.
  IFF_JPEG,     ///< JPEG file format.
  IFF_PNG,      ///< PNG file format.
  IFF_WEBP,     ///< WebP file format.
  IFF_LST       ///< Lst file format.
} ImgFileFormat;

/**
 * @brief   Specifies supported TIFF compressions.
 * @details The enum specifies all supported TIFF compressions.
 * @ingroup MinImgIOAPI
 */
typedef enum
{
  IFC_NONE,     ///< No compression.
  IFC_LZW,      ///< Lempel-Ziv & Welch algorithm.
  IFC_DEFLATE,  ///< Deflate compression.
  IFC_PACKBITS, ///< PackBits compression (Macintosh RLE algorithm).
  IFC_JPEG,     ///< JPEG DCT compression.
  IFC_RLE,      ///< CCITT modified Huffman RLE algorithm.
  IFC_GROUP3,   ///< CCITT Group 3 fax encoding.
  IFC_GROUP4    ///< CCITT Group 4 fax encoding.
} ImgFileComp;

/**
 * @brief   Specifies additional information for an image.
 * @details The structure specifies additional information about the image such
 *          as horizontal and vertical DPI, file format and lossy quality. This
 *          is used both in input functions (to get additional information about
 *          the input image) and output functions (to specify the proper way of
 *          writing the image).
 * @ingroup MinImgIOAPI
 */
typedef struct
{
  ImgFileFormat  iff;    ///< The image file format (see #ImgFileFormat).
  ImgFileComp    comp;   ///< The image file compression (see #ImgFileComp).
  float          xDPI;   ///< The horizontal resolution, in dots-per-inch.
  float          yDPI;   ///< The vertical resolution, in dots-per-inch.
  int            qty;    ///< The resultant image quality (for JPEG compression).
} ExtImgProps;

/**
 * @brief   Detect the image file format by the magic bytes or by the name.
 * @param   pFileName The filename of the image to guest the format for.
 * @returns One of the available file format (see @c #ImgFileFormat) on success
 *           or @c #IFF_UNKNOWN if the function was unable to guess.
 * @ingroup MinImgIOAPI
 *
 * The function opens the image, reads the first bits (magic numbers) of a file
 * which uniquely identify the type of file. If the file does not exist or the
 * function cannot open the file, then it try to guess the format just using
 * the file extension. Currently the following file formats are supported:
 * @li TIFF files - *.tiff, *.tif
 * @li JPEG files - *.jpeg, *.jpg
 */
MINIMGIO_API int GuessImageFileFormat
(
  const char *pFileName
);

/**
 * @brief   Returns the number of pages.
 * @param   pFileName The filename of the image to get the page count for.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgIOAPI
 *
 * The function returns the number of pages that are in the specified image file.
 */
MINIMGIO_API int GetMinImageFilePages
(
  const char *pFileName
);

/**
 * @brief   Returns the name of the specified page.
 * @param   pPageName     The string buffer for page name.
 * @param   pageNameSize  The size of buffer @c pPageName.
 * @param   pFileName     The filename of the image to process.
 * @param   page          0-based page number.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgIOAPI
 *
 * The function opens the specified image file and generates the name of the
 * specified page. For classic images (such as TIFF, JPEG, or PNG) the page name
 * is equal to filename. For list file page name is the concrete filename in the list.
 */
MINIMGIO_API int GetMinImagePageName
(
  char       *pPageName,
  int         pageNameSize,
  const char *pFileName,
  int         page
);

/**
 * @brief   Gets basic information about an image.
 * @param   pImg      The image to be filled.
 * @param   pFileName The filename of the image to get properties for.
 * @param   page      0-based page number.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgIOAPI
 *
 * The function opens the specified image file, extracts the image information
 * and fills appropriate fields of @c pImg (@c pImg->width, @c pImg->height,
 * @c pImg->channels, @c pImg->channelDepth, and @c pImg->format). This also
 * zeros @c pImg->pScan0 and @c pImg->stride fields.
 */
MINIMGIO_API int GetMinImageFileProps
(
  MinImg     *pImg,
  const char *pFileName,
  int         page IS_BY_DEFAULT(0)
);

/**
 * @brief   Gets detailed information about an image.
 * @param   pImg      The image to be filled.
 * @param   pProps    The additional information about the image.
 * @param   pFileName The filename of the image to get properties for.
 * @param   page      0-based page number.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgIOAPI
 *
 * The function opens the specified image file, extracts the image information
 * and fills appropriate fields of @c pImg (in the same way as the function
 * GetMinImageFileProps()). Moreover, this gets some additional information
 * (such as horizontal and vertical resolutions) and stores it into pProps.
 */
MINIMGIO_API int GetMinImageFilePropsEx
(
  MinImg      *pImg,
  ExtImgProps *pProps,
  const char  *pFileName,
  int          page IS_BY_DEFAULT(0)
);

/**
 * @brief   Loads an image from a file.
 * @param   pImg      Loaded image.
 * @param   pFileName The filename of the image to load.
 * @param   page      0-based page number.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgIOAPI
 *
 * The function loads an image from the specified file. The image data are
 * placed into @c pImg which must be allocated in advance. This function
 * automatically detects the format of the image to load.
 */
MINIMGIO_API int LoadMinImage
(
  const MinImg *pImg,
  const char   *pFileName,
  int           page IS_BY_DEFAULT(0)
);

/**
 * @brief   Saves an image to a specified file.
 * @param   pFileName The name of the file to save the image.
 * @param   pImg      The image to be saved.
 * @param   page      0-based page number.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgIOAPI
 *
 * The function saves the image to the specified file. The image format is
 * chosen based on the filename extension.
 */
MINIMGIO_API int SaveMinImage
(
  const char   *pFileName,
  const MinImg *pImg,
  int           page IS_BY_DEFAULT(0)
);

/**
 * @brief   Saves an image to a specified file with specified options.
 * @param   pFileName The name of the file to save the image.
 * @param   pImg      The image to be saved.
 * @param   pProps    The specified save parameters.
 * @param   page      0-based page number.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgIOAPI
 *
 * The function saves the image to the specified file. This function also takes
 * into account the additional save parameters @c pProps, which can be used
 * to specify the image format, resolutions, and the quality. If the image
 * format is not specified by the @c pProps argument, then it will be chosen
 * based on the filename extension.
 */
MINIMGIO_API int SaveMinImageEx
(
  const char        *pFileName,
  const MinImg      *pImg,
  const ExtImgProps *pProps,
  int                page IS_BY_DEFAULT(0)
);

/**
 * @brief   Packs a grayscale image into monochrome once.
 * @param   pDst      The output 1-bit single-channel image.
 * @param   pSrc      The input 8-bit single-channel image.
 * @param   level     The threshold value.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgIOAPI
 *
 * The function converts the input grayscale (8-bit) image into monochrome
 * 1-bit one using the specified threshold value. The pixel treats as black if
 * its value is less then threshold and as white otherwise.
 */
MINIMGIO_API int PackMinImage
(
  const MinImg *pDst,
  const MinImg *pSrc,
  uint8_t       level IS_BY_DEFAULT(128)
);

/**
 * @brief   Unpacks a monochrome image into grayscale one.
 * @param   pDst      The output 8-bit single-channel image.
 * @param   pSrc      The input 1-bit single-channel image.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgIOAPI
 *
 * The function converts the input monochrome 1-bit image into grayscale
 * 8-bit one. Zero is mapped into zero and one is mapped into 255.
 */
MINIMGIO_API int UnpackMinImage
(
  const MinImg *pDst,
  const MinImg *pSrc
);


#ifdef __cplusplus
  } // extern "C"
#endif

#endif // #ifndef MINIMGIO_MINIMGIO_H_INCLUDED
