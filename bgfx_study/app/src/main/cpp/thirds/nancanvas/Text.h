#ifndef TEXT_H
#define TEXT_H

class NVGcontext;

namespace NanoCanvas
{
    class Canvas;
    
    /**
     * @class Font
     * @brief Text font class
     */
    struct Font
    {
        /// The face id of the font
        int face = -1;
        
        /// The font name
        const char* name;
        
        Font() = default;
        
        /**
         * @brief Creates font by loading it from the disk from specified file path.
         * @param ctx The context
         * @param fname The name of the font
         * @param ttfPath The ttf file path to load
         */
        Font(NVGcontext* ctx, const char* fname , const char* ttfPath);
        
        /**
         * @brief Creates image by loading it from the specified memory chunk.
         * @param ctx The context
         * @param fname The name of the font
         * @param mem The memory to load from
         * @param invalidateMem Should invalidate the Memory after loading
         * @note Invalidate Memory is not release the Memory you should do that yourself!
         * @see Memory::invalidate
         */
        Font(NVGcontext* ctx, const char* fname, const Memory& mem, bool invalidateMem);
        
        /**
         * @brief Check is the font face is valid
         * @return Is the font face is valid
         */
        inline bool isValid()const{ return face >= 0; }
        ~Font(){};
    };
    
    /// The text alignment is formed by horizontal alignment and vertical alignemt
    namespace TextAlign
    {
        /// Horizontal Alignment
        enum HorizontalAlign
        {
            /// Align text horizontally to left.
            Left      = 1<<0,
            /// Align text horizontally to center.
            Center    = 1<<1,
            /// Align text horizontally to right.  
            Right     = 1<<2,
        };

        /// Vertical Alignment
        enum VerticalAlign
        {
            /// Align text vertically to top.
            Top       = 1<<3,
            /// Align text vertically to middle.
            Middle    = 1<<4,
            /// Align text vertically to bottom. 
            Bottom    = 1<<5,
            /// Align text vertically to baseline.
            Baseline  = 1<<6 
        };
    }
    
    /// Text style description structure
    /// @note The text alignment of text is formed by horizontal alignment and vertical alignemt
    struct TextStyle
    {
        /// The font face of current text style
        int face          = -1;
        /// The font size of current text style in pixel
        float size        = 12.0f;
        /// The proportional line height of current text style. 
        /// The line height is specified as multiple of font size. 
        float lineHeight  = NAN;
        /// The blur of current text style
        float blur        = NAN;
        /// The letter spacing of current text style in pixel
        float letterSpace = NAN;
        /// The color of current text style
        Color color       = Colors::ZeroColor;
        /// The horizontal alignment of current text style
        /// @see TextAlign::HorizontalAlign
        int hAlign = TextAlign::Left;
        /// The vertical alignment of current text style
        /// @see TextAlign::VerticalAlign
        int vAlign = TextAlign::Baseline;
    };
}

#endif // TEXT_H
