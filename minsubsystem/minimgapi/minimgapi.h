/*
Copyright (c) 2011-2013, Smart Engines Limited. All rights reserved.

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
 * @file   minimgapi.h
 * @brief  MinImgAPI library application programming interface.
 *
 * Some of the functions also have inline versions. These versions are defined
 * in minimgapi-inl.h and have the same names as the ordinary ones preceded with
 * underscore.
 */

#pragma once
#ifndef MINIMGAPI_MINIMGAPI_H_INCLUDED
#define MINIMGAPI_MINIMGAPI_H_INCLUDED

#include <stdlib.h>
#include <minbase/crossplat.h>
#include <minbase/minimg.h>

/**
 * @mainpage Overview
 * <b>MinImgAPI</b> is an open-source platform-independent library that contains
 * image processing functions which treat the image as a matrix. That is, these
 * functions know nothing about "pixel" essence. Examples of such functions are:
 * allocation memory for image data, copying images, rotating an image by right
 * angle and others.
 *
 * For the internal representation of images is used cross-platform open-source
 * container - MinImg (see @ref MinUtils_MinImg section for more information).
 * The advantages of this container are the using a minimal number of fields
 * needed to represent the bitmap image and the easy way to cast it to other
 * standard and popular containers (for instance, Windows DIB, GDI+ BitmapData,
 * Intel/OpenCV IplImage).
 *
 * The library is written in C++ and can be compiled under Linux (GCC) and
 * Windows (MSVC 8 and later). Though the library has been written in C++, it
 * has C interface, so it can be embedded in different systems.
 */

/**
 * @page MinImgAPITutorial Quick Tutorial
 * This tutorial is intended to get you start using <b>MinImgAPI</b> library.
 * The tutorial demonstrates popular use cases of library usages, therefore it
 * is not a complete or detailed documentation. Note also, that some secondary
 * operations will be purposely omitted for brevity.
 *
 * @section MinImgAPITutorial_Alloc Allocate and Deallocate Images
 * This is the most popular use case of usage the library. To do that you should
 * define image header at first and then allocate memory for image data. The
 * following example shows the way to allocate 24-bit RGB image of 640x480 size:
 *
 * @code
 * // Define header
 * MinImg image = {0};
 * image.width = 640;
 * image.height = 480;
 * image.channels = 3;
 * image.channelDepth = 1;
 * image.format = FMT_UINT;
 *
 * // Allocates the memory for the image data
 * PROPAGATE_ERROR(AllocMinImage(&image, 16));
 * @endcode
 *
 * If you use @c AllocMinImage() for allocation of memory then you <b>must</b>
 * use @c FreeMinImage() to deallocate that. The following example demonstrates
 * the usage of @c FreeMinImage() function:
 *
 * @code
 * PROPAGATE_ERROR(FreeMinImage(&image));
 * @endcode
 *
 * @section MinImgAPITutorial_Copy Copy Images
 * Another popular use case is cloning the image. Let we have @c sourceImage and
 * want to clone it. The following code shows the proper way to do that:
 *
 * @code
 * // Define clone image
 * MinImg cloneImage = {0};
 *
 * // Make a copy of the header and allocate it
 * PROPAGATE_ERROR(CloneMinImagePrototype(&cloneImage, &sourceImage));
 *
 * // Copy image data
 * PROPAGATE_ERROR(CopyMinImage(&cloneImage, &sourceImage));
 * @endcode
 */

/**
 * @page MinImgAPILicese MinImgAPI License Agreements
 * @section MinImgAPILicese_Library Library License Agreement
 * MinImgAPI is released under FreeBSD License. It is free for both
 * academic and commercial use.
 * @include license.txt
 *
 * @section MinImgAPILicese_Documentation Documentation License Agreement
 * This documentation is released under FreeBSD Documentation License. It is
 * free for both academic and commercial use.
 * @include doc/license.txt
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def     MINIMGAPI_API
 * @brief   Specifies storage-class information (only for MSC).
 * @ingroup MinImgAPI_API
 */
#if defined _MSC_VER && defined MINIMGAPI_EXPORTS
#  define MINIMGAPI_API __declspec(dllexport)
#else
#  define MINIMGAPI_API
#endif

/**
 * @def     GET_IMAGE_LINE_BIT(p, x)
 * @brief   Returns value of x-th bit of image line pointed by p. If bit is on,
 *          returns not just 1, but returns it in the same position within byte,
 *          in which it was within the image byte.
 * @ingroup MinImgAPI_API
 */
#define GET_IMAGE_LINE_BIT(p, x) (((p)[(x) >> 3]) & (0x80U >> ((x) & 7)))

/**
 * @def     SET_IMAGE_LINE_BIT(p, x)
 * @brief   Sets x-th bit of image line pointed by p to be 1.
 * @ingroup MinImgAPI_API
 */
#define SET_IMAGE_LINE_BIT(p, x) (((p)[(x) >> 3]) |= (0x80U >> ((x) & 7)))

/**
 * @def     CLEAR_IMAGE_LINE_BIT(p, x)
 * @brief   Sets x-th bit of image line pointed by p to be 0.
 * @ingroup MinImgAPI_API
 */
#define CLEAR_IMAGE_LINE_BIT(p, x) (((p)[(x) >> 3]) &= (0xFF7FU >> ((x) & 7)))

/**
 * @def     INVERT_IMAGE_LINE_BIT(p, x)
 * @brief   Switches the value of x-th bit of image line pointed by p.
 * @ingroup MinImgAPI_API
 */
#define INVERT_IMAGE_LINE_BIT(p, x) (((p)[(x) >> 3]) ^= (0x80U >> ((x) & 7)))

/**
 * @defgroup MinImgAPI_API MinImgAPI Library API
 * @brief    This section describes an application programming interface (API)
 *           of <b>MinImgAPI</b> library. Though <b>MinImgAPI</b> has been
 *           written in C++, it has C interface to make it easy embedding the
 *           different systems.
 */

/**
 * @defgroup MinImgAPI_Utility MinImgAPI Library Utility
 * @brief    This section describes different utility functions and classes.
 */

/**
 * @brief   Specifies allocation options.
 * @details The enum specifies whether the new object should be allocated. This
 *          is used in various create- and clone-functions.
 */
typedef enum {
  AO_EMPTY,         ///< The object should stay empty (without allocation).
  AO_PREALLOCATED   ///< The object should be allocated.
} AllocationOption;

/**
 * @brief   Specifies the degree of rules validation.
 * @details The enum specifies the degree of rules validation. This can be used,
 *          for example, to choose a proper way of input arguments validation.
 */
typedef enum {
  RO_STRICT          = 0x00,  ///< Validate each rule in a proper way.
  RO_IGNORE_BORDERS  = 0x01,  ///< Skip validations of image borders.
  RO_REUSE_CONTAINER = 0x02   ///< Allow overwrite of allocated MinImg.
} RulesOption;

/**
 * @brief   Specifies border acceptable border conditions.
 * @details The enum specifies acceptable options for border condition. If a
 *          function needs pixels outside of an image, then they are
 *          reconstructed according to one the following modes (that is, fill
 *          the "image border").
 */
typedef enum {
  BO_IGNORE,      ///< Ignores the image size and allows out of memory reading.
  BO_REPEAT,      ///< The value of pixel out of the image is assumed to be
                  ///  equal to the nearest one in the image.
  BO_SYMMETRIC,   ///< Assumes that coordinate plane is periodical with
                  ///  an image as a half-period.
  BO_CYCLIC,      ///< Assumes that coordinate plane is periodical with an image
                  ///  as a period.
  BO_CONSTANT,    ///< Assumes that pixels out of image have fixed value.
  BO_VOID         ///< Assumes that pixels out of image do not exist.
} BorderOption;

/**
 * @brief   Specifies acceptable directions.
 * @details The enum specifies directions which can be used in image
 *          transformation, image filtration, calculation orientation and other
 *          functions.
 */
typedef enum {
  DO_VERTICAL,     ///< Vertical transformation.
  DO_HORIZONTAL,   ///< Horizontal transformation.
  DO_BOTH          ///< Transformation in both directions.
} DirectionOption;

/**
 * @brief   Specifies the way two images are placed in memory with respect
 *          to each other.
 * @details The enum specifies location of destination image with respect to
 *          source one, that can restrict some operations with these images.
 */
typedef enum {
  TCR_TANGLED_IMAGES         = 0x00,
  ///< Images are tangled in a complex way; one needs to copy source.
  TCR_FORWARD_PASS_POSSIBLE  = 0x01,
  ///< Every pixel of source image has address not lower than corresponding
  ///  pixel of destination image; pixel-by-pixel processing is possible.
  TCR_BACKWARD_PASS_POSSIBLE = 0x02,
  ///< Every pixel of destination image has address not lower than corresponding
  ///  pixel of source image; pixel-by-pixel processing is possible for
  ///  vertically flipped images.
  TCR_INDEPENDENT_LINES      = 0x04,
  ///< Corresponding lines of the images do not intersect in memory.
  TCR_INDEPENDENT_IMAGES     = TCR_FORWARD_PASS_POSSIBLE |
                               TCR_BACKWARD_PASS_POSSIBLE |
                               TCR_INDEPENDENT_LINES,
  ///< Images do not intersect in memory.
  TCR_SAME_IMAGE             = TCR_BACKWARD_PASS_POSSIBLE |
                               TCR_FORWARD_PASS_POSSIBLE
  ///< Images coincide pixel-to-pixel; copy-like actions require no processing,
  ///  for other cases one needs to copy source.
} TangleCheckResult;

/**
 * @brief   Makes new MinImg, allocated or not.
 * @param   p_image       The image.
 * @param   width         Width of the image.
 * @param   height        Height of the image.
 * @param   channels      Number of image channels.
 * @param   element_type  Type (MinTyp value) of the image content.
 * @param   address_space Number of the virtual device hosting the image.
 * @param   allocation    Specifies whether the image should be allocated.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function fills the image header. If @c allocation is set
 * to @c AO_PREALLOCATED (the default) then a new image will also be allocated.
 * Function fails if p_image->pScan0 is not NULL.
 */
MINIMGAPI_API int NewMinImagePrototype(
    MinImg          *p_image,
    int              width,
    int              height,
    int              channels,
    MinTyp           element_type,
    int              address_space IS_BY_DEFAULT(0),
    AllocationOption allocation    IS_BY_DEFAULT(AO_PREALLOCATED));

/**
 * @brief   Allocates an image.
 * @param   p_image   The image to be allocated.
 * @param   alignment Alignment for image rows, by default 16 bytes.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function allocates the memory for the image data. The memory block size
 * to allocate is specified by the "header fields" of the @c p_image. On success
 * the function updates @c p_image->pScan0 and @c p_image->stride fields in
 * accordance with allocated memory block.
 * Function fails if p_image->pScan0 is not NULL.
 */
MINIMGAPI_API int AllocMinImage(
    MinImg *p_image,
    int     alignment IS_BY_DEFAULT(16));

/**
 * @brief   Deallocates an image.
 * @param   p_image The image to be deallocated.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function deallocates the image data and clean @c p_image->pScan0 and
 * @c p_image->stride fields.
 */
MINIMGAPI_API int FreeMinImage(
    MinImg *p_image);

/**
 * @brief   Makes a copy of the image header.
 * @param   p_dst_image The destination image.
 * @param   p_src_image The source image.
 * @param   allocation  Specifies whether the destination image should be
 *                      allocated.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function makes a full copy of the image header. If @c allocation is set
 * to @c AO_PREALLOCATED (the default) then a new image will also be allocated.
 */
MINIMGAPI_API int CloneMinImagePrototype(
    MinImg          *p_dst_image,
    const MinImg    *p_src_image,
    AllocationOption allocation IS_BY_DEFAULT(AO_PREALLOCATED));

/**
 * @brief   Makes a copy of the transposed image header.
 * @param   p_dst_image The destination image.
 * @param   p_src_image The source image.
 * @param   allocation  Specifies whether the destination image should be
 *                      allocated.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function makes a full copy of the transposed image header (that is,
 * @c p_dst_image->width @c = @c p_src_image->height and
 * @c p_dst_image->height @c = @c p_src_image->width).
 * If @c allocation is set to @c AO_PREALLOCATED (the default) then a new image
 * will also be allocated.
 */
MINIMGAPI_API int CloneTransposedMinImagePrototype(
    MinImg          *p_dst_image,
    const MinImg    *p_src_image,
    AllocationOption allocation IS_BY_DEFAULT(AO_PREALLOCATED));

/**
 * @brief   Makes a copy of the image header with another type (MinTyp value).
 * @param   p_dst_image The destination image.
 * @param   p_src_image The source image.
 * @param   type        The required type (MinTyp value) of the destination
 *                      image.
 * @param   allocation  Specifies whether the destination image should be
 *                      allocated.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function makes a full copy of the image header with required type (MinTyp
 * value). If @c allocation is set to @c AO_PREALLOCATED (the default) then a
 * new image will also be allocated.
 */
MINIMGAPI_API int CloneRetypifiedMinImagePrototype(
    MinImg          *p_dst_image,
    const MinImg    *p_src_image,
    MinTyp           type,
    AllocationOption allocation IS_BY_DEFAULT(AO_PREALLOCATED));

/**
 * @brief   Makes a copy of the image header with another number of channels.
 * @param   p_dst_image The destination image.
 * @param   p_src_image The source image.
 * @param   channels    The required number of the destination image channels.
 * @param   allocation  Specifies whether the destination image should be
 *                      allocated.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function makes a full copy of the image header with required number of
 * channels. If @c allocation is set to @c AO_PREALLOCATED (the default) then a
 * new image will also be allocated.
 */
MINIMGAPI_API int CloneDimensionedMinImagePrototype(
    MinImg          *p_dst_image,
    const MinImg    *p_src_image,
    int              channels,
    AllocationOption allocation IS_BY_DEFAULT(AO_PREALLOCATED));

/**
 * @brief   Makes a copy of the image header with another size.
 * @param   p_dst_image The destination image.
 * @param   p_src_image The source image.
 * @param   width       The required width of the destination image.
 * @param   height      The required height of the destination image.
 * @param   allocation  Specifies whether the destination image should be
 *                      allocated.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function makes a full copy of the image header with required size. If @c
 * allocation is set to @c AO_PREALLOCATED (the default) then a new image will
 * also be allocated.
 */
MINIMGAPI_API int CloneResizedMinImagePrototype(
    MinImg          *p_dst_image,
    const MinImg    *p_src_image,
    int              width,
    int              height,
    AllocationOption allocation IS_BY_DEFAULT(AO_PREALLOCATED));

/**
 * @brief   Fills MinImg structure as pointer to the user scalar.
 * @param   p_image      Pointer to the MinImg structure.
 * @param   p_scalar     Pointer to the user scalar.
 * @param   element_type Type (MinTyp value) of channel element (scalar).
 * @param   rules        The degree of validation.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function fills MinImg structure to represent user scalar as one-channel,
 * one-pixel image. Content of the user memory is not affected. Ownership of
 * the memory buffer is not affected, so user must deallocate memory
 * same way it was allocated.
 * Function fails if p_image->pScan0 is not NULL.
 */
MINIMGAPI_API int WrapScalarWithMinImage(
    MinImg     *p_image,
    void       *p_scalar,
    MinTyp      element_type,
    RulesOption rules IS_BY_DEFAULT(RO_STRICT));

/**
 * @brief   Fills MinImg structure as pointer to the user pixel.
 * @param   p_image      Pointer to the MinImg structure.
 * @param   p_pixel      Pointer to the user pixel.
 * @param   channels     Number of pixel channels.
 * @param   element_type Type (MinTyp value) of channel element (pixel element).
 * @param   rules        The degree of validation.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function fills MinImg structure to represent user pixel
 * as one-pixel image. Content of the user memory is not affected. Ownership of
 * the memory buffer is not affected, so user must deallocate memory
 * same way it was allocated.
 * Function fails if p_image->pScan0 is not NULL.
 */
MINIMGAPI_API int WrapPixelWithMinImage(
    MinImg     *p_image,
    void       *p_pixel,
    int         channels,
    MinTyp      element_type,
    RulesOption rules IS_BY_DEFAULT(RO_STRICT));

/**
 * @brief   Fills MinImg structure as pointer to the user vector of scalars.
 * @param   p_image      Pointer to the MinImg structure.
 * @param   p_vector     Pointer to the user vector of scalars.
 * @param   size         Size of the user vector.
 * @param   direction    Direction of the vector to be in image.
 * @param   element_type Type (MinTyp value) of channel element (vector
 *                       element).
 * @param   rules        The degree of validation.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function fills MinImg structure to represent user vector of scalars
 * as one-line image (horizontal or vertical, depending on @c direction).
 * Content of the user memory is not affected. Ownership of the memory buffer
 * is not affected, so user must deallocate memory same way it was allocated.
 * Function fails if p_image->pScan0 is not NULL.
 */
MINIMGAPI_API int WrapScalarVectorWithMinImage(
    MinImg          *p_image,
    void            *p_vector,
    int              size,
    DirectionOption  direction,
    MinTyp           element_type,
    RulesOption      rules IS_BY_DEFAULT(RO_STRICT));

/**
 * @brief   Fills MinImg structure as pointer to the user vector of pixels.
 * @param   p_image      Pointer to the MinImg structure.
 * @param   p_vector     Pointer to the user vector of pixels.
 * @param   size         Size of the user vector.
 * @param   direction    Direction of the vector to be in image.
 * @param   channels     Number of pixel channels.
 * @param   element_type Type (MinTyp value) of channel element (pixel element).
 * @param   rules        The degree of validation.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function fills MinImg structure to represent user vector of pixels
 * as one-line image (horizontal or vertical, depending on @c direction).
 * Content of the user memory is not affected. Ownership of the memory buffer
 * is not affected, so user must deallocate memory same way it was allocated.
 * Function fails if p_image->pScan0 is not NULL.
 */
MINIMGAPI_API int WrapPixelVectorWithMinImage(
    MinImg          *p_image,
    void            *p_vector,
    int              size,
    DirectionOption  direction,
    int              channels,
    MinTyp           element_type,
    RulesOption      rules IS_BY_DEFAULT(RO_STRICT));

/**
 * @brief   Fills MinImg structure as pointer to the user solid memory buffer.
 * @param   p_image      Pointer to the MinImg structure.
 * @param   p_buffer     Pointer to the user buffer.
 * @param   width        Width of the image.
 * @param   height       Height of the image.
 * @param   channels     Number of image channels.
 * @param   element_type Type (MinTyp value) of the image content.
 * @param   rules        The degree of validation.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function fills MinImg structure to represent user memory buffer
 * as image. Content of the user memory is not affected. Ownership of
 * the memory buffer is not affected, so user must deallocate memory
 * same way it was allocated.
 * Function fails if p_image->pScan0 is not NULL.
 */
MINIMGAPI_API int WrapSolidBufferWithMinImage(
    MinImg     *p_image,
    void       *p_buffer,
    int         width,
    int         height,
    int         channels,
    MinTyp      element_type,
    RulesOption rules IS_BY_DEFAULT(RO_STRICT));

/**
 * @brief   Fills MinImg structure as pointer to the user memory buffer
 *          with fixed stride.
 * @param   p_image      Pointer to the MinImg structure.
 * @param   p_buffer     Pointer to the user buffer.
 * @param   width        Width of the image.
 * @param   height       Height of the image.
 * @param   channels     Number of image channels.
 * @param   element_type Type (MinTyp value) of the image content.
 * @param   stride       Stride of the image.
 * @param   rules        The degree of validation.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function fills MinImg structure to represent user memory buffer
 * as image. Content of the user memory is not affected. Ownership of
 * the memory buffer is not affected, so user must deallocate memory
 * same way it was allocated.
 * Function fails if p_image->pScan0 is not NULL.
 */
MINIMGAPI_API int WrapAlignedBufferWithMinImage(
    MinImg     *p_image,
    void       *p_buffer,
    int         width,
    int         height,
    int         channels,
    MinTyp      element_type,
    int         stride,
    RulesOption rules IS_BY_DEFAULT(RO_STRICT));

/**
 * @brief   Gets a region of an image.
 * @param   p_dst_image The destination image.
 * @param   p_src_image The source image.
 * @param   x0          The x-coordinate of the top-left corner of the region.
 * @param   y0          The y-coordinate of the top-left corner of the region.
 * @param   width       The width of the region.
 * @param   height      The height of the region.
 * @param   rules       The degree of validation.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function get a subimage from the source image. Note, that the function
 * <b>does not</b> make a copy of the specified region. Therefore, <b>it is
 * strongly forbidden</b> to call @c FreeMinImage() for the @c p_dst_image.
 */
MINIMGAPI_API int GetMinImageRegion(
    MinImg       *p_dst_image,
    const MinImg *p_src_image,
    int           x0,
    int           y0,
    int           width,
    int           height,
    RulesOption   rules IS_BY_DEFAULT(RO_STRICT));

/**
 * @brief   Flips an image in vertical without copying.
 * @param   p_dst_image  The destination image.
 * @param   p_src_image  The source image.
 * @param   rules        The degree of validation.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * This function flips the source image in vertical direction. Note, that the
 * function <b>does not</b> make a copy of the specified region. Therefore,
 * <b> it is forbidden</b> to call @c FreeMinImage() for the @c p_dst_image.
 */
MINIMGAPI_API int FlipMinImageVertically(
    MinImg       *p_dst_image,
    const MinImg *p_src_image,
    RulesOption   rules IS_BY_DEFAULT(RO_STRICT));

/**
 * @brief   Makes an image header where every pixel element is considered
 *          as a separate pixel.
 * @param   p_dst_image  The destination image.
 * @param   p_src_image  The source image.
 * @param   rules        The degree of validation.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * This function fills the image header pointed by @c p_dst_image to describe
 * the same image as @c p_src_image, but treating every pixel element
 * as a separate pixel. Note, that the function <b>does not</b> make a copy of
 * the specified region. Therefore, <b> it is forbidden</b> to call
 * @c FreeMinImage() for the @c p_dst_image.
 */
MINIMGAPI_API int UnfoldMinImageChannels(
    MinImg       *p_dst_image,
    const MinImg *p_src_image,
    RulesOption   rules IS_BY_DEFAULT(RO_STRICT));

/**
 * @brief   Takes a subset of equidistant image lines without copying.
 * @param   p_dst_image  The destination image.
 * @param   p_src_image  The source image.
 * @param   begin        Y coordinate to start with.
 * @param   period       Distance between lines.
 * @param   end          Max Y coordinate or -1.
 * @param   rules        The degree of validation.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * This function fills the image header pointed by @c p_dst_image to describe a
 * subset of equidistant lines of the source image starting with @c begin, with
 * distance @c period between them and finishing with y coordinate not greater
 * then @c end if it is non-negative, or height of the source image otherwise.
 * Note, that the function <b>does not</b> make a copy of the specified region.
 * Therefore, <b> it is forbidden</b> to call @c FreeMinImage() for the
 * @c p_dst_image.
 */
MINIMGAPI_API int SliceMinImageVertically(
    MinImg       *p_dst_image,
    const MinImg *p_src_image,
    int           begin,
    int           period,
    int           end IS_BY_DEFAULT(-1),
    RulesOption   rules IS_BY_DEFAULT(RO_STRICT));

/**
 * @brief   Unrolls solid image into one-line image without copying.
 * @param   p_dst_image  The destination image.
 * @param   p_src_image  The source image.
 * @param   rules        The degree of validation.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @remarks The source image must be solid, i.e. it's buffer must be contiguous.
 * @ingroup MinImgAPI_API
 *
 * This function fills the image header pointed by @c p_dst_image to describe
 * the same image as @c p_src_image as one horizontal line. Note, that the
 * function <b>does not</b> make a copy of the specified region. Therefore,
 * <b> it is forbidden</b> to call @c FreeMinImage() for the @c p_dst_image.
 */
MINIMGAPI_API int UnrollSolidMinImage(
    MinImg       *p_dst_image,
    const MinImg *p_src_image,
    RulesOption   rules IS_BY_DEFAULT(RO_STRICT));

/**
 * @brief   Returns format that corresponds to the given type (MinTyp value).
 * @param   typ The type (MinTyp value) to get format from (see @c #MinTyp).
 * @returns Appropriate format on success
 *          or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function returns the format that corresponds to the given type (MinTyp
 * value) (see @c #MinTyp and @c #MinFmt).
 */
MINIMGAPI_API int GetFmtByTyp(
    MinTyp typ);

/**
 * @brief   Returns channel depth that corresponds to the given type (MinTyp
 *          value).
 * @param   typ The type (MinTyp value) to get channel depth from
 *          (see @c #MinTyp).
 * @returns Appropriate channel depth on success or
 *          an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function returns the channel depth that corresponds to the given type
 * (MinTyp value) (see @c #MinTyp).
 */
MINIMGAPI_API int GetDepthByTyp(
    MinTyp typ);

/**
 * @brief   Returns type (MinTyp value) that corresponds to the given format and
 *          channel depth.
 * @param   fmt   The format of number presentation (see @c #MinFmt).
 * @param   depth The byte-depth of number presentation.
 * @returns Appropriate type (MinTyp value) on success
 *          or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function returns the type (MinTyp value) that corresponds to the given
 * format and depth (see @c #MinTyp and @c #MinFmt).
 */
MINIMGAPI_API int GetTypByFmtAndDepth(
    MinFmt fmt,
    int    depth);

/**
 * @brief   Returns type (MinTyp value) of an image channel element.
 * @param   p_image The input image.
 * @returns Appropriate image type (MinTyp value) or an error code otherwise
 *          (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function analyzes @c p_image->format and @c p_image->channelDepth fields
 * and returns the type (MinTyp value) of the input image elements
 * (see @c #MinTyp).
 */
MINIMGAPI_API int GetMinImageType(
    const MinImg *p_image);

/**
 * @brief   Assigns type (MinTyp value) to the image.
 * @param   p_image      The input image.
 * @param   element_type New image element type (MinTyp value).
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function updates @c p_image->format and @c p_image->channelDepth field
 * values according to assignable image element type (MinTyp value).
 */
MINIMGAPI_API int SetMinImageType(
    MinImg *p_image,
    MinTyp  element_type);

/**
 * @brief   Returns the amount of bits in one pixel of image.
 * @param   p_image      The image.
 * @returns amount of bits per image pixel on success or
 *          an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function analyzes @c p_image->channels and @c p_image->channelDepth
 * fields and returns the amount of bits per image pixel.
 */
MINIMGAPI_API int GetMinImageBitsPerPixel(
    const MinImg *p_image);

/**
 * @brief   Returns the amount of bytes in one line of an image.
 * @param   p_image      The image.
 * @returns amount of bytes per image line on success or
 *          an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function analyzes @c p_image->channels, @c p_image->channelDepth and
 * @c p_image->width fields and returns the amount of bytes per image line
 * (for bit images incomplete byte counts as the whole one).
 */
MINIMGAPI_API int GetMinImageBytesPerLine(
    const MinImg *p_image);

/**
 * @brief   Checks whether image prototype is valid or not.
 * @param   p_image      The image.
 * @returns @c NO_ERRORS if image prototype is valid or @c BAD_ARGS otherwise.
 * @ingroup MinImgAPI_API
 *
 * The function checks @c p_image->format and @c p_image->channelDepth to have
 * one of provided values (see @c #MinFmt and @c #MinTyp), then checks
 * @c p_image->width, @c p_image->height and @c p_image->channels fields
 * to be non-negative. Note, that the function returns @c NO_ERRORS on success,
 * which should not be wrongly interpreted as false.
 */
MINIMGAPI_API int AssureMinImagePrototypeIsValid(
    const MinImg *p_image);

/**
 * @brief   Checks whether image is valid or not.
 * @param   p_image      The image.
 * @returns @c NO_ERRORS if image is valid or @c BAD_ARGS otherwise.
 * @ingroup MinImgAPI_API
 *
 * The function checks image to have valid prototype and, if image has at least
 * one element, checks @c p_image->pScan0 to be non-zero and image lines to not
 * intersect in memory (analyzing @c p_image->stride and amount of bytes per
 * image line). Note, that the function returns @c NO_ERRORS on success,
 * which should not be wrongly interpreted as false.
 */
MINIMGAPI_API int AssureMinImageIsValid(
    const MinImg *p_image);

/**
 * @brief   Checks whether image is empty or not.
 * @param   p_image      The image.
 * @returns @c NO_ERRORS if image is valid and empty,
 *          >0 if image is valid but not empty,
 *          or @c BAD_ARGS otherwise.
 * @ingroup MinImgAPI_API
 *
 * The function checks image to be valid, then analyzes @c p_image->width,
 * @c p_image->height and @c p_image->channels fields to check if image has
 * at least one element. Note, that the function returns @c NO_ERRORS
 * on success, which should not be wrongly interpreted as false.
 */
MINIMGAPI_API int AssureMinImageIsEmpty(
    const MinImg *p_image);

/**
 * @brief   Checks whether image memory buffer is contiguous or not.
 * @param   p_image The image.
 * @returns @c NO_ERRORS if image buffer is contiguous,
 *             >0 if image buffer is not contiguous,
 *             or negative error code (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * This function checks if bits of image form solid memory chunk. Note,
 * that the function returns @c NO_ERRORS on success, which should not be
 * wrongly interpreted as false. Note that images with negative stride
 * is always treated as NOT solid due to safety reasons: even if memory
 * chunk is contiguous, it starts not from @c p_image->pScan0 address.
 */
MINIMGAPI_API int AssureMinImageIsSolid(
    const MinImg *p_image);

/**
 * @brief   Checks whether image has exactly one element (channel) or not.
 * @param   p_image      The image.
 * @returns @c NO_ERRORS if image has exactly one element,
 *             >0 if image has not exactly one element,
 *             or negative error code (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * This function analyzes @c p_image->width, @c p_image->height
 * and @c p_image->channels fields to check whether image has exactly
 * one element (one-channeled pixel) or not.
 */
MINIMGAPI_API int AssureMinImageIsScalar(
    const MinImg *p_image);

/**
 * @brief   Checks whether image has exactly one pixel or not.
 * @param   p_image      The image.
 * @returns @c NO_ERRORS if image has exactly one pixel,
 *             >0 if image has not exactly one pixel,
 *             or negative error code (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * This function analyzes @c p_image->width and @c p_image->height fields
 * to check whether image has exactly one pixel or not.
 */
MINIMGAPI_API int AssureMinImageIsPixel(
    const MinImg *p_image);

/**
 * @brief   Checks whether image fits given parameters or not.
 * @param   p_image      The image.
 * @param   element_type type (MinTyp value) of element to be checked for,
 *                       or -1 for not specified.
 *          channels     amount of channels in image pixel,
 *                       or -1 for not specified.
 *          width        width of the image, or -1 for not specified.
 *          height       height of the image, or -1 for not specified.
 * @returns @c NO_ERRORS if image fits given parameters,
 *             >0 if image doesn't fit given parameters,
 *             or negative error code (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * This function checks the image header for congruence with given parameters.
 * Parameters given as -1 are considered not restricted and are not checked.
 * Note, that the function returns @c NO_ERRORS on success, which should not be
 * wrongly interpreted as false.
 */
MINIMGAPI_API int AssureMinImageFits(
    const MinImg *p_image,
    MinTyp        element_type,
    int           channels      IS_BY_DEFAULT(-1),
    int           width         IS_BY_DEFAULT(-1),
    int           height        IS_BY_DEFAULT(-1));

/**
 * @brief   Returns a pointer to the specified image line.
 * @param   p_image  The input image.
 * @param   y        0-based line index.
 * @param   border   The border condition (see #BorderOption).
 * @param   p_canvas The line to be used if the @c border is @c BO_CONSTANT.
 * @returns A pointer to the specified line on success or NULL otherwise.
 * @ingroup MinImgAPI_API
 *
 * The function returns a pointer to the specified image line. If the @c y is
 * out of the range then the function will return the pointer in accordance with
 * the specified border condition (see #BorderOption).
 */
MINIMGAPI_API uint8_t *GetMinImageLine(
    const MinImg *p_image,
    int           y,
    BorderOption  border   IS_BY_DEFAULT(BO_VOID),
    void         *p_canvas IS_BY_DEFAULT(NULL));

/**
 * @brief   Compares headers of two images.
 * @param   p_image_a First image.
 * @param   p_image_b Second image.
 * @returns Zero if the headers are equal, a positive value if they are not or
 *          negative error code (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function just compares the header information (that is width, height,
 * number of channels, channel depth and format) of the source and destination
 * images. It does not matter whether the images are allocated or not.
 */
MINIMGAPI_API int CompareMinImagePrototypes(
    const MinImg *p_image_a,
    const MinImg *p_image_b);

/**
 * @brief   Compares sizes of two images in pixels.
 * @param   p_image_a First image.
 * @param   p_image_b Second image.
 * @returns Zero if the sizes are equal, a positive value if they are not or
 *          negative error code (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function compares width and height of the source and destination images.
 * It does not matter whether the images are allocated or not.
 */
MINIMGAPI_API int CompareMinImage2DSizes(
    const MinImg *p_image_a,
    const MinImg *p_image_b);

/**
 * @brief   Compares sizes of two images in pixel elements (channels).
 * @param   p_image_a First image.
 * @param   p_image_b Second image.
 * @returns Zero if the sizes are equal, a positive value if they are not or
 *          negative error code (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function compares width, height and number of channels of the source and
 * destination images. It does not matter whether the images are allocated
 * or not.
 */
MINIMGAPI_API int CompareMinImage3DSizes(
    const MinImg *p_image_a,
    const MinImg *p_image_b);

/**
 * @brief   Compares pixel types (MinTyp values) of two images.
 * @param   p_image_a First image.
 * @param   p_image_b Second image.
 * @returns Zero if the pixel types (MinTyp values) are the same, a positive
 *          value if they are not or negative error code (see @c MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function compares types (MinTyp values) of elements (channels) and number
 * of channels in pixels of two images. It does not matter whether the images
 * are allocated or not.
 */
MINIMGAPI_API int CompareMinImagePixels(
    const MinImg *p_image_a,
    const MinImg *p_image_b);

/**
 * @brief   Compares element (channel) types (MinTyp values) of two images.
 * @param   p_image_a First image.
 * @param   p_image_b Second image.
 * @returns Zero if the types (MinTyp values) are the same, a positive value
 *          if they are not or negative error code (see @c MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function compares types (MinTyp values) of elements (channels) of two
 * images. It does not matter whether the images are allocated or not.
 */
MINIMGAPI_API int CompareMinImageTypes(
    const MinImg *p_image_a,
    const MinImg *p_image_b);

/**
 * @brief   Compares headers and contents of two images.
 * @param   p_image_a First image.
 * @param   p_image_b Second image.
 * @returns Zero if the images are equal, a positive value if they are not or
 *          negative error code (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function compares two images to be equal in header and contents.
 * Images must be allocated.
 */
MINIMGAPI_API int CompareMinImages(
    const MinImg *p_image_a,
    const MinImg *p_image_b);

/**
 * @brief   Checks how two images are placed in memory respecting to each other.
 * @param   p_result    Variable to place result to.
 * @param   p_dst_image The image to be destination in later manipulations.
 * @param   p_src_image The image to be source in later manipulations.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function checks how two images are placed in memory with respect to each
 * other, treating one image as source for later manipulations and another one
 * as destination, describing the ways of manipulating that are possible without
 * damaging images' contents (see @c #TangleCheckResult).
 */
MINIMGAPI_API int CheckMinImagesTangle(
    uint32_t     *p_result,
    const MinImg *p_dst_image,
    const MinImg *p_src_image);

/**
 * @brief   Fills every element of an image with zero value.
 * @param   p_image    The input image.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @remarks The input image must be already allocated.
 * @ingroup MinImgAPI_API
 *
 * The function fills the whole image with zero value.
*/
MINIMGAPI_API int ZeroFillMinImage(
    const MinImg *p_image);

/**
 * @brief   Fills every line of an image cyclically with a given value.
 * @param   p_image    The input image.
 * @param   p_canvas   The pointer to the fill value.
 * @param   value_size The size of the fill value. If it is equal to zero
 *                     then pixel size is used.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @remarks The input image must be already allocated.
 * @ingroup MinImgAPI_API
 *
 * The function fills each line of the input image with repeating given value.
 *
 * The efficient way to fill 1-channel bit image with constant value is to fill
 * the byte pointed by @c p_canvas with that value and pass 1 as @c value_size.
*/
MINIMGAPI_API int FillMinImage(
    const MinImg *p_image,
    const void   *p_canvas,
    int           value_size IS_BY_DEFAULT(0));

/**
 * @brief   Copies one image to another.
 * @param   p_dst_image The destination image.
 * @param   p_src_image The source image.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @remarks The destination image must be already allocated.
 * @remarks Both source and destination images must have the same size, the same
 *          format, and the same number of channels.
 * @ingroup MinImgAPI_API
 *
 * The function copies all elements from the source image to the destination
 * one: @f[ p_dst_image(i, j) = p_src_image(i, j) @f]
*/
MINIMGAPI_API int CopyMinImage(
    const MinImg *p_dst_image,
    const MinImg *p_src_image);

/**
 * @brief   Copies fragment of one image to fragment of another.
 * @param   p_dst_image The destination image.
 * @param   p_src_image The source image.
 * @param   dst_x0      The x-coordinate of the top-left corner of the region
                        of destination image.
 * @param   dst_y0      The y-coordinate of the top-left corner of the region
                        of destination image.
 * @param   src_x0      The x-coordinate of the top-left corner of the region
                        of source image.
 * @param   src_y0      The y-coordinate of the top-left corner of the region
                        of source image.
 * @param   width       The width of the region.
 * @param   height      The height of the region.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @remarks The destination image must be already allocated.
 * @remarks Both source and destination images must have the same format
 *          and the same number of channels.
 * @ingroup MinImgAPI_API
 *
 * The function copies all elements from the region of the source image
 * to the region of the destination one.
*/
MINIMGAPI_API int CopyMinImageFragment(
    const MinImg *p_dst_image,
    const MinImg *p_src_image,
    int           dst_x0,
    int           dst_y0,
    int           src_x0,
    int           src_y0,
    int           width,
    int           height);

/**
 * @brief   Flips an image around vertical or horizontal axis.
 * @param   p_dst_image The destination image.
 * @param   p_src_image The source image
 * @param   direction   Specifies how to flip the image (see #DirectionOption).
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @remarks The destination image must be already allocated.
 * @remarks Both source and destination images must have the same size, the same
 *          format, and the same number of channels.
 * @ingroup MinImgAPI_API
 *
 * The function flips the image around vertical or horizontal axis. That is
 * @f$ p_src_image(i, j) = p_src_image(p_src_image->height - i - 1, j) @f$
 * for vertical flipping and
 * @f$ p_src_image(i, j) = p_src_image(i, p_src_image->width - j - 1) @f$
 * for horizontal flipping.
 */
MINIMGAPI_API int FlipMinImage(
    const MinImg *p_dst_image,
    const MinImg *p_src_image,
    DirectionOption direction);

/**
 * @brief   Transposes an image.
 * @param   p_dst_image The destination image.
 * @param   p_src_image The source image.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @remarks The destination image must be already allocated.
 * @remarks Both source and destination images must have the same format and
 *          the same number of channels.
 * @ingroup MinImgAPI_API
 *
 * The function transpose the source image: @f[ pDst(i, j) = pSrc(j, i) @f]
*/
MINIMGAPI_API int TransposeMinImage(
    const MinImg *p_dst_image,
    const MinImg *p_src_image);

/**
 * @brief   Rotates an image by 90 degrees (clockwise).
 * @param   p_dst_image   The destination image.
 * @param   p_src_image   The source image.
 * @param   num_rotations The multiplication factor.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @remarks The destination image must be already allocated.
 * @remarks Both source and destination images must have the same format and
 *          the same number of channels.
 * @ingroup MinImgAPI_API
 *
 * The function rotates the image clockwise by @c num_rotations * 90 degrees.
*/
MINIMGAPI_API int RotateMinImageBy90(
    const MinImg *p_dst_image,
    const MinImg *p_src_image,
    int           num_rotations);

/**
 * @brief   Copies specified channels of an image to another one.
 * @param   p_dst_image    The destination image.
 * @param   p_src_image    The source image.
 * @param   p_dst_channels 0-based destination channel indices.
 * @param   p_src_channels 0-based source channel indices.
 * @param   num_channels   The number of channels to copy.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @remarks The destination image must be already allocated.
 * @remarks Both source and destination images must have the same size and
 *          the same format.
 * @ingroup MinImgAPI_API
 *
 * The function copies the specified channels of the source image to the
 * destination one.
 */
MINIMGAPI_API int CopyMinImageChannels(
    const MinImg *p_dst_image,
    const MinImg *p_src_image,
    const int    *p_dst_channels,
    const int    *p_src_channels,
    int           num_channels);

/**
 * @brief   Interleaves pixels of the source images in the resulting image.
 * @param   p_dst_image    The destination image.
 * @param   p_p_src_images The pointers to the source images.
 * @param   num_src_images The number of source images.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function interleaves pixels of the source images in the resulting image.
 * The number of channels of a pixel in the resulting image equals therefore
 * to the sum of the number of channels over the list of source images.
 */
MINIMGAPI_API int InterleaveMinImages(
    const MinImg        *p_dst_image,
    const MinImg *const *p_p_src_images,
    int                  num_src_images);

/**
 * @brief   Deinterleaves pixels of the source image in the resulting images.
 * @param   p_p_dst_images The list of destination images.
 * @param   p_src_image    The source image.
 * @param   num_dst_images The number of destination images.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function deinterleaves pixels of the source image
 * into the resulting images. The list of destination images defines proper
 * partition of the channels of the source pixels. The destination pixels of
 * the i-th destination image contains corresponding channels of the source
 * image.
 */
MINIMGAPI_API int DeinterleaveMinImage(
    const MinImg *const *p_p_dst_images,
    const MinImg        *p_src_image,
    int                  num_dst_images);

/**
 * @brief   Changes image sample rate.
 * @param   p_dst_image The destination image.
 * @param   p_src_image The source image.
 * @param   x_phase     Horizontal phase of resampling.
 * @param   y_phase     Vertical phase of resampling.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @remarks The destination image must be already allocated.
 * @remarks Both source and destination images must have the same format and
 *          the same number of channels.
 * @ingroup MinImgAPI_API
 *
 * The function resamples an image in the sense of changing image sample rate.
 * The source image pixels are copied to destination one as whole entities,
 * with no interpolation.
 */
MINIMGAPI_API int ResampleMinImage(
    const MinImg *p_dst_image,
    const MinImg *p_src_image,
    double        x_phase IS_BY_DEFAULT(0.5),
    double        y_phase IS_BY_DEFAULT(0.5));

#ifdef __cplusplus
} // extern "C"
#endif

#endif // #ifndef MINIMGAPI_MINIMGAPI_H_INCLUDED
