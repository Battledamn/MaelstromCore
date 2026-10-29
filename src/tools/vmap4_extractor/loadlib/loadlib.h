/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LOAD_LIB_H
#define LOAD_LIB_H

#include <string>

#include <cstdint>
#ifndef uint64_t
#ifdef __linux__
#include <linux/types.h>
#endif
#endif
// Same fixed-width types as Define.h on every platform: MSVC's __int8 is char, not signed char,
// so the old WIN32 typedefs clashed with Define.h's int8 in files that include both.
typedef int64_t            int64;
typedef int32_t            int32;
typedef int16_t            int16;
typedef int8_t             int8;
typedef uint64_t           uint64;
typedef uint32_t           uint32;
typedef uint16_t           uint16;
typedef uint8_t            uint8;

#define FILE_FORMAT_VERSION    18

//
// File version chunk
//
struct file_MVER
{
    union
    {
        uint32 fcc;
        char   fcc_txt[4];
    };
    uint32 size;
    uint32 ver;
};

class FileLoader
{
    uint8*  data;
    uint32  data_size;
public:
    virtual bool prepareLoadedData();
    uint8* GetData()     {return data;}
    uint32 GetDataSize() {return data_size;}

    file_MVER* version;
    FileLoader();
    ~FileLoader();
    bool loadFile(std::string const& filename, bool log = true);
    virtual void free();
};
#endif
