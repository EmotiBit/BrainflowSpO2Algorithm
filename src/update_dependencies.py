# This script does not need to be edited manually

from pathlib import Path
import json
import os
import sys
import shutil

with open("libraries.json", "r") as file:
  libraries = json.load(file)

# change CWD to Arduino libraries folder
arduino_lib_path = os.path.join(Path.home(), "Documents", "Arduino", "libraries")
print(f"Changing working directory to {arduino_lib_path}")
os.chdir(arduino_lib_path)

for library in libraries:
  repo_name = library["git_url"].split("/")[-1].replace(".git", "")
  if Path(repo_name).exists():
    shutil.rmtree(repo_name)

  os.system(f"git clone {library["git_url"]}")
  os.chdir(os.path.join(arduino_lib_path, repo_name))
  if not os.path.exists(library["root_dir"]):
    print(f"Error: Subdirectory {library["root_dir"]} does not exist.")
    sys.exit(1)
  else:
    os.system(f"git sparse-checkout set --no-cone {library["root_dir"]}")
    shutil.copytree(library["root_dir"], os.getcwd(), dirs_exist_ok=True)
    shutil.rmtree(library["root_dir"].split("/")[0])
    print(f"Moved {library["root_dir"]} to be the root of {repo_name}")