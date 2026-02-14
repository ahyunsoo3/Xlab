from cryptography.hazmat.primitives.ciphers.aead import AESGCM
import os

def aes_gcm_encrypt(key: bytes, plaintext: bytes, aad: bytes = b""):
    if len(key) != 32:
        raise ValueError("Key must be 32 bytes for AES-256-GCM")

    nonce = os.urandom(12)
    aesgcm = AESGCM(key)
    ciphertext_with_tag = aesgcm.encrypt(nonce, plaintext, aad)
    return nonce, ciphertext_with_tag

def aes_gcm_decrypt(key: bytes, nonce: bytes, ciphertext_with_tag: bytes, aad: bytes = b""):
    if len(key) != 32:
        raise ValueError("Key must be 32 bytes for AES-256-GCM")
    if len(nonce) != 12:
        raise ValueError("Nonce must be 12 bytes for AES-GCM")

    aesgcm = AESGCM(key)
    plaintext = aesgcm.decrypt(nonce, ciphertext_with_tag, aad)
    return plaintext

if __name__ == "__main__":
    key = os.urandom(32)
    print(key)
    msg = b"hello AES-256-GCM"

    nonce, ct = aes_gcm_encrypt(key, msg, aad=b"header-data")
    print(f'\n{nonce}\n{ct}\n')
    pt = aes_gcm_decrypt(key, nonce, ct, aad=b"header-data")

    print("Decrypted:", pt.decode())
