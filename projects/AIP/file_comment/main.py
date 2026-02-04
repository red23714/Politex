import subprocess, os, shutil, time
from natsort import natsorted

def run(file_name):
    process = subprocess.Popen(file_name, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    process = subprocess.Popen('timur.exe', stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

files = os.listdir("./test")
files = natsorted(files)

file_name = "main.exe" #input("Название программы с exe\n")

for file in files:
    shutil.copy2("./test/" + file, "./test.c")
    run(file_name)
    time.sleep(0.1)
    ans = open("./test.wc", 'r')
    source = open("./test2.wc", 'r')
    s_ans = ans.read()
    s_source = source.read()
    s_ans = s_ans.replace('\n', '')
    s_ans = s_ans.replace(' ', '')
    s_source = s_source.replace('\n', '')
    s_source = s_source.replace(' ', '')

    print("Test " + file)
    if s_source == s_ans:
        print("Good")
    else:
        print("Fail")
        break
            
input()
    

