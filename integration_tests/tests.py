import client
import sys
import subprocess
if sys.platform == 'win32':
    server_path = "../build/bin/Release/server.exe"
else:
    server_path = "../build/bin/Release/server"
rulesPath = "./res/rules.csv"
server_proc = subprocess.Popen([server_path, "--rules", rulesPath])
client.test_all_cases('./res/test_cases')
server_proc.kill()