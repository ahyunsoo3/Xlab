#include <openssl/evp.h>
#include <openssl/rand.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <stdexcept>

static void throwIf(bool cond, const char* msg) {
    if (cond) throw std::runtime_error(msg);
}

struct GcmEncrypted {
    std::vector<unsigned char> nonce;      // 12 bytes
    std::vector<unsigned char> ciphertext; // same length as plaintext
    std::vector<unsigned char> tag;        // 16 bytes
};

GcmEncrypted aes256_gcm_encrypt(const std::vector<unsigned char>& key,
                               const std::vector<unsigned char>& plaintext,
                               const std::vector<unsigned char>& aad = {}) {
    throwIf(key.size() != 32, "Key must be 32 bytes for AES-256");
    GcmEncrypted out;
    out.nonce.resize(12);
    out.tag.resize(16);
    out.ciphertext.resize(plaintext.size());

    throwIf(RAND_bytes(out.nonce.data(), (int)out.nonce.size()) != 1, "RAND_bytes failed");

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    throwIf(!ctx, "EVP_CIPHER_CTX_new failed");

    int len = 0;
    int ciphertext_len = 0;

    try {
        throwIf(EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1,
                "EncryptInit (cipher) failed");

        throwIf(EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, (int)out.nonce.size(), nullptr) != 1,
                "SET_IVLEN failed");

        throwIf(EVP_EncryptInit_ex(ctx, nullptr, nullptr, key.data(), out.nonce.data()) != 1,
                "EncryptInit (key/iv) failed");

        // Optional AAD (authenticated but not encrypted)
        if (!aad.empty()) {
            throwIf(EVP_EncryptUpdate(ctx, nullptr, &len, aad.data(), (int)aad.size()) != 1,
                    "EncryptUpdate (AAD) failed");
        }

        if (!plaintext.empty()) {
            throwIf(EVP_EncryptUpdate(ctx, out.ciphertext.data(), &len,
                                      plaintext.data(), (int)plaintext.size()) != 1,
                    "EncryptUpdate (pt) failed");
            ciphertext_len = len;
        }

        // Finalize (GCM doesn't output more data here typically, but still call it)
        throwIf(EVP_EncryptFinal_ex(ctx, out.ciphertext.data() + ciphertext_len, &len) != 1,
                "EncryptFinal failed");
        ciphertext_len += len;
        out.ciphertext.resize(ciphertext_len);

        throwIf(EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, (int)out.tag.size(), out.tag.data()) != 1,
                "GET_TAG failed");

        EVP_CIPHER_CTX_free(ctx);
        return out;
    } catch (...) {
        EVP_CIPHER_CTX_free(ctx);
        throw;
    }
}

std::vector<unsigned char> aes256_gcm_decrypt(const std::vector<unsigned char>& key,
                                             const std::vector<unsigned char>& nonce,
                                             const std::vector<unsigned char>& ciphertext,
                                             const std::vector<unsigned char>& tag,
                                             const std::vector<unsigned char>& aad = {}) {
    throwIf(key.size() != 32, "Key must be 32 bytes for AES-256");
    throwIf(nonce.size() != 12, "Nonce should be 12 bytes for AES-GCM");
    throwIf(tag.size() != 16, "Tag must be 16 bytes (recommended) for AES-GCM");

    std::vector<unsigned char> plaintext(ciphertext.size());

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    throwIf(!ctx, "EVP_CIPHER_CTX_new failed");

    int len = 0;
    int plaintext_len = 0;

    try {
        throwIf(EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1,
                "DecryptInit (cipher) failed");

        throwIf(EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, (int)nonce.size(), nullptr) != 1,
                "SET_IVLEN failed");

        throwIf(EVP_DecryptInit_ex(ctx, nullptr, nullptr, key.data(), nonce.data()) != 1,
                "DecryptInit (key/iv) failed");

        if (!aad.empty()) {
            throwIf(EVP_DecryptUpdate(ctx, nullptr, &len, aad.data(), (int)aad.size()) != 1,
                    "DecryptUpdate (AAD) failed");
        }

        if (!ciphertext.empty()) {
            throwIf(EVP_DecryptUpdate(ctx, plaintext.data(), &len,
                                      ciphertext.data(), (int)ciphertext.size()) != 1,
                    "DecryptUpdate (ct) failed");
            plaintext_len = len;
        }

        // Set expected tag BEFORE final
        throwIf(EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, (int)tag.size(), (void*)tag.data()) != 1,
                "SET_TAG failed");

        // Final returns 1 if tag verifies; 0 otherwise
        int ok = EVP_DecryptFinal_ex(ctx, plaintext.data() + plaintext_len, &len);
        throwIf(ok != 1, "DecryptFinal failed: tag mismatch (tampered or wrong key/nonce/tag)");

        plaintext_len += len;
        plaintext.resize(plaintext_len);

        EVP_CIPHER_CTX_free(ctx);
        return plaintext;
    } catch (...) {
        EVP_CIPHER_CTX_free(ctx);
        throw;
    }
}

int main() {
    // Example: random 32-byte key (store securely; don't regen if you need to decrypt later!)
    std::vector<unsigned char> key(32);
    RAND_bytes(key.data(), (int)key.size());

    std::string msg = "hello AES-256-GCM";
    std::vector<unsigned char> pt(msg.begin(), msg.end());

    auto enc = aes256_gcm_encrypt(key, pt);

    auto dec = aes256_gcm_decrypt(key, enc.nonce, enc.ciphertext, enc.tag);

    std::cout << "Decrypted: " << std::string(dec.begin(), dec.end()) << "\n";
}
