import tkinter
import re

alphabet = 'абвгдеёжзийклмнопрстуфхцчшщъыьэюя'.upper()
frequens = [8.01, 1.59, 4.54, 1.7, 2.98, 8.45, 0.04, 0.94, 1.65, 7.35, 1.21, 
            3.49, 4.4, 3.21, 6.7, 10.97, 2.81, 4.73, 5.47, 6.26, 2.62, 0.26, 
            0.97, 0.48, 1.44, 0.73, 0.36, 0.04, 1.90, 1.74, 0.32, 0.64, 2.01
]
alph_freq = {k: v for k, v in zip(alphabet, frequens)}
alph_freq = dict(sorted(alph_freq.items(), key= lambda item: item[1], reverse=True))

chars_to_remove = [",", "?", "!", ":", ";", "'", '"', '\n', ".", "—"]

def frequence_anal(str):
    symbols = [symbol for symbol in str]

    letter_counts = {k: 0 for k in symbols}

    for l in str:
        letter_counts[l] += 1

    letter_counts = dict(sorted(letter_counts.items(), key= lambda item: item[1], reverse=True))

    for k, v in letter_counts.items():
        letter_counts[k] = round(letter_counts[k] / len(str) * 100, 2)
    
    return letter_counts

def read_file(name):
    f = open(name, 'r')
    str = f.read().upper()

    for char in chars_to_remove:
        str = str.replace(char, "")
    
    word = re.split(" |\n", str)

    return str, word

cryptogramm_str, cryptogramm_words = read_file('in.txt')
cryptogramm_freq = frequence_anal(cryptogramm_str)

keys_list_1 = list(alph_freq.keys())
keys_list_2 = list(cryptogramm_freq.keys())

# for i in range(0, len(cryptogramm_freq)):
#     print(keys_list_2[i], "->", keys_list_1[i])

cryptogramm_words = sorted(set(cryptogramm_words), key=lambda item: len(item), reverse=True)

for word in cryptogramm_words:
    print(word)