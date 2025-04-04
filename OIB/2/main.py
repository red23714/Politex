import re

alphabet = 'абвгдеёжзийклмнопрстуфхцчшщъыьэюя'.upper()
frequens = [8.01, 1.59, 4.54, 1.7, 2.98, 8.45, 0.04, 0.94, 1.65, 7.35, 1.21, 
            3.49, 4.4, 3.21, 6.7, 10.97, 2.81, 4.73, 5.47, 6.26, 2.62, 0.26, 
            0.97, 0.48, 1.44, 0.73, 0.36, 0.04, 1.90, 1.74, 0.32, 0.64, 2.01
]
alph_freq = {k: v for k, v in zip(alphabet, frequens)}
alph_freq = dict(sorted(alph_freq.items(), key= lambda item: item[1], reverse=True))

chars_to_remove = [",", "?", "!", ":", ";", "'", '"', '\n', ".", "—"]

history = []

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
    cry = first = f.read().upper()

    for char in chars_to_remove:
        cry = cry.replace(char, "")
    
    word = list(set(re.split(" |\n", cry)))

    if '' in word:
        word.remove('')

    cry = cry.replace(' ', '').replace('\n', '')

    return first, cry, word

cryptogramm, cryptogramm_str, cryptogramm_words = read_file('in.txt')

original = cryptogramm

def replace_letter(what, to):
    global cryptogramm, cryptogramm_words

    cryptogramm = cryptogramm.replace(what, to)

    for i in range(0, len(cryptogramm_words)):
        cryptogramm_words[i] = cryptogramm_words[i].replace(what, to)


while(True):
    command = input()
    command = command.split(' ')
    
    match command[0]:
        case 'q':
            break
        case 'freq':
            cryptogramm_freq = frequence_anal(cryptogramm_str)

            keys_list_1 = list(alph_freq.keys())
            keys_list_2 = list(cryptogramm_freq.keys())

            for i in range(0, len(cryptogramm_freq)):
                print(keys_list_2[i], "->", keys_list_1[i])
        case 'mlen':
            temp = sorted(cryptogramm_words, key=lambda item: len(item), reverse=True)

            for word in temp:
                print(word)
        case 'r':
            replace_letter(command[1], command[2])

            history.append(command)

        case 'unenc':
            len_unencode = []
            for word in cryptogramm_words:
                len_unencode.append(sum(1 for ch in word if not ch.isupper()))
            
            temp = {k: v for k, v in zip(cryptogramm_words, len_unencode)}
            temp = dict(sorted(temp.items(), key= lambda item: item[1], reverse=True))

            for item in temp:
                print(item)
        
        case 'h':
            i = 1
            for command in history:
                print(i, command[1], '->', command[2])
                i += 1

        case 'undo':
            if history:
                last = history.pop(int(command[1]) - 1)
                replace_letter(last[2].lower(), last[1])

        case 'print':
            print(cryptogramm)
        case 'orig':
            print(original)
        case 'auto':
            crypt_freq = frequence_anal(cryptogramm_str)

            keys_list_1 = list(alph_freq.keys())
            keys_list_2 = list(crypt_freq.keys())

            for i in range(0, len(crypt_freq)): 
                replace_letter(keys_list_2[i], keys_list_1[i].lower())
                history.append(['r', keys_list_2[i], keys_list_1[i]])

