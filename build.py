import subprocess
subprocess.run(['cmake', '-S', '.', '-B', './build', '-DCMAKE_BUILD_TYPE=Release'])
subprocess.run(['cmake', '--build', './build', '--config', 'Release'])