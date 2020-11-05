#include "NanoCanvas.h"
#include "nanovg/nanovg.h"

namespace NanoCanvas
{
    Font::Font(Canvas* canvas, const char* fname , const char* ttfPath)
    {
        if( canvas->valid() && strlen(fname) && strlen(ttfPath) )
        {
            face = nvgCreateFont(canvas->nvgContext(),fname,ttfPath);
        }
        name = fname;
    }
    
    Font::Font(Canvas* canvas, const char* fname ,
               const Memory& memory, bool invalidateMem)
    {
        if(canvas->valid() && memory.isValid() && strlen(fname) )
        {
            face = nvgCreateFontMem(canvas->nvgContext(),fname,
                                        (unsigned char*)memory.data,
                                        memory.size, invalidateMem);
        }
        name = fname;
    }
}

