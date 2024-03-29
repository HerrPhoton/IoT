import re
import sys
from glob import glob


PRIVATE_VARS = ['ssid', 'password', 'mqtt_user', 'mqtt_password']
code = None

for path in glob('./**/*.ino', recursive = True):

    with open(path, 'r') as file:
        code = file.read()

    with open(path, 'w') as file:
        for var in PRIVATE_VARS:
            code = re.sub(rf'{var}\s*=\s*".*?"', rf'{var} = ""', code)

        file.write(code)

sys.exit(0)