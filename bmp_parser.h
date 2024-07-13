#ifndef BMP_PARSER_H_
#define BMP_PARSER_H_
#include <fstream>
#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
/*
notice:
This parser assumes that the bitmap is uncompressed.
COMPRESSED BITMAPS DO NOT WORK
*/

namespace BMP
{
union pixel
{
    
    char el[4];
    struct{
    char r; 
    char g; 
    char b; 
    char a; 
    };
    
   
};


using B2=union
{char c[2];uint16_t x; };
using B4=union
{char c[4];uint32_t x; };
using B2i=union
{char c[2];int16_t x; };
using B4i=union
{char c[4];int32_t x; };

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
B4 size_of_header;
B4i bitmap_width;
B4i bitmap_height;
B2 num_color_panels;
B2 bit_per_pixel;
};
struct BMP_DATA
{
    std::vector<pixel> pixels;
    size_t byte_per_pixel;
    int32_t width; 
    int32_t height;
};

size_t __get_row_size(size_t bits_per_pixel,int image_width)
{
    return std::floorf((bits_per_pixel*image_width+31.f)/32.f)*4.f;
}
bool __bmp_is_head(char a, char b)
{
    return (a=='B'&&b=='M')||(a=='B'&&b=='A')||(a=='C'&&b=='I')
    ||(a=='C'&&b=='P')||(a=='I'&&b=='C')||(a=='P'&&b=='T')
    ;
}

BMP_DATA parse(const char*const path)
{
    std::ifstream in{path, std::ios::binary};
    std::string line="";
    std::string data=""; 
    __BITMAPCOREHEADER core_header; 
    __BMP_FILE_HEADER file_header;
    int64_t currByte=-1; 
    BMP_DATA ret;
    #define __MAP2(L){L.at(i),L.at(i+1)} 
    #define __MAP4(L){L.at(i),L.at(i+1),L.at(i+2),L.at(i+3)}

    while(std::getline(in,line))
    {
        for(size_t i=0; i< line.size();++i)
        {
            if(i+1< line.size()&&__bmp_is_head(line.at(i),line.at(i+1)))currByte=0;
            if(currByte==0&&i+1<line.size())file_header.header_field=__MAP2(line);
            if(currByte==2&&i+3<line.size())file_header.file_size=__MAP4(line);
            if(currByte==2&&i+1<line.size())file_header.implementation_detail_a=__MAP2(line);
            if(currByte==2&&i+1<line.size())file_header.implementation_detail_b=__MAP2(line);
            if(currByte==2&&i+3<line.size())file_header.pixel_data_offset=__MAP4(line);
            if(currByte==2&&i+3<line.size())file_header.pixel_data_offset=__MAP4(line);
            if(currByte==2&&i+3<line.size())core_header.size_of_header=__MAP4(line);
            if(currByte==2&&i+3<line.size())core_header.bitmap_width=__MAP4(line);
            if(currByte==2&&i+3<line.size())core_header.bitmap_height=__MAP4(line);
            if(currByte==2&&i+1<line.size())core_header.num_color_panels=__MAP2(line);
            if(currByte==2&&i+1<line.size())core_header.bit_per_pixel=__MAP2(line);



            if(currByte>-1)currByte++;
        }
        data+=line;
        line="";
    }

    in.close();
    const size_t row_size=__get_row_size(core_header.bit_per_pixel.x,core_header.bitmap_width.x);
    const size_t row_norm=core_header.bit_per_pixel.x*core_header.bitmap_width.x/8;
    const size_t total_size=row_size*core_header.bitmap_height.x;
    ret.byte_per_pixel=core_header.bit_per_pixel.x/8; 
    ret.width=core_header.bitmap_width.x;
    ret.height=core_header.bitmap_height.x;

    for(size_t i=file_header.pixel_data_offset.x;i<total_size;++i)
    {
        if(i%(row_size)>row_norm)continue;
        pixel p={0}; 
        for(size_t j=0; j<ret.byte_per_pixel;++j)
        {
            p.el[j]=data.at(i+j);
        }
        ret.pixels.push_back(p);
        i+=ret.byte_per_pixel-1;
    }


    return ret;
}


};

#endif