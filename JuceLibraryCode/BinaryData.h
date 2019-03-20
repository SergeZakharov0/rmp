/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   background_png;
    const int            background_pngSize = 12831;

    extern const char*   folder_active_png;
    const int            folder_active_pngSize = 341;

    extern const char*   folder_inactive_png;
    const int            folder_inactive_pngSize = 464;

    extern const char*   grid_png;
    const int            grid_pngSize = 8318;

    extern const char*   hyperia_png;
    const int            hyperia_pngSize = 5705;

    extern const char*   instrument_png;
    const int            instrument_pngSize = 608;

    extern const char*   knob_png;
    const int            knob_pngSize = 4470;

    extern const char*   libraryback_greyfull_png;
    const int            libraryback_greyfull_pngSize = 446;

    extern const char*   libraryback_pack_png;
    const int            libraryback_pack_pngSize = 445;

    extern const char*   librarybackground_png;
    const int            librarybackground_pngSize = 2163;

    extern const char*   librarysliderback_png;
    const int            librarysliderback_pngSize = 614;

    extern const char*   librarysliderhead_png;
    const int            librarysliderhead_pngSize = 766;

    extern const char*   offButton_png;
    const int            offButton_pngSize = 1609;

    extern const char*   onButton_png;
    const int            onButton_pngSize = 944;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 14;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
