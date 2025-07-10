# This script does not need to be edited manually

from pathlib import Path
import json
import os
import sys
import shutil

def onerror(func, path, exc_info):
    """
    Error handler for ``shutil.rmtree``.

    If the error is due to an access error (read only file)
    it attempts to add write permission and then retries.

    If the error is for another reason it re-raises the error.
    
    Usage : ``shutil.rmtree(path, onerror=onerror)``
    """
    import stat
    # Is the error an access error?
    if not os.access(path, os.W_OK):
        os.chmod(path, stat.S_IWUSR)
        func(path)
    else:
        raise

with open("libraries.json", "r") as file:
  libraries = json.load(file)

# change CWD to Arduino libraries folder
arduino_lib_path = os.path.join(Path.home(), "Documents", "Arduino", "libraries")
print(f"Changing working directory to {arduino_lib_path}")
os.chdir(arduino_lib_path)

for library in libraries:
  repo_name = library["git_url"].split("/")[-1].replace(".git", "")
  if Path(repo_name).exists():
    shutil.rmtree(repo_name, onerror=onerror)

  os.system(f"git clone {library["git_url"]}")
  os.chdir(os.path.join(arduino_lib_path, repo_name))
  if not os.path.exists(library["root_dir"]):
    print(f"Error: Subdirectory {library["root_dir"]} does not exist.")
    sys.exit(1)
  else:
    os.system(f"git sparse-checkout set --no-cone {library["root_dir"]}")
    shutil.copytree(library["root_dir"], os.getcwd(), dirs_exist_ok=True)
    shutil.rmtree(library["root_dir"].split("/")[0], onerror=onerror)
    print(f"Moved {library["root_dir"]} to be the root of {repo_name}")
  shutil.rmtree(".git", onerror=onerror)