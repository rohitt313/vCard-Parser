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
```

## Running the Application

1.  **Clone the repository** to your local machine.
2.  **Navigate to the repository root directory** in your terminal.
3.  **Build the C shared library:** Use the following command in your terminal within the repository root directory:
    ```bash
    gcc -shared -o bin/libvcparser.so LinkedListAPI.c VCHelpers.c VCParser.c -Iinclude
    ```
    * This command compiles the C source files and creates the shared library `libvcparser.so` in the `bin` directory. Ensure you have GCC installed.
4.  **Navigate to the `bin` directory:**
    ```bash
    cd bin
    ```
    * This moves your terminal's current location to the `bin` folder where the Python script and the shared library will reside.
5.  **Ensure vCard files are in the `cards` directory:**
    * If the `cards` directory doesn't exist within the `bin` directory, create it:
        ```bash
        mkdir cards
        ```
    * Place all your `.vcf` vCard files into this `bin/cards` directory. The Python script will look for them here.
6.  **Configure MySQL connection (if needed):**
    * Open the `A3main.py` script in a text editor.
    * Locate the section where the MySQL connection is established. Look for lines involving `mysql.connector.connect()`.
    * Modify the connection parameters (host, user, password, database name) to match your MySQL server credentials.
    * Save the changes to `A3main.py`. If you don't intend to use MySQL, you can likely skip this step, but the script might still attempt a connection.
7.  **Run the Python script:**
    ```bash
    python A3main.py
    ```
    * This command executes the `A3main.py` script using the Python interpreter. The script will then load the C shared library, parse the vCard files, store the data, and launch the text-based user interface.
8.  **Interact with the TUI:** Once the script runs successfully, you should see a text-based interface in your terminal. Follow the on-screen instructions to navigate and interact with the vCard data.

**Troubleshooting:**

* **`libvcparser.so` not found:** Ensure that the `gcc` command in step 3 executed without errors and that the `libvcparser.so` file exists in the `bin` directory.
* **MySQL connection errors:** Double-check the MySQL connection details you entered in `A3main.py` (step 6). Ensure your MySQL server is running and accessible.
* **`asciimatics` or `mysql.connector` not found:** If you get import errors, make sure you have installed the required Python libraries using `pip install asciimatics mysql-connector-python`.
* **No vCards loaded:** Verify that you have placed `.vcf` files in the `bin/cards` directory (step 5).
