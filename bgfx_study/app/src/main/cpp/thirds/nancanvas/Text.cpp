#include "Text.h"
#include "nanovg/nanovg.h"
#include "lua/SkMemory.h"

namespace NanoCanvas {

    Font::Font(NVGcontext *ctx, const char *fname, const char *ttfPath) {
        if (strlen(fname) && strlen(ttfPath)) {
            face = nvgCreateFont(ctx, fname, ttfPath);
        }
        name = fname;
    }

    Font::Font(NVGcontext *ctx, const char *fname, const SkMemory& memory, bool invalidateMem) {
        if (strlen(fname)) {
            face = nvgCreateFontMem(ctx, fname,
                                    (unsigned char *) memory.data,
                                    memory.size, invalidateMem);
        }
        name = fname;
    }
}

