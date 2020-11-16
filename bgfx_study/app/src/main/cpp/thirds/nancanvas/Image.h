#ifndef IMAGE_H
#define IMAGE_H

#include "bgfx/bgfx.h"

class NVGcontext;
//you should
namespace NanoCanvas
{
    class Canvas;
    
    /**
     * @class Image
     * @brief The Image class of Nano canas
     * NanoVG allows you to load jpg, png, psd, tga, pic and gif files to be used for rendering.
     */
    class Image
    {
    public:
    
        /** @brief Flags for image creation
         *  Can be used with bit operation
         */
        enum ImageFlag
        {
            /// Generate mipmaps during creation of the image.
            GenerateMipmaps     = 1<<0,
            /// Repeat image in X direction.  
            RepeatX             = 1<<1,
            /// Repeat image in Y direction.
            RepeatY             = 1<<2,
            /// Flips (inverses) image in Y direction when rendered.
            FlipY               = 1<<3,
            /// Image data has premultiplied alpha.
            PreMultiplied       = 1<<4,
        };
        
        /// Delete default constructor
        Image() = delete;
        
        /**
         * @brief Creates image by loading it from the disk from specified file name.
         * @param canvas The canvas who owns the image
         * @param filePath The image file path to load
         * @param imageFlags Creation flags
         * @see Image::ImageFlag
         */
        Image(NVGcontext* ctx,const char* filePath, int imageFlags = 0);
        
        /**
         * @brief  Creates image by loading it from the specified chunk of memory.
         * @param canvas The canvas who owns the image
         * @param memory The memory block to load from
         * @param imageFlags Creation flags
         * @see Image::ImageFlag
         */
        Image(NVGcontext* ctx, const bgfx::Memory& memory, int imageFlags = 0);
        
        /**
         * @brief Creates image with RGBA format from specified image data.
         * @param canvas The canvas who owns the image
         * @param w The width of the image
         * @param h The height of the image
         * @param memory The memory block to load from
         * @param imageFlags Creation flags
         */
        Image(NVGcontext* ctx, int w, int h, const bgfx::Memory& memory, int imageFlags=0);
        
        ~Image();
        
        /// Delete copy constructor
        Image(const Image&) = delete;
        /// Disable assignment
        Image& operator=(const Image&) = delete;
        
        /// Check is the image id is bigger than 0
        inline bool isValid()const{ return imageID != 0 ;}
        
        /// Update the image with memory data
        void update(const bgfx::Memory& memory);
        
        /**
         * @brief Get image size
         * @param width  [out] The width of the image , must be left-value
         * @param height [out] The height of the image , must be left-value
         */
        void size(int& width,int& height);
        
        /// The image id of nanovg
        int imageID = 0;
    private:
        /// The owner context
        NVGcontext * _ctx = nullptr;
    };
}

#endif // IMAGE_H
