//
// Created by Raul Romero on 2024-01-31.
//

#ifndef ATOM_READER_H
#define ATOM_READER_H

#include <string>
#include <fstream>
#include "../Defines.h"
#include <vector>

namespace atom
{

enum class EReaderType
{
    File,
    Memory
};

enum class EReaderMode
{
    Read,
    Write,
    All,
};

enum class EReaderFormat
{
    Binary,
    Text
};

class Reader
{
private:
    EReaderType m_Type;
    EReaderMode m_Mode;
    EReaderFormat m_Format;

    char* m_Memory = nullptr;

    std::fstream m_File;
public:
    static const char* ReadTextFile(const std::string& path);
private:
    Reader(EReaderType type = EReaderType::File, EReaderMode mode = EReaderMode::Read, EReaderFormat format = EReaderFormat::Text);
    ~Reader();
    bool Open(const std::string& path);
    std::string Read(const std::string& path);
    std::string Read();
    [[maybe_unused]] static Reader& TextFileReader();
    [[maybe_unused]] static Reader& CreateBinaryFileReader();
    [[maybe_unused]] static Reader& CreateTextMemoryReader();
    [[maybe_unused]] static Reader& CreateBinaryMemoryReader();
    static const char* ReadCStr();
    [[nodiscard]] bool IsMemoryValid() const;
    void ClearMemory();
    const char* GetMemory();
    void SetMemory(const char* memory);
    void Close();
};

}

#endif //ATOM_READER_H
