#ifndef BMP_PARSER_H_
#define BMP_PARSER_H_
#include <fstream>
#include <iostream>
#include <string>
#include <cstdint>


namespace BMP
{
using B2=union
{uint16_t x; char c[2];};
using B4=union
{uint32_t x; char c[4];};
using B2i=int16_t;
using B4i=int32_t;

struct __BMP_FILE_HEADER
{
B2 header_field; 
B4 file_size; 
B2 implementation_detail_a; 
B2 implementation_detail_b;
B4 pixel_data_offset;  
};
struct __BITMAPCOREHEADER
{
B4 size_of_header{12};
B2 bitmap_width;
B2 bitmap_height;
B2 num_color_panels{1};
B2 bit_per_pixel;
};
struct __BITMAPINFOHEADER
{
B4 size_of_header{40};
B4i bitmap_width;
B4i bitmap_height;
B2 num_color_panels{1};
B2 bit_per_pixel;
B4 compression_method; 
B4 image_data_size; 
B4 resolution_horizontal;
B4 resolution_vertical;
B4 num_colors_in_palett; 
B4 num_important_colors;
};

enum __HEADER_TYPE
{
    NONE, 
    CORE,
    INFO,
    OTHER
};  
__HEADER_TYPE __get_header_type(size_t size_of_header)
{
    switch(size_of_header)
    {
        case 12: return __HEADER_TYPE::CORE;
        case 40: return __HEADER_TYPE::INFO;
    }

    return __HEADER_TYPE::OTHER;
}
size_t __get_row_size(size_t bits_per_pixel,int image_width)
{
    return std::floorf((bits_per_pixel*image_width+31.f)/32.f)*4.f;
}


};

#endif