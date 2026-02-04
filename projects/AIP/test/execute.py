import subprocess, test, os, shutil

def run(str_in):
    process = subprocess.Popen('main.exe', stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    input_data = str_in.encode()
    stdoutdata, stderrdata = process.communicate(input=input_data)

    return stdoutdata

print("Type the count of tests: ")
test.test_count = int(input())

test.generate_test()
test.check_test()

for i in range(0, test.test_count):
    ans = int(run(test.input[i]))
    if test.output[i] == ans:
        print(i + 1, "Done")
    else:
        print(i + 1, "Fail")
        print(test.input[i])
        print(test.output[i], ans)
        break

