import sys

def rc4(key: bytes, data: bytes) -> bytes:
    S = list(range(256)); j = 0
    for i in range(256):
        j = (j + S[i] + key[i % len(key)]) % 256
        S[i], S[j] = S[j], S[i]
    i = j = 0; out = bytearray()
    for b in data:
        i = (i + 1) % 256; j = (j + S[i]) % 256
        S[i], S[j] = S[j], S[i]
        out.append(b ^ S[(S[i] + S[j]) % 256])
    return bytes(out)

blob = open(sys.argv[1], "rb").read()
klen = blob[0]
key = blob[1:1 + klen]
enc = blob[1 + klen:]
dec = rc4(key, enc)

print(f"размер ресурса: {len(blob)}, длина ключа: {klen}, данных: {len(enc)}")
print("--- сырой результат ---")
print(repr(dec))
print("--- по полям ---")
for sep in (b"\x1e", b"|\x1e|", b"|"):
    if sep in dec:
        for n, f in enumerate(dec.split(sep)):
            print(n, repr(f))
        print("разделитель:", repr(sep))
        break
