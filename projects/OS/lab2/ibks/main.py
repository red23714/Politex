import os
import time
import requests
import browser_cookie3
from concurrent.futures import ThreadPoolExecutor

base_url = "http://ibks.spbstu.ru:3508/labs/38473/tests/files/input/"
start_id = 6122757

tcpclient = [4, 4, 4, 4, 4, 4, 54, 4, 4, 5, 6]
tcpserver = [4, 4, 4, 4, 4, 4, 34, 34, 64, 4, 4, 4, 5, 5, 5, 7]
udpclient = [4, 5, 14, 5, 5, 5, 4, 5, 5, 5, 5, 5, 5, 5, 14]
udpserver = [4, 5, 14, 14, 44, 44, 44, 44]

cookies = browser_cookie3.chromium(domain_name="ibks.spbstu.ru")

session = requests.Session()
session.cookies.update(cookies)

tasks = []
current_id = start_id


def prepare_tasks(group_name, folders, start_id):
    tasks = []
    current_id = start_id

    os.makedirs(group_name, exist_ok=True)

    for folder_index, file_count in enumerate(folders, start=1):
        folder_name = os.path.join(group_name, f"folder_{folder_index}")
        os.makedirs(folder_name, exist_ok=True)

        for i in range(1, file_count + 1):
            url = base_url + str(current_id)
            tasks.append((url, folder_name))
            current_id += 1

    return tasks, current_id


tcpclient_tasks, current_id = prepare_tasks("tcpclient", tcpclient, current_id)
tcpserver_tasks, current_id = prepare_tasks("tcpserver", tcpserver, current_id)
udpclient_tasks, current_id = prepare_tasks("udpclient", udpclient, current_id)
udpserver_tasks, current_id = prepare_tasks("udpserver", udpserver, current_id)

tasks = tcpclient_tasks + tcpserver_tasks + udpclient_tasks + udpserver_tasks


def get_filename(response, url):
    cd = response.headers.get("Content-Disposition")

    if cd:
        parts = cd.split("filename=")
        if len(parts) > 1:
            filename = parts[1].strip().strip('"')
            return filename

    # fallback (если сервер не дал имя)
    return url.split("/")[-1] + ".txt"


def download(task):
    url, folder = task

    try:
        r = session.get(url)

        content_type = r.headers.get("Content-Type", "")

        if "text/html" in content_type:
            print("LOGIN PAGE INSTEAD OF FILE:", url)
            return

        if r.status_code == 200:
            filename = get_filename(r, url)
            file_path = os.path.join(folder, filename)

            with open(file_path, "wb") as f:
                f.write(r.content)

            print("OK", file_path)
        else:
            print("FAIL", url, r.status_code)

    except Exception as e:
        print("ERROR", url, e)

    time.sleep(1)


with ThreadPoolExecutor(max_workers=5) as executor:
    executor.map(download, tasks)
