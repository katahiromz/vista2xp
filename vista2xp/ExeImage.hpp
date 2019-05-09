// ExeImage.hpp
////////////////////////////////////////////////////////////////////////////

#ifndef EXE_IMAGE_HPP
#define EXE_IMAGE_HPP   20      // Version 20

#ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
#endif

#if !defined(_WIN32) || (defined(_WONVER) && _WONVER == 0)
    #include "wonnt.h"          // Wonders API
#else
    #include <windows.h>
#endif
#include "pdelayload.h"         // for ImgDelayDescr

#include <vector>               // for std::vector
#include <sstream>              // for std::stringstream

#include <cstdio>               // for fopen, fclose, FILE, ...
#include <cstring>              // for memcpy
#include <cassert>              // for assert macro
#include <sys/types.h>          // for _stat
#include <sys/stat.h>           // for _stat

namespace codereverse
{

struct ImportSymbol;
struct ExportSymbol;
class ExeImage;

////////////////////////////////////////////////////////////////////////////

struct ImportSymbol
{
    DWORD               iDLL;
    DWORD               dwRVA;
    WORD                wHint;
    union
    {
        struct
        {
            WORD        wImportByName;
            WORD        wOrdinal;
        } Name;
        const char     *pszName;
    };
};

////////////////////////////////////////////////////////////////////////////

struct ExportSymbol
{
    DWORD       dwRVA;
    DWORD       dwOrdinal;
    const char *pszName;
    const char *pszForwarded;
};

////////////////////////////////////////////////////////////////////////////

class ExeImage
{
public:
    ExeImage();
    ExeImage(const char *filename);
    ExeImage(const std::vector<BYTE>& image);
    virtual ~ExeImage();

    bool load(const char *filename);
    bool save(const char *filename) const;
    void unload();

#ifdef UNICODE
    ExeImage(const wchar_t *filename);
    bool load(const wchar_t *filename);
    bool save(const wchar_t *filename) const;
#endif

    // attributes
    bool is_loaded() const;
    bool is_64bit() const;
    DWORD size_of_file() const;
    DWORD size_of_image() const;
    DWORD size_of_headers() const;
    DWORD number_of_sections() const;

    // headers
          IMAGE_DOS_HEADER        *get_dos();
          IMAGE_NT_HEADERS32      *get_nt32();
          IMAGE_NT_HEADERS64      *get_nt64();
          IMAGE_NT_HEADERS        *get_nt();
          IMAGE_OPTIONAL_HEADER32 *get_optional32();
          IMAGE_OPTIONAL_HEADER64 *get_optional64();
          IMAGE_OPTIONAL_HEADER   *get_optional();
    const IMAGE_DOS_HEADER        *get_dos() const;
    const IMAGE_NT_HEADERS32      *get_nt32() const;
    const IMAGE_NT_HEADERS64      *get_nt64() const;
    const IMAGE_NT_HEADERS        *get_nt() const;
    const IMAGE_OPTIONAL_HEADER32 *get_optional32() const;
    const IMAGE_OPTIONAL_HEADER64 *get_optional64() const;
    const IMAGE_OPTIONAL_HEADER   *get_optional() const;

    // data access
          IMAGE_DATA_DIRECTORY *get_data_dir();
    const IMAGE_DATA_DIRECTORY *get_data_dir() const;
          IMAGE_DATA_DIRECTORY *get_data_dir(DWORD dwIndex);
    const IMAGE_DATA_DIRECTORY *get_data_dir(DWORD dwIndex) const;
          BYTE *get_data(DWORD dwIndex);
    const BYTE *get_data(DWORD dwIndex) const;
          BYTE *get_data(DWORD dwIndex, DWORD& dwSize);
    const BYTE *get_data(DWORD dwIndex, DWORD& dwSize) const;

    template <typename T_STRUCT>
          T_STRUCT *get_typed_data(DWORD dwIndex);
    template <typename T_STRUCT>
    const T_STRUCT *get_typed_data(DWORD dwIndex) const;

    bool rva_in_entry(DWORD rva, DWORD index) const;

    template <typename T_STRUCT>
          T_STRUCT *map_file(DWORD offset);
    template <typename T_STRUCT>
    const T_STRUCT *map_file(DWORD offset) const;

    template <typename T_STRUCT>
          T_STRUCT *map_image(DWORD offset);
    template <typename T_STRUCT>
    const T_STRUCT *map_image(DWORD offset) const;

    // import
          IMAGE_IMPORT_DESCRIPTOR *get_import();
    const IMAGE_IMPORT_DESCRIPTOR *get_import() const;
    bool get_import_dll_names(std::vector<const char *>& names) const;
    bool get_import_symbols(DWORD dll_index, std::vector<ImportSymbol>& symbols) const;

    // export
          IMAGE_EXPORT_DIRECTORY *get_export();
    const IMAGE_EXPORT_DIRECTORY *get_export() const;
    bool get_export_symbols(std::vector<ExportSymbol>& symbols) const;

    // delay load
          ImgDelayDescr *get_delay_load();
    const ImgDelayDescr *get_delay_load() const;
    bool get_delay_load_entries(std::vector<ImgDelayDescr>& entries) const;

    // resource
          IMAGE_RESOURCE_DIRECTORY *get_resource();
    const IMAGE_RESOURCE_DIRECTORY *get_resource() const;

    // dumping
    void dump_all(std::stringstream& ss) const;
    void dump_dos(std::stringstream& ss) const;
    void dump_nt(std::stringstream& ss) const;
    void dump_optional(std::stringstream& ss) const;
    void dump_data_dir(std::stringstream& ss) const;
    void dump_section_table(std::stringstream& ss) const;
    void dump_import(std::stringstream& ss) const;
    void dump_export(std::stringstream& ss) const;
    void dump_delay_load(std::stringstream& ss) const;

    bool do_map();
    bool do_reverse_map();

protected:
    bool              m_is_64bit;
    std::vector<BYTE> m_file_image;
    std::vector<BYTE> m_loaded_image;
    IMAGE_DOS_HEADER *m_dos;
    IMAGE_NT_HEADERS *m_nt;
    IMAGE_FILE_HEADER *m_file;
    IMAGE_SECTION_HEADER *m_section_table;
    IMAGE_DATA_DIRECTORY *m_data_dir;

    inline const ExeImage *const_this()
    {
        return this;
    }

    template <typename T_TYPE>
    inline T_TYPE *drop_const(const T_TYPE *obj)
    {
        return const_cast<T_TYPE *>(obj);
    }

    void _get_import_symbols32(const IMAGE_IMPORT_DESCRIPTOR *desc, std::vector<ImportSymbol>& symbols) const;
    void _get_import_symbols64(const IMAGE_IMPORT_DESCRIPTOR *desc, std::vector<ImportSymbol>& symbols) const;
};

////////////////////////////////////////////////////////////////////////////
// inlines

inline ExeImage::ExeImage() :
    m_is_64bit(false), m_dos(NULL), m_nt(NULL), m_file(NULL),
    m_section_table(NULL), m_data_dir(NULL)
{
}

inline ExeImage::ExeImage(const char *filename) :
    m_is_64bit(false), m_dos(NULL), m_nt(NULL), m_file(NULL),
    m_section_table(NULL), m_data_dir(NULL)
{
    load(filename);
}

inline void ExeImage::unload()
{
    m_is_64bit = false;
    m_dos = NULL;
    m_nt = NULL;
    m_file = NULL;
    m_section_table = NULL;
    m_data_dir = NULL;
    m_file_image.clear();
    m_loaded_image.clear();
}

inline bool ExeImage::load(const char *filename)
{
    unload();

    using namespace std;
    struct _stat st;
    if (_stat(filename, &st) != 0)
        return false;

    bool ok = false;
    if (FILE *fp = fopen(filename, "rb"))
    {
        m_file_image.resize(st.st_size);
        if (fread(&m_file_image[0], st.st_size, 1, fp))
        {
            ok = true;
        }
        fclose(fp);
    }

    if (ok)
    {
        ok = do_map();
    }
    return ok;
}

inline bool ExeImage::save(const char *filename) const
{
    DWORD size = size_of_file();
    const BYTE *contents = map_file<BYTE>(0);

    using namespace std;
    FILE *fp = fopen(filename, "wb");
    if (fp)
    {
        if (!fwrite(contents, size, 1, fp))
        {
            fclose(fp);
            remove(filename);
            return false;
        }
        fclose(fp);
        return true;
    }
    return false;
}

#ifdef UNICODE
    inline ExeImage::ExeImage(const wchar_t *filename) :
        m_is_64bit(false), m_dos(NULL), m_nt(NULL), m_file(NULL),
        m_section_table(NULL), m_data_dir(NULL)
    {
        load(filename);
    }

    inline bool ExeImage::load(const wchar_t *filename)
    {
        unload();

        using namespace std;
        struct _stat st;
        if (_wstat(filename, &st) != 0)
            return false;

        bool ok = false;
        if (FILE *fp = _wfopen(filename, L"rb"))
        {
            m_file_image.resize(st.st_size);
            if (fread(&m_file_image[0], st.st_size, 1, fp))
            {
                ok = true;
            }
            fclose(fp);
        }

        if (ok)
        {
            ok = do_map();
        }
        return ok;
    }

    inline bool ExeImage::save(const wchar_t *filename) const
    {
        DWORD size = size_of_file();
        const BYTE *contents = map_file<BYTE>(0);

        using namespace std;
        FILE *fp = _wfopen(filename, L"wb");
        if (fp)
        {
            if (!fwrite(contents, size, 1, fp))
            {
                fclose(fp);
                _wremove(filename);
                return false;
            }
            fclose(fp);
            return true;
        }
        return false;
    }
#endif  // def UNICODE

inline ExeImage::~ExeImage()
{
}

inline bool ExeImage::do_map()
{
    IMAGE_DOS_HEADER *dos = map_file<IMAGE_DOS_HEADER>(0);
    if (dos && dos->e_magic == IMAGE_DOS_SIGNATURE && dos->e_lfanew != 0)
    {
        m_dos = dos;
    }

    DWORD offset = dos ? dos->e_lfanew : 0;
    IMAGE_NT_HEADERS32 *nt32 = map_file<IMAGE_NT_HEADERS32>(offset);
    if (nt32 && nt32->Signature == IMAGE_NT_SIGNATURE)
    {
        m_nt = reinterpret_cast<IMAGE_NT_HEADERS *>(nt32);
        m_file = &m_nt->FileHeader;
    }
    else
    {
        return false;
    }

    if (IMAGE_NT_HEADERS64 *nt64 = get_nt64())
    {
        m_is_64bit = true;
        m_section_table = reinterpret_cast<IMAGE_SECTION_HEADER *>(nt64 + 1);
        m_data_dir = nt64->OptionalHeader.DataDirectory;
    }
    else
    {
        m_is_64bit = false;
        m_section_table = reinterpret_cast<IMAGE_SECTION_HEADER *>(nt32 + 1);
        m_data_dir = nt32->OptionalHeader.DataDirectory;
    }

    DWORD cbImage = size_of_image();
    DWORD cbHeaders = size_of_headers();
    m_loaded_image.resize(cbImage, 0);

    memcpy(&m_loaded_image[0], &m_file_image[0], cbHeaders);

    for (DWORD i = 0; i < m_file->NumberOfSections; ++i)
    {
        IMAGE_SECTION_HEADER *entry = &m_section_table[i];
        if (entry->PointerToRawData)
        {
            memcpy(&m_loaded_image[entry->VirtualAddress],
                   &m_file_image[entry->PointerToRawData],
                   entry->SizeOfRawData);
        }
    }

    return true;
}

inline bool ExeImage::do_reverse_map()
{
    if (!is_loaded())
        return false;

    for (DWORD i = 0; i < m_file->NumberOfSections; ++i)
    {
        IMAGE_SECTION_HEADER *entry = &m_section_table[i];
        if (entry->PointerToRawData)
        {
            memcpy(&m_file_image[entry->PointerToRawData],
                   &m_loaded_image[entry->VirtualAddress],
                   entry->SizeOfRawData);
        }
    }

    return true;
}

inline bool ExeImage::is_64bit() const
{
    return m_is_64bit;
}

inline const IMAGE_DOS_HEADER *ExeImage::get_dos() const
{
    if (m_dos && m_dos->e_magic == IMAGE_DOS_SIGNATURE && m_dos->e_lfanew != 0)
        return m_dos;
    return NULL;
}

inline const IMAGE_NT_HEADERS32 *ExeImage::get_nt32() const
{
    if (m_file && m_file->SizeOfOptionalHeader == sizeof(IMAGE_OPTIONAL_HEADER32))
        return reinterpret_cast<IMAGE_NT_HEADERS32 *>(m_nt);
    return NULL;
}

inline const IMAGE_NT_HEADERS64 *ExeImage::get_nt64() const
{
    if (m_file && m_file->SizeOfOptionalHeader == sizeof(IMAGE_OPTIONAL_HEADER64))
        return reinterpret_cast<IMAGE_NT_HEADERS64 *>(m_nt);
    return NULL;
}

inline const IMAGE_NT_HEADERS *ExeImage::get_nt() const
{
    if (m_file && m_file->SizeOfOptionalHeader == sizeof(IMAGE_OPTIONAL_HEADER))
        return m_nt;
    return NULL;
}

inline const IMAGE_OPTIONAL_HEADER32 *ExeImage::get_optional32() const
{
    const IMAGE_NT_HEADERS32 *nt32 = get_nt32();
    if (nt32)
        return &nt32->OptionalHeader;
    return NULL;
}

inline const IMAGE_OPTIONAL_HEADER64 *ExeImage::get_optional64() const
{
    if (const IMAGE_NT_HEADERS64 *nt64 = get_nt64())
        return &nt64->OptionalHeader;
    return NULL;
}

inline const IMAGE_OPTIONAL_HEADER *ExeImage::get_optional() const
{
    if (const IMAGE_NT_HEADERS *nt = get_nt())
        return &nt->OptionalHeader;
    return NULL;
}

inline DWORD ExeImage::size_of_headers() const
{
    if (is_64bit())
        return get_optional64()->SizeOfHeaders;
    else
        return get_optional32()->SizeOfHeaders;
}

inline DWORD ExeImage::size_of_file() const
{
    return DWORD(m_file_image.size());
}

inline DWORD ExeImage::size_of_image() const
{
    if (is_64bit())
        return get_optional64()->SizeOfImage;
    else
        return get_optional32()->SizeOfImage;
}

inline bool ExeImage::is_loaded() const
{
    return m_nt != NULL;
}

inline DWORD ExeImage::number_of_sections() const
{
    return m_file->NumberOfSections;
}

inline const IMAGE_DATA_DIRECTORY *ExeImage::get_data_dir() const
{
    return (is_loaded() ? m_data_dir : NULL);
}

inline const IMAGE_DATA_DIRECTORY *ExeImage::get_data_dir(DWORD dwIndex) const
{
    if (is_loaded() && m_data_dir)
    {
        if (dwIndex < IMAGE_NUMBEROF_DIRECTORY_ENTRIES)
            return &m_data_dir[dwIndex];
    }
    return NULL;
}

inline const BYTE *ExeImage::get_data(DWORD dwIndex) const
{
    if (const IMAGE_DATA_DIRECTORY *dir = get_data_dir(dwIndex))
    {
        if (dir->VirtualAddress && dir->Size)
        {
            return map_image<BYTE>(dir->VirtualAddress);
        }
    }
    return NULL;
}

inline const BYTE *ExeImage::get_data(DWORD dwIndex, DWORD& dwSize) const
{
    if (const IMAGE_DATA_DIRECTORY *dir = get_data_dir(dwIndex))
    {
        if (dir->VirtualAddress && dir->Size)
        {
            dwSize = dir->Size;
            return map_image<BYTE>(dir->VirtualAddress);
        }
    }
    dwSize = 0;
    return NULL;
}

template <typename T_STRUCT>
inline const T_STRUCT *ExeImage::get_typed_data(DWORD dwIndex) const
{
    if (const IMAGE_DATA_DIRECTORY *dir = get_data_dir(dwIndex))
    {
        if (dir->VirtualAddress && dir->Size)
        {
            return map_image<T_STRUCT>(dir->VirtualAddress);
        }
    }
    return NULL;
}

inline IMAGE_IMPORT_DESCRIPTOR *ExeImage::get_import()
{
    return get_typed_data<IMAGE_IMPORT_DESCRIPTOR>(IMAGE_DIRECTORY_ENTRY_IMPORT);
}
inline const IMAGE_IMPORT_DESCRIPTOR *ExeImage::get_import() const
{
    return get_typed_data<IMAGE_IMPORT_DESCRIPTOR>(IMAGE_DIRECTORY_ENTRY_IMPORT);
}

inline bool ExeImage::get_import_dll_names(std::vector<const char *>& names) const
{
    const IMAGE_IMPORT_DESCRIPTOR *pImpDesc = get_import();
    if (!pImpDesc || !pImpDesc->OriginalFirstThunk)
        return false;

    for (; pImpDesc->FirstThunk != 0; ++pImpDesc)
    {
        names.push_back(map_image<char>(pImpDesc->Name));
    }
    return true;
}

inline bool ExeImage::get_import_symbols(DWORD dll_index, std::vector<ImportSymbol>& symbols) const
{
    const IMAGE_IMPORT_DESCRIPTOR *pImpDesc = get_import();
    if (!pImpDesc || !pImpDesc->OriginalFirstThunk)
        return false;

    if (is_64bit())
    {
        _get_import_symbols64(&pImpDesc[dll_index], symbols);
    }
    else
    {
        _get_import_symbols32(&pImpDesc[dll_index], symbols);
    }
    return true;
}

inline void
ExeImage::_get_import_symbols32(const IMAGE_IMPORT_DESCRIPTOR *desc,
                                std::vector<ImportSymbol>& symbols) const
{
    const IMAGE_IMPORT_BY_NAME *pIBN;
    const DWORD *pIAT;      // import address table (IAT)
    const DWORD *pINT;      // import name table (INT)

    pIAT = reinterpret_cast<const DWORD *>(static_cast<DWORD_PTR>(desc->FirstThunk));
    if (desc->OriginalFirstThunk)
        pINT = reinterpret_cast<const DWORD *>(map_image<BYTE>(desc->OriginalFirstThunk));
    else
        pINT = pIAT;

    for (DWORD k = 0; pINT[k] != 0; ++k)
    {
        if (pINT[k] >= size_of_image())
            continue;

        ImportSymbol symbol;
        symbol.dwRVA = desc->FirstThunk + k * sizeof(DWORD);

        if (IMAGE_SNAP_BY_ORDINAL32(pINT[k]))
        {
            symbol.wHint = 0;
            symbol.Name.wImportByName = 0;
            symbol.Name.wOrdinal = WORD(IMAGE_ORDINAL32(pINT[k]));
        }
        else
        {
            pIBN = map_image<IMAGE_IMPORT_BY_NAME>(pINT[k]);
            symbol.wHint = pIBN->Hint;
            symbol.pszName = reinterpret_cast<const char *>(pIBN->Name);
        }

        symbols.push_back(symbol);
    }
}

inline void
ExeImage::_get_import_symbols64(const IMAGE_IMPORT_DESCRIPTOR *desc,
                                std::vector<ImportSymbol>& symbols) const
{
    const IMAGE_IMPORT_BY_NAME *pIBN;
    const ULONGLONG *pIAT64;    // import address table (IAT)
    const ULONGLONG *pINT64;    // import name table (INT)

    pIAT64 = reinterpret_cast<const ULONGLONG *>(static_cast<DWORD_PTR>(desc->FirstThunk));
    if (desc->OriginalFirstThunk)
        pINT64 = map_image<ULONGLONG>(desc->OriginalFirstThunk);
    else
        pINT64 = pIAT64;

    for (DWORD k = 0; pINT64[k] != 0; ++k)
    {
        if (pINT64[k] >= size_of_image())
            continue;

        ImportSymbol symbol;
        symbol.dwRVA = desc->FirstThunk + k * sizeof(DWORD);

        if (IMAGE_SNAP_BY_ORDINAL64(pINT64[k]))
        {
            symbol.wHint = 0;
            symbol.Name.wImportByName = 0;
            symbol.Name.wOrdinal = WORD(IMAGE_ORDINAL64(pINT64[k]));
        }
        else
        {
            pIBN = map_image<IMAGE_IMPORT_BY_NAME>(DWORD(pINT64[k]));
            symbol.wHint = pIBN->Hint;
            symbol.pszName = reinterpret_cast<const char *>(pIBN->Name);
        }

        symbols.push_back(symbol);
    }
}

inline IMAGE_EXPORT_DIRECTORY *ExeImage::get_export()
{
    return get_typed_data<IMAGE_EXPORT_DIRECTORY>(IMAGE_DIRECTORY_ENTRY_EXPORT);
}
inline const IMAGE_EXPORT_DIRECTORY *ExeImage::get_export() const
{
    return get_typed_data<IMAGE_EXPORT_DIRECTORY>(IMAGE_DIRECTORY_ENTRY_EXPORT);
}

inline bool ExeImage::get_export_symbols(std::vector<ExportSymbol>& symbols) const
{
    const IMAGE_EXPORT_DIRECTORY *dir = get_export();
    if (!dir)
        return false;

    // export address table (EAT)
    const DWORD *pEAT = map_image<DWORD>(dir->AddressOfFunctions);
    // export name pointer table (ENPT)
    const DWORD *pENPT = map_image<DWORD>(dir->AddressOfNames);
    // export ordinal table (EOT)
    const WORD *pEOT = map_image<WORD>(dir->AddressOfNameOrdinals);

    DWORD i, k;
    WORD wOrdinal;
    for (i = 0; i < dir->NumberOfNames; ++i)
    {
        wOrdinal = pEOT[i];

        ExportSymbol symbol;
        symbol.dwRVA = pEAT[wOrdinal];
        symbol.pszName = map_image<char>(pENPT[i]);
        symbol.dwOrdinal = dir->Base + wOrdinal;
        symbol.pszForwarded = NULL;
        symbols.push_back(symbol);
    }

    for (i = 0; i < dir->NumberOfFunctions; ++i)
    {
        for (k = 0; k < dir->NumberOfNames; ++k)
        {
            if (static_cast<DWORD>(pEOT[k]) == i)
                break;
        }
        if (k < dir->NumberOfNames)
            continue;

        DWORD dw = pEAT[i];
        if (dw == 0)
            continue;

        ExportSymbol symbol;
        symbol.pszName = NULL;
        if (rva_in_entry(dw, IMAGE_DIRECTORY_ENTRY_EXPORT))
        {
            symbol.dwRVA = 0;
            symbol.pszForwarded = map_image<char>(dw);
        }
        else
        {
            symbol.dwRVA = dw;
            symbol.pszForwarded = NULL;
        }
        symbol.dwOrdinal = dir->Base + i;
        symbols.push_back(symbol);
    }

    return true;
}

inline ImgDelayDescr *ExeImage::get_delay_load()
{
    return get_typed_data<ImgDelayDescr>(IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);
}

inline const ImgDelayDescr *ExeImage::get_delay_load() const
{
    return get_typed_data<ImgDelayDescr>(IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);
}

inline bool ExeImage::get_delay_load_entries(std::vector<ImgDelayDescr>& entries) const
{
    const ImgDelayDescr *descr = get_delay_load();
    if (!descr || !descr->rvaHmod)
        return false;

    for (size_t i = 0; descr[i].rvaHmod; ++i)
    {
        entries.push_back(descr[i]);
    }
    return true;
}

inline IMAGE_RESOURCE_DIRECTORY *ExeImage::get_resource()
{
    return get_typed_data<IMAGE_RESOURCE_DIRECTORY>(IMAGE_DIRECTORY_ENTRY_RESOURCE);
}
inline const IMAGE_RESOURCE_DIRECTORY *ExeImage::get_resource() const
{
    return get_typed_data<IMAGE_RESOURCE_DIRECTORY>(IMAGE_DIRECTORY_ENTRY_RESOURCE);
}

inline bool ExeImage::rva_in_entry(DWORD rva, DWORD index) const
{
    if (const IMAGE_DATA_DIRECTORY *dir = get_data_dir(index))
    {
        return (dir->VirtualAddress <= rva &&
                rva < dir->VirtualAddress + dir->Size);
    }
    return false;
}

template <typename T_STRUCT>
inline const T_STRUCT *ExeImage::map_image(DWORD offset) const
{
    if (m_loaded_image.size() < offset + sizeof(T_STRUCT))
        return NULL;
    const BYTE *pb = reinterpret_cast<const BYTE *>(&m_loaded_image[0]) + offset;
    return reinterpret_cast<const T_STRUCT *>(pb);
}

template <typename T_STRUCT>
inline const T_STRUCT *ExeImage::map_file(DWORD offset) const
{
    if (m_file_image.size() < offset + sizeof(T_STRUCT))
        return NULL;
    const BYTE *pb = reinterpret_cast<const BYTE *>(&m_file_image[0]) + offset;
    return reinterpret_cast<const T_STRUCT *>(pb);
}

inline IMAGE_DOS_HEADER *ExeImage::get_dos()
{
    return drop_const(const_this()->get_dos());
}
inline IMAGE_NT_HEADERS32 *ExeImage::get_nt32()
{
    return drop_const(const_this()->get_nt32());
}
inline IMAGE_NT_HEADERS64 *ExeImage::get_nt64()
{
    return drop_const(const_this()->get_nt64());
}
inline IMAGE_NT_HEADERS *ExeImage::get_nt()
{
    return drop_const(const_this()->get_nt());
}
inline IMAGE_OPTIONAL_HEADER32 *ExeImage::get_optional32()
{
    return drop_const(const_this()->get_optional32());
}
inline IMAGE_OPTIONAL_HEADER64 *ExeImage::get_optional64()
{
    return drop_const(const_this()->get_optional64());
}
inline IMAGE_OPTIONAL_HEADER *ExeImage::get_optional()
{
    return drop_const(const_this()->get_optional());
}
inline IMAGE_DATA_DIRECTORY *ExeImage::get_data_dir()
{
    return drop_const(const_this()->get_data_dir());
}
inline IMAGE_DATA_DIRECTORY *ExeImage::get_data_dir(DWORD dwIndex)
{
    return drop_const(const_this()->get_data_dir(dwIndex));
}
inline BYTE *ExeImage::get_data(DWORD dwIndex)
{
    return drop_const(const_this()->get_data(dwIndex));
}
inline BYTE *ExeImage::get_data(DWORD dwIndex, DWORD& dwSize)
{
    return drop_const(const_this()->get_data(dwIndex, dwSize));
}
template <typename T_STRUCT>
inline T_STRUCT *ExeImage::get_typed_data(DWORD dwIndex)
{
    return drop_const(const_this()->get_typed_data<T_STRUCT>(dwIndex));
}
template <typename T_STRUCT>
inline T_STRUCT *ExeImage::map_image(DWORD offset)
{
    return drop_const(const_this()->map_image<T_STRUCT>(offset));
}
template <typename T_STRUCT>
inline T_STRUCT *ExeImage::map_file(DWORD offset)
{
    return drop_const(const_this()->map_file<T_STRUCT>(offset));
}

////////////////////////////////////////////////////////////////////////////
// dumping

#define EXE_IMAGE_DUMP(ss,name,parent) ss << #name ": " << parent->name << "\n"

inline void ExeImage::dump_dos(std::stringstream& ss) const
{
    ss << "\n### DOS Header ###\n";

    if (!m_dos)
    {
        ss << "No DOS header.\n";
        return;
    }

    EXE_IMAGE_DUMP(ss, e_magic, m_dos);
    EXE_IMAGE_DUMP(ss, e_cblp, m_dos);
    EXE_IMAGE_DUMP(ss, e_cp, m_dos);
    EXE_IMAGE_DUMP(ss, e_crlc, m_dos);
    EXE_IMAGE_DUMP(ss, e_cparhdr, m_dos);
    EXE_IMAGE_DUMP(ss, e_minalloc, m_dos);
    EXE_IMAGE_DUMP(ss, e_maxalloc, m_dos);
    EXE_IMAGE_DUMP(ss, e_ss, m_dos);
    EXE_IMAGE_DUMP(ss, e_sp, m_dos);
    EXE_IMAGE_DUMP(ss, e_csum, m_dos);
    EXE_IMAGE_DUMP(ss, e_ip, m_dos);
    EXE_IMAGE_DUMP(ss, e_cs, m_dos);
    EXE_IMAGE_DUMP(ss, e_lfarlc, m_dos);
    EXE_IMAGE_DUMP(ss, e_ovno, m_dos);
    EXE_IMAGE_DUMP(ss, e_oemid, m_dos);
    EXE_IMAGE_DUMP(ss, e_oeminfo, m_dos);
    EXE_IMAGE_DUMP(ss, e_lfanew, m_dos);
}

inline void ExeImage::dump_nt(std::stringstream& ss) const
{
    ss << "\n### NT Header ###\n";

    if (!m_nt)
    {
        ss << "Invalid NT header.\n";
        return;
    }

    if (is_64bit())
    {
        ss << "NT Header is 64-bit.\n";
        const IMAGE_NT_HEADERS64 *nt64 = get_nt64();

        EXE_IMAGE_DUMP(ss, Signature, nt64);
        EXE_IMAGE_DUMP(ss, Machine, m_file);
        EXE_IMAGE_DUMP(ss, NumberOfSections, m_file);
        EXE_IMAGE_DUMP(ss, TimeDateStamp, m_file);
        EXE_IMAGE_DUMP(ss, PointerToSymbolTable, m_file);
        EXE_IMAGE_DUMP(ss, NumberOfSymbols, m_file);
        EXE_IMAGE_DUMP(ss, SizeOfOptionalHeader, m_file);
        EXE_IMAGE_DUMP(ss, Characteristics, m_file);

        dump_optional(ss);
    }
    else
    {
        ss << "NT Header is 32-bit.\n";
        const IMAGE_NT_HEADERS32 *nt32 = get_nt32();

        EXE_IMAGE_DUMP(ss, Signature, nt32);
        EXE_IMAGE_DUMP(ss, Machine, m_file);
        EXE_IMAGE_DUMP(ss, NumberOfSections, m_file);
        EXE_IMAGE_DUMP(ss, TimeDateStamp, m_file);
        EXE_IMAGE_DUMP(ss, PointerToSymbolTable, m_file);
        EXE_IMAGE_DUMP(ss, NumberOfSymbols, m_file);
        EXE_IMAGE_DUMP(ss, SizeOfOptionalHeader, m_file);
        EXE_IMAGE_DUMP(ss, Characteristics, m_file);

        dump_optional(ss);
    }
}

inline void ExeImage::dump_section_table(std::stringstream& ss) const
{
    ss << "\n### Section Table ###\n";

    if (!m_section_table)
    {
        ss << "Invalid section table.\n";
        return;
    }

    for (DWORD i = 0; i < number_of_sections(); ++i)
    {
        ss << "\nSection #" << i << ": ";
        IMAGE_SECTION_HEADER *header = &m_section_table[i];
        for (int k = 0; k < 8 && header->Name[k]; ++k)
        {
            ss << header->Name[k];
        }
        ss << "\n";

        EXE_IMAGE_DUMP(ss, VirtualSize, (&header->Misc));
        EXE_IMAGE_DUMP(ss, VirtualAddress, header);
        EXE_IMAGE_DUMP(ss, SizeOfRawData, header);
        EXE_IMAGE_DUMP(ss, PointerToRawData, header);
        EXE_IMAGE_DUMP(ss, PointerToRelocations, header);
        EXE_IMAGE_DUMP(ss, PointerToLinenumbers, header);
        EXE_IMAGE_DUMP(ss, NumberOfRelocations, header);
        EXE_IMAGE_DUMP(ss, NumberOfLinenumbers, header);
        EXE_IMAGE_DUMP(ss, Characteristics, header);
    }
}

inline void ExeImage::dump_optional(std::stringstream& ss) const
{
    ss << "\n### Optional Header ###\n";

    if (is_64bit())
    {
        const IMAGE_OPTIONAL_HEADER64 *optional64 = get_optional64();
        if (!optional64)
        {
            ss << "Invalid NT header.\n";
            return;
        }
        ss << "Optional Header is 64-bit.\n";

        EXE_IMAGE_DUMP(ss, Magic, optional64);
        ss << "MajorLinkerVersion: " << (UINT)optional64->MajorLinkerVersion << "\n";
        ss << "MinorLinkerVersion: " << (UINT)optional64->MinorLinkerVersion << "\n";
        EXE_IMAGE_DUMP(ss, SizeOfCode, optional64);
        EXE_IMAGE_DUMP(ss, SizeOfInitializedData, optional64);
        EXE_IMAGE_DUMP(ss, SizeOfUninitializedData, optional64);
        EXE_IMAGE_DUMP(ss, AddressOfEntryPoint, optional64);
        EXE_IMAGE_DUMP(ss, BaseOfCode, optional64);
        EXE_IMAGE_DUMP(ss, ImageBase, optional64);
        EXE_IMAGE_DUMP(ss, SectionAlignment, optional64);
        EXE_IMAGE_DUMP(ss, FileAlignment, optional64);
        EXE_IMAGE_DUMP(ss, MajorOperatingSystemVersion, optional64);
        EXE_IMAGE_DUMP(ss, MinorOperatingSystemVersion, optional64);
        EXE_IMAGE_DUMP(ss, MajorImageVersion, optional64);
        EXE_IMAGE_DUMP(ss, MinorImageVersion, optional64);
        EXE_IMAGE_DUMP(ss, MajorSubsystemVersion, optional64);
        EXE_IMAGE_DUMP(ss, MinorSubsystemVersion, optional64);
        EXE_IMAGE_DUMP(ss, Win32VersionValue, optional64);
        EXE_IMAGE_DUMP(ss, SizeOfImage, optional64);
        EXE_IMAGE_DUMP(ss, SizeOfHeaders, optional64);
        EXE_IMAGE_DUMP(ss, CheckSum, optional64);
        EXE_IMAGE_DUMP(ss, Subsystem, optional64);
        EXE_IMAGE_DUMP(ss, DllCharacteristics, optional64);
        EXE_IMAGE_DUMP(ss, SizeOfStackReserve, optional64);
        EXE_IMAGE_DUMP(ss, SizeOfStackCommit, optional64);
        EXE_IMAGE_DUMP(ss, SizeOfHeapReserve, optional64);
        EXE_IMAGE_DUMP(ss, SizeOfHeapCommit, optional64);
        EXE_IMAGE_DUMP(ss, LoaderFlags, optional64);
        EXE_IMAGE_DUMP(ss, NumberOfRvaAndSizes, optional64);
    }
    else
    {
        const IMAGE_OPTIONAL_HEADER32 *optional32 = get_optional32();
        if (!optional32)
        {
            ss << "Invalid NT header.\n";
            return;
        }
        ss << "Optional Header is 32-bit.\n";

        EXE_IMAGE_DUMP(ss, Magic, optional32);
        ss << "MajorLinkerVersion: " << (UINT)optional32->MajorLinkerVersion << "\n";
        ss << "MinorLinkerVersion: " << (UINT)optional32->MinorLinkerVersion << "\n";
        EXE_IMAGE_DUMP(ss, SizeOfCode, optional32);
        EXE_IMAGE_DUMP(ss, SizeOfInitializedData, optional32);
        EXE_IMAGE_DUMP(ss, SizeOfUninitializedData, optional32);
        EXE_IMAGE_DUMP(ss, AddressOfEntryPoint, optional32);
        EXE_IMAGE_DUMP(ss, BaseOfCode, optional32);
        EXE_IMAGE_DUMP(ss, BaseOfData, optional32);
        EXE_IMAGE_DUMP(ss, ImageBase, optional32);
        EXE_IMAGE_DUMP(ss, SectionAlignment, optional32);
        EXE_IMAGE_DUMP(ss, FileAlignment, optional32);
        EXE_IMAGE_DUMP(ss, MajorOperatingSystemVersion, optional32);
        EXE_IMAGE_DUMP(ss, MinorOperatingSystemVersion, optional32);
        EXE_IMAGE_DUMP(ss, MajorImageVersion, optional32);
        EXE_IMAGE_DUMP(ss, MinorImageVersion, optional32);
        EXE_IMAGE_DUMP(ss, MajorSubsystemVersion, optional32);
        EXE_IMAGE_DUMP(ss, MinorSubsystemVersion, optional32);
        EXE_IMAGE_DUMP(ss, Win32VersionValue, optional32);
        EXE_IMAGE_DUMP(ss, SizeOfImage, optional32);
        EXE_IMAGE_DUMP(ss, SizeOfHeaders, optional32);
        EXE_IMAGE_DUMP(ss, CheckSum, optional32);
        EXE_IMAGE_DUMP(ss, Subsystem, optional32);
        EXE_IMAGE_DUMP(ss, DllCharacteristics, optional32);
        EXE_IMAGE_DUMP(ss, SizeOfStackReserve, optional32);
        EXE_IMAGE_DUMP(ss, SizeOfStackCommit, optional32);
        EXE_IMAGE_DUMP(ss, SizeOfHeapReserve, optional32);
        EXE_IMAGE_DUMP(ss, SizeOfHeapCommit, optional32);
        EXE_IMAGE_DUMP(ss, LoaderFlags, optional32);
        EXE_IMAGE_DUMP(ss, NumberOfRvaAndSizes, optional32);
    }

    dump_data_dir(ss);
}

inline void ExeImage::dump_data_dir(std::stringstream& ss) const
{
    ss << "\n### Data Directories ###\n";

    const IMAGE_DATA_DIRECTORY *dir = get_data_dir();
    if (!dir)
    {
        ss << "No data directories.\n";
        return;
    }

    for (DWORD dwIndex = 0; dwIndex < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; ++dwIndex)
    {
        ss << "#" << dwIndex << ": VirtualAddress " << dir->VirtualAddress <<
              ", Size " << dir->Size << ".\n";
        ++dir;
    }
}

inline void ExeImage::dump_import(std::stringstream& ss) const
{
    ss << "\n### Import ###\n";

    std::vector<const char *> names;
    if (!get_import_dll_names(names))
    {
        ss << "No import table.\n";
        return;
    }

    for (size_t i = 0; i < names.size(); ++i)
    {
        ss << names[i] << ": \n";
        std::vector<ImportSymbol> symbols;
        if (get_import_symbols(i, symbols))
        {
            for (size_t k = 0; k < symbols.size(); ++k)
            {
                ImportSymbol& symbol = symbols[k];
                if (symbol.Name.wImportByName)
                {
                    ss << "    " << symbol.pszName << " (hint: " << symbol.wHint << ")\n";
                }
                else
                {
                    ss << "    @" << symbol.Name.wOrdinal << "\n";
                }
            }
        }
    }
}

inline void ExeImage::dump_export(std::stringstream& ss) const
{
    ss << "\n### Export ###\n";

    std::vector<ExportSymbol> symbols;
    if (!get_export_symbols(symbols))
    {
        ss << "No export table.\n";
        return;
    }

    for (size_t i = 0; i < symbols.size(); ++i)
    {
        ExportSymbol& symbol = symbols[i];

        if (symbol.pszName)
            ss << "    " << symbol.pszName << ": " << symbol.dwOrdinal << "\n";
        else
            ss << "    (no name): " << symbol.dwOrdinal << "\n";
    }
}

inline void ExeImage::dump_delay_load(std::stringstream& ss) const
{
    ss << "\n### Delay Load ###\n";

    std::vector<ImgDelayDescr> entries;
    if (!get_delay_load_entries(entries))
    {
        ss << "No delay load.\n";
        return;
    }

    for (size_t i = 0; i < entries.size(); ++i)
    {
        ss << "Entry #" << i << "\n";
        ImgDelayDescr *entry = &entries[i];
        EXE_IMAGE_DUMP(ss, grAttrs, entry);
        EXE_IMAGE_DUMP(ss, rvaDLLName, entry);
        EXE_IMAGE_DUMP(ss, rvaHmod, entry);
        EXE_IMAGE_DUMP(ss, rvaIAT, entry);
        EXE_IMAGE_DUMP(ss, rvaINT, entry);
        EXE_IMAGE_DUMP(ss, rvaBoundIAT, entry);
        EXE_IMAGE_DUMP(ss, rvaUnloadIAT, entry);
        EXE_IMAGE_DUMP(ss, dwTimeStamp, entry);
    }
}

inline void ExeImage::dump_all(std::stringstream& ss) const
{
    dump_dos(ss);
    dump_nt(ss);
    dump_section_table(ss);
    dump_import(ss);
    dump_export(ss);
    dump_delay_load(ss);
}

#undef EXE_IMAGE_DUMP

////////////////////////////////////////////////////////////////////////////

} // namespace codereverse

#endif  // ndef EXE_IMAGE_HPP
