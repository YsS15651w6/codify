# Codify

**Codify** (v1.2) is a versatile and powerful text encoding and decoding tool equipped with both a Command Line Interface (CLI) and a Graphical User Interface (GUI). It provides support for multiple custom encoding formats, with optional password protection for enhanced security.

---

## Features

-   **Dual Interface**: Seamlessly switch between CLI and GUI modes.
-   **Multiple Codecs**: Support for various custom encoding algorithms.
-   **Secure Encoding**: Password-protected encoding with `eclcrypt_pwd`.
-   **Cross-Platform**: Builds on Windows, Linux, and macOS using CMake.

## Supported Formats

| Key            | Description                      | Password Required |
| :------------- | :------------------------------- | :---------------- |
| `pigl`         | PIGL Encoding                    | No                |
| `ubdb`         | UBDB Encoding                    | No                |
| `eclcrypt`     | ECL Crypt Encoding               | No                |
| `eclcrypt_pwd` | ECL Crypt with Password Encoding | **Yes**           |

---

## 🛠 Build Instructions

### Prerequisites

-   **CMake** (3.10 or higher)
-   **C Compiler** (GCC, Clang, or MSVC)

### Quick Build

**Windows (PowerShell)**:
```powershell
.\build.ps1
```

**Linux / macOS (Bash)**:
```bash
./build.sh
```

### Manual Build

1.  Create a build directory:
    ```bash
    mkdir build
    cd build
    ```
2.  Generate build files with CMake:
    ```bash
    cmake ..
    ```
3.  Compile:
    ```bash
    cmake --build . --config Release
    ```

### Packaging

To generate a standalone installer or package:

```bash
cpack -C Release
```

-   **Windows**: Generates an NSIS installer and a ZIP archive.
-   **Linux**: Generates a `.deb` package and a `.tar.gz` archive.

---

## Usage

Run the application in **GUI mode** by simply executing the binary without arguments:

```bash
./codify
```

### CLI Mode

To use the Command Line Interface, pass `cli` as the first argument.

**Syntax**:
```bash
codify cli <command> <format> [text] [password] [flags]
```

**Commands**:
-   `encode`: Encodes the input text.
-   `decode`: Decodes the input text.
-   `help`: Displays the help message.

**Arguments**:
-   `<format>`: The encoding format key (e.g., `pigl`, `eclcrypt`).
-   `[text]`: Direct text input (optional if using `-i`).
-   `[password]`: Password for formats that require it (e.g., `eclcrypt_pwd`).

**Flags**:
-   `-i, --input <file>`: Read input from a file.
-   `-o, --output <file>`: Write output to a file.
-   `-v, --version`: Show version information.
-   `-h, --help`: Show help.

### Examples

**Basic Encoding**:
```bash
./codify cli encode ubdb "Hello World"
```

**Password Protected Encoding**:
```bash
./codify cli encode eclcrypt_pwd "Secret Message" "MyPassword123"
```

**File Processing**:
```bash
./codify cli decode pigl -i encoded.txt -o decoded.txt
```

---

## License

This project is licensed under the **GPLv3 License**. See the [LICENSE](LICENSE) file for details.