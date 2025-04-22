import os

extensions = ('.cpp', '.h')
line_count = 0

for file in os.listdir('.'):
    if file.endswith(extensions) and os.path.isfile(file):
        with open(file, 'r', errors='ignore') as f:
            line_count += sum(1 for _ in f)

print(f"Total lines of code: {line_count}")
