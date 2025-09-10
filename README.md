# Cyber Security Cryptography Tool

A user-friendly Windows desktop application for performing essential cryptographic operations. This tool provides a graphical user interface (GUI) for encrypting and decrypting files using **RSA** and generating and verifying digital signatures using **DSA**. It's designed as a practical demonstration of the **Crypto++** library in a C++ Win32 environment.

---

## Output

1. **Application UI**

![Application Screenshot](./Images/UI.png)
---

## Features

### 🔐 RSA Asymmetric Encryption

* **Generate RSA Keys:** Creates a 2048-bit RSA public/private key pair.
* **Encrypt Files:** Encrypts any file using the generated public key.
* **Decrypt Files:** Decrypts files using the corresponding private key.

### ✍️ DSA Digital Signatures

* **Generate DSA Keys & Signature:** Creates a 1024-bit DSA key pair and generates a digital signature for a chosen file.
* **Verify Signature:** Checks the integrity and authenticity of a file by verifying its digital signature.

### 💻 User-Friendly Interface

* **Responsive UI:** Cryptographic operations run on a background thread to prevent UI freezing.
* **Progress Bar:** Provides visual feedback during file processing.
* **Clear Error Handling:** Displays detailed error messages in pop-up dialogs.

---

## 📂 File Structure

```
Cyber-Security-Project/
|-- .gitattributes
|-- .gitignore
|-- Cyber-Security.sln
|-- Cyber-Security/
|   |-- Cyber-Security.cpp
|   |-- Cyber-Security.h
|   |-- Cyber-Security.ico
|   |-- Cyber-Security.rc
|   |-- Cyber-Security.vcxproj
|   |-- framework.h
|   |-- Resource.h
|   `-- ... (other project files)
|-- Images/
|   |-- key_gen.png
|   |-- rsa_en.png
|   |-- rsa_de.png
|   |-- dsa_sig_gen.png
|   `-- dsa_sig_verify.png
|-- README.md
`-- test/
    |-- Message.txt
    `-- ... (test files)
```

---

## Component Diagram

```mermaid
flowchart TD
    UI[UI (Win32 API)] -- "Invokes Operations" --> CryptoLogic[Crypto Logic]
    CryptoLogic -- "Uses" --> CryptoPP[Crypto++ Library]
    CryptoLogic -- "Reads/Writes Files" --> FileSystem[File System]

```

---

## Application Workflow Canvas

```mermaid
graph TD
    A[Start Application] --> B{Select Operation};
    B --> C[RSA Encryption];
    B --> D[RSA Decryption];
    B --> E[DSA Signature Generation];
    B --> F[DSA Signature Verification];

    subgraph "Encryption Flow"
        C --> C1[1. Upload Plaintext File];
        C1 --> C2[2. Process];
        C2 --> C3[Creates encrypted.dat];
    end

    subgraph "Decryption Flow"
        D --> D1[1. Upload encrypted.dat];
        D1 --> D2[2. Set Download Location];
        D2 --> D3[3. Process];
        D3 --> D4[Saves Decrypted File];
    end

    subgraph "Signature Flow"
        E --> E1[1. Upload Original File];
        E1 --> E2[2. Process];
        E2 --> E3[Creates signature.dat & DSA keys];
    end

    subgraph "Verification Flow"
        F --> F1[1. Upload Original File];
        F1 --> F2[2. Process];
        F2 --> F3{Result: Valid / Invalid};
    end
```

---

## Sequence Diagrams

### 🔑 RSA Key Pair Generation

```mermaid
sequenceDiagram
    participant User
    participant UI
    participant CryptoLogic
    participant CryptoPP

    User->>UI: Request RSA Key Generation
    UI->>CryptoLogic: Generate RSA Keys
    CryptoLogic->>CryptoPP: Call RSA KeyGen API
    CryptoPP-->>CryptoLogic: Return Public/Private Keys
    CryptoLogic-->>UI: Save Keys to Files
    UI-->>User: Keys Generated Successfully
```

### 🔐 RSA Encryption

```mermaid
sequenceDiagram
    participant User
    participant UI
    participant CryptoLogic
    participant CryptoPP
    participant FileSystem

    User->>UI: Select File for Encryption
    UI->>CryptoLogic: Encrypt File
    CryptoLogic->>FileSystem: Read Plaintext File
    CryptoLogic->>CryptoPP: Encrypt with Public Key
    CryptoPP-->>CryptoLogic: Return Ciphertext
    CryptoLogic->>FileSystem: Write encrypted.dat
    UI-->>User: Encryption Completed
```

### 🔓 RSA Decryption

```mermaid
sequenceDiagram
    participant User
    participant UI
    participant CryptoLogic
    participant CryptoPP
    participant FileSystem

    User->>UI: Select encrypted.dat
    UI->>CryptoLogic: Decrypt File
    CryptoLogic->>FileSystem: Read encrypted.dat
    CryptoLogic->>CryptoPP: Decrypt with Private Key
    CryptoPP-->>CryptoLogic: Return Plaintext
    CryptoLogic->>FileSystem: Save Decrypted File
    UI-->>User: Decryption Completed
```

### 🖊️ DSA Signature Generation

```mermaid
sequenceDiagram
    participant User
    participant UI
    participant CryptoLogic
    participant CryptoPP
    participant FileSystem

    User->>UI: Select File for Signature
    UI->>CryptoLogic: Generate Signature
    CryptoLogic->>FileSystem: Read File
    CryptoLogic->>CryptoPP: Generate Signature with DSA Keys
    CryptoPP-->>CryptoLogic: Return Signature
    CryptoLogic->>FileSystem: Save signature.dat
    UI-->>User: Signature Generated
```

### ✅ DSA Signature Verification

```mermaid
sequenceDiagram
    participant User
    participant UI
    participant CryptoLogic
    participant CryptoPP
    participant FileSystem

    User->>UI: Select File + signature.dat
    UI->>CryptoLogic: Verify Signature
    CryptoLogic->>FileSystem: Read File & signature.dat
    CryptoLogic->>CryptoPP: Verify Signature with DSA Public Key
    CryptoPP-->>CryptoLogic: Return Verification Result
    UI-->>User: Valid / Invalid
```

---

## Getting Started

### 🔧 Prerequisites

* **Visual Studio 2022:** Install with the *Desktop development with C++* workload.
* **Crypto++ Library:** Required for cryptographic functions.

  * **Download:** [Crypto++ Official Website](https://www.cryptopp.com)
  * **Integration:** Configure Visual Studio project to link against the Crypto++ library.
  * Set *Include Directories* and *Library Directories* to point to your Crypto++ installation.

### 🚀 How to Build and Run

1. **Clone the Repository**

```bash
git clone https://github.com/Krishna200608/Cyber-Security-Project.git
```

2. **Open in Visual Studio**
   Open the `Cyber-Security.sln` solution file.

3. **Configure Crypto++**

   * Right-click on the **Cyber-Security** project → Properties.
   * Set **Configuration**: `Debug`, **Platform**: `x64`.
   * Go to *Configuration Properties → VC++ Directories*:

     * **Include Directories:** `C:\Users\YourUser\Downloads\cryptopp890`
     * **Library Directories:** `C:\Users\YourUser\Downloads\cryptopp890\x64\Output\Debug`
   * Go to *Configuration Properties → Linker → Input*:

     * Add `cryptlib.lib` to **Additional Dependencies**.
   * Click **Apply** → **OK**.

4. **Set Build Configuration**
   Choose **Debug** or **Release** for the `x64` platform.

5. **Build the Solution**
   From the top menu: `Build → Build Solution`.

6. **Run the Application**
   Press `F5` or run the generated `.exe` from the build output folder (`x64/Debug` or `x64/Release`).
