# vCard-Parser
This application processes vCard files located in the `bin/cards` directory and stores the extracted information in both an in-memory SQLite database and a remote MySQL database. It provides a text-based user interface (TUI) built with `asciimatics` for interacting with the vCard data.

## Prerequisites

Before running the application, ensure you have the following installed:

* **Python 3:** The script is written in Python 3.
* **`ctypes`:** This module is part of the standard Python library and is used to interface with the C shared library.
* **`sqlite3`:** This module is part of the standard Python library for SQLite database interaction.
* **`mysql.connector`:** This library is required to connect to the MySQL database. You can install it using pip:
    ```bash
    pip install mysql-connector-python
    ```
* **`asciimatics`:** This library is used for the text-based user interface. You can install it using pip:
    ```bash
    pip install asciimatics
    ```
* **GCC (or a compatible C compiler):** Required to compile the C source files into a shared library.

## Building the C Shared Library

The Python script relies on a compiled C shared library (`libvcparser.so`). You need to build this library from the provided C source files (`LinkedListAPI.c`, `VCHelpers.c`, `VCParser.c`).

On Linux/macOS, you can typically use the following command in your terminal within the repository root directory:

```bash
gcc -shared -o bin/libvcparser.so LinkedListAPI.c VCHelpers.c VCParser.c -Iinclude
