#include <memory>
#include <codecvt>
#include <sciter-x-api.h>

inline sciter::value utf2w(const std::string& src)
{
#if defined(WINDOWS)
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;
    return sciter::value::from_string(converter.from_bytes(src));
#else
    return sciter::value::from_string(aux::utf2w(src).c_str());
#endif
}

inline std::string w2utf(const sciter::string& src)
{
#if defined(WINDOWS)
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;
    return converter.to_bytes(src);
#else
    return aux::w2utf(src).c_str();
#endif
}

sciter::value fs_exeFileName();
sciter::value fs_exists(sciter::string filename);
sciter::value fs_createDirectories(sciter::string filename);
sciter::value fs_copyFile(sciter::string src, sciter::string dst);
sciter::value fs_remove(sciter::string filename);
sciter::value fs_rename(sciter::string src, sciter::string dst);
sciter::value fs_fileSize(sciter::string filename);

sciter::value rsa_encrypt(sciter::string public_key_str, sciter::string plain);
sciter::value rsa_decrypt(sciter::string private_key_str, sciter::string cipher);

sciter::value aes_encrypt(sciter::string key, sciter::string plain);
sciter::value aes_decrypt(sciter::string key, sciter::string cipher);

sciter::value md5sum(sciter::value plain);
