# PowerInformation Sample

A C++20 command-line tool for querying and setting Windows power profile settings, including support for Intel Hybrid architecture (P-core/E-core detection).

---

## Requirements

- **Visual Studio 2022** (minimum version)
- **Windows SDK** (included with Visual Studio)
- **vcpkg** (for C++ library dependencies, if required)

---

## How to Install vcpkg

1. Clone the vcpkg repository:
    ```sh
    git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg
    ```

2. Bootstrap vcpkg:
    ```sh
    .\bootstrap-vcpkg.bat
    ```

3. Integrate vcpkg with Visual Studio:
    ```sh
    .\vcpkg integrate install
    ```

4. Install required packages (if any):
    ```sh
    vcpkg install <package-name>:x64-windows
    ```
    Replace `<package-name>` with the required libraries (e.g., `boost`, `wil`, etc.).

---

## How to Build

1. Open the solution or project in **Visual Studio 2022**.
2. Select the `x64` platform and `Release` or `Debug` configuration.
3. Build the project (`Build > Build Solution` or `Ctrl+Shift+B`).

Or, from the command line (if you have MSBuild installed):

```sh
msbuild /p:Configuration=Release /p:Platform=x64
```

---

## Usage

To use the PowerInformation tool, open a command prompt and navigate to the directory where the executable is located. Run the executable with the desired command-line arguments. For example:

```sh
PowerInformation.exe Help


PowerInformation.exe Query
PowerInformation.exe Set <ProfileName> <SettingName> <Value>
```
## Command-Line Arguments
```sh
No parameters: Displays processor information and available power profiles.
Help: Displays help information.
Query: Queries the current power settings.
Set <ProfileName> <SettingName> <Value>: Sets a specific power setting for the given profile.
```sh

## Example Commands
```sh
PowerInformation.exe 
PowerInformation.exe Help
PowerInformation.exe Query
PowerInformation.exe Set "Balanced" "ProcessorPerformanceBoost" "Enabled"
PowerInformation.exe Set "Balanced" "Heterogeneous thread scheduling policy" 5
```