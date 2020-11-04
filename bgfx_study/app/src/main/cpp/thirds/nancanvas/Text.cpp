#include "NanoCanvas.h"
#include "nanovg/nanovg.h"

namespace NanoCanvas
{
    Font::Font(Canvas& canvas, const string& fname , const string& ttfPath)
    {
        if( canvas.valid() && fname.length() && ttfPath.length() )
        {
            face = nvgCreateFont(canvas.nvgContext(),fname.c_str(),ttfPath.c_str());
        }
        name = fname;
    }
    
    Font::Font(Canvas& canvas, const string& fname ,
               const Memory& memory, bool invalidateMem)
    {
        if(canvas.valid() && memory.isValid() && fname.length() )
        {
            face = nvgCreateFontMem(canvas.nvgContext(),fname.c_str(),
                                        (unsigned char*)memory.data,
                                        memory.size,invalidateMem);
        }
        name = fname;
    }
}

