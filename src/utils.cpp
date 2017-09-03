#include <rsa.h>
#include <osrng.h>
#include <base64.h>
#include <modes.h>
#include <md5.h>
#include <hex.h>
#include <easylogging++.h>
#include <boost/filesystem.hpp>

#if defined(WINDOWS)
#include <windows.h>
#else
#include <unistd.h>
#include <libproc.h>
#endif

#include "utils.h"

sciter::value fs_exists(sciter::string filename)
{
	return sciter::value(boost::filesystem::exists(w2utf(filename)));
}

sciter::value fs_createDirectories(sciter::string filename)
{
	return sciter::value(boost::filesystem::create_directories(w2utf(filename)));
}

sciter::value fs_copyFile(sciter::string src, sciter::string dst)
{
	boost::system::error_code ec;
	boost::filesystem::copy_file(w2utf(src), w2utf(dst), ec);
	return sciter::value(ec == 0);
}

sciter::value fs_remove(sciter::string filename)
{
	return sciter::value(static_cast<int>(boost::filesystem::remove_all(w2utf(filename))));
}

sciter::value fs_rename(sciter::string src, sciter::string dst)
{
	boost::system::error_code ec;
	boost::filesystem::rename(w2utf(src), w2utf(dst), ec);
	return sciter::value(ec == 0);
}

sciter::value fs_fileSize(sciter::string filename)
{
    return sciter::value(static_cast<int>(boost::filesystem::file_size(w2utf(filename))));
}

sciter::value fs_exeFileName()
{
#if defined(WINDOWS)
	std::vector<wchar_t> wpath(512);
	DWORD ret = GetModuleFileName(nullptr, wpath.data(), 512);
	std::wstring wstr(wpath.data());
    return sciter::value(wstr.c_str(), wstr.size());
#else
    pid_t pid;
    char pathbuf[PROC_PIDPATHINFO_MAXSIZE];

    pid = getpid();
    int ret = proc_pidpath (pid, pathbuf, sizeof(pathbuf));
    if (ret <= 0) {
        return sciter::value();
    } else {
        return utf2w(pathbuf);
    }
#endif
}

sciter::value rsa_encrypt(sciter::string public_key_str, sciter::string plain)
{
    std::string key_str = w2utf(public_key_str);
    std::string text = w2utf(plain);

    try {
        CryptoPP::StringSource ss(key_str, true, new CryptoPP::Base64Decoder());
        CryptoPP::RSA::PublicKey public_key;
        public_key.Load(ss);

        CryptoPP::AutoSeededRandomPool prng;
        std::string result;
        CryptoPP::RSAES_PKCS1v15_Encryptor encryptor(public_key);
        CryptoPP::StringSource(text, true, new CryptoPP::PK_EncryptorFilter(prng, encryptor,
            new CryptoPP::StringSink(result)));
        std::string b64;
        CryptoPP::StringSource(result, true,
            new CryptoPP::Base64Encoder(new CryptoPP::StringSink(b64), false));

        return utf2w(b64);
    } catch (const std::exception& e) {
        LOG(ERROR) << e.what();
        return sciter::value();
    }
}

sciter::value rsa_decrypt(sciter::string private_key_str, sciter::string cipher)
{
    std::string key_str = w2utf(private_key_str);
    std::string cipher_str = w2utf(cipher);

    try {
        CryptoPP::StringSource ss(key_str, true, new CryptoPP::Base64Decoder());
        CryptoPP::RSA::PrivateKey private_key;
        private_key.BERDecodePrivateKey(ss, false, ss.MaxRetrievable());

        CryptoPP::AutoSeededRandomPool prng;
        std::string origin_str;
        CryptoPP::StringSource(cipher_str, true,
            new CryptoPP::Base64Decoder(new CryptoPP::StringSink(origin_str)));
        std::string result;
        CryptoPP::RSAES_PKCS1v15_Decryptor decryptor(private_key);
        CryptoPP::StringSource(origin_str, true, new CryptoPP::PK_DecryptorFilter(prng, decryptor,
            new CryptoPP::StringSink(result)));

        return utf2w(result);
    } catch (const std::exception& e) {
        LOG(ERROR) << e.what();
        return sciter::value();
    }
}

sciter::value aes_encrypt(sciter::string key, sciter::string plain)
{
    std::string key_str = w2utf(key);
    std::string plain_str = w2utf(plain);

    try {
        std::string result;
        CryptoPP::ECB_Mode<CryptoPP::AES>::Encryption ecb_encryptor((byte *)key_str.c_str(), CryptoPP::AES::MAX_KEYLENGTH);
        auto encryptor = new CryptoPP::StreamTransformationFilter(ecb_encryptor,
            new CryptoPP::Base64Encoder(new CryptoPP::StringSink(result), false));
        CryptoPP::StringSource(plain_str, true, encryptor);

        return utf2w(result);
    } catch (const std::exception& e) {
        LOG(DEBUG) << e.what();
        return sciter::value();
    }
}

sciter::value aes_decrypt(sciter::string key, sciter::string cipher)
{
    std::string key_str = w2utf(key);
    std::string cipher_str = w2utf(cipher);

    try {
        std::string result;
        CryptoPP::ECB_Mode<CryptoPP::AES>::Decryption ecb_decryptor((byte *)key_str.c_str(), CryptoPP::AES::MAX_KEYLENGTH);
        auto decryptor = new CryptoPP::Base64Decoder(new CryptoPP::StreamTransformationFilter(ecb_decryptor,
            new CryptoPP::StringSink(result)));
        CryptoPP::StringSource(cipher_str, true, decryptor);

        return utf2w(result);
    } catch (const std::exception& e) {
        LOG(ERROR) << e.what();
        return sciter::value();
    }
}

sciter::value md5sum(sciter::value plain)
{
    std::string plain_str = w2utf(plain.to_string());
    std::string digest;
    CryptoPP::Weak1::MD5 md5;
    CryptoPP::StringSource(plain_str, true, new CryptoPP::HashFilter(md5,
        new CryptoPP::HexEncoder(new CryptoPP::StringSink(digest))));
    
    return utf2w(digest);
}
