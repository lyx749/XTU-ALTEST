#ifndef BMP_HPP
#define BMP_HPP
using ui32 = unsigned int;
using ui16 = unsigned short;
#include <vector>
#include <fstream>
#include <iostream>

#pragma pack(2)

struct BMPHeader
{
    ui16 signature;
    ui32 fileSize;
    ui32 reserved;
    ui32 dataOffset;
};

struct BMPInfoHeader
{
    ui32 headerSize;
    ui32 width;
    ui32 height;
    ui16 planes;
    ui16 bitsPerPixel;
    ui32 compression;
    ui32 imageSize;
    ui32 XPixelsPerMeter;
    ui32 YPixelsPerMeter;
    ui32 colorSpaceType;
    ui32 colorSpaceEndpoints;
};
#pragma

class readBMP
{
private:
    readBMP(){};
    static inline readBMP *interface = nullptr;

public:
    static inline readBMP *GetInterface()
    {
        if (!interface)
        {
            interface = new readBMP();
            static freeSource f;
        }
        return interface;
    }
    class freeSource
    {
    public:
        ~freeSource()
        {
            delete interface;
            interface = nullptr;
        }
    };

    std::vector<double> bmpToVector(char *filePath)
    {
        std::ifstream inFile(filePath, std::ios::binary);
        if (inFile.is_open())
        {
            BMPHeader bmpHeader;
            BMPInfoHeader bmpInfoHeader;

            inFile.read(reinterpret_cast<char *>(&bmpHeader), sizeof(bmpHeader));
            inFile.read(reinterpret_cast<char *>(&bmpInfoHeader), sizeof(bmpInfoHeader));

            // std::cout << "fileSize = " << bmpHeader.fileSize << std::endl;
            int width = bmpInfoHeader.width;
            int height = bmpInfoHeader.height;
            int imgDataSize = width * height;
            int bitCount = bmpInfoHeader.bitsPerPixel;
            int paletteInfoSize = 1 << bitCount;

            // std::cout << "width = " << width << ", height = " << height << std::endl;
            // std::cout << "bitsPerPixel = " << bmpInfoHeader.bitsPerPixel << std::endl;
            unsigned char *palette = new unsigned char[paletteInfoSize * 4];
            inFile.read(reinterpret_cast<char *>(palette), sizeof(unsigned char) * paletteInfoSize * 4);
            /*
            对于 8 位深的 BMP 图像，调色板包含 256 个颜色信息，每个颜色信息占用 4 字节，按照 BGR(A) 的顺序排列。
            对于 4 位深的 BMP 图像，调色板包含 16 个颜色信息，每个颜色信息占用 4 字节，按照 BGR(A) 的顺序排列。
            对于 1 位深的 BMP 图像，调色板包含 2 个颜色信息，每个颜色信息占用 4 字节，按照 BGR(A) 的顺序排列。
            */
            std::vector<unsigned char> imageData(width * height);

            inFile.read(reinterpret_cast<char *>(imageData.data()), sizeof(unsigned char) * width * height);

            std::vector<double> bmpVector;
            for (int i = 0; i < imgDataSize; ++i)
            {
                int position = (int)imageData[i] * 4;
                double tmp = (double)palette[position] + (double)palette[position + 1] + (double)palette[position + 2] + (double)palette[position + 3];
                // std::cout << (int)imageData[i] << " " << (double)palette[position] << " " << (double)palette[position + 1] << " " << (double)palette[position + 2] << " " << (double)palette[position + 3] << std::endl;
                bmpVector.push_back(tmp / (255.0 * 4.0));
            }

            // for (auto &e : bmpVector)
            // {
            //     std::cout << e << " ";
            // }
            imageData.clear();
            delete[] palette;
            inFile.close();
            return bmpVector;
        }
        else
        {
            std::cerr << "can't open the file" << std::endl;
            exit(1);
        }
    }
};
#endif