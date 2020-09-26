# aris-euler-avionics

## Pushing to Remote Repository

In order to ensure consistency and easier diff review between commits,
*clang-format* should be used to format all modified C/C++ files. 

The entire codebase is already formatted with *clang-format v12.0.0*.

To download `clang-format` visit this [page](https://releases.llvm.org/download.html).

To automatically format the modified code before committing, you should
copy the pre-commit hook from `./hooks` to `./git/hooks`.

Unfortunately, it is not easily possible to check whether the code is
formatted correctly on the server side (while you are doing a push) so
_**please**_ do this if you are working with C/C++ files. 

## Ground Station
* **Installation**<br/>
The groundstation GUI requires **Python 3.7**. The required Python packages can be installed as follows:<br/>
``pip install -r requirements.txt``
* **Run groundstation GUI**<br/>
To run the groundstation software execute following command in the console:<br/>
``python main.py``
* **Logging**<br/>
The log is stored in *Logs/Groundstation.log*<br/>
It logs information pertaining to events like serial communication,...
* **Recording**<br/>
Received data is stored in *recording.csv*.
