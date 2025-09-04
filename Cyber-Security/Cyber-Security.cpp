// Cyber Security.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Cyber Security.h"
#include <windows.h>
#include <commdlg.h>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>


// Crypto++ headers (Corrected Paths)
#include "rsa.h"
#include "osrng.h"
#include "base64.h"
#include "files.h"
#include "dsa.h"
#include "sha.h"
#include "pssr.h"
#include "whrlpool.h"

// --- START OF FINAL FIX ---
// Manually define the missing identifier to force the compiler to recognize it.
#ifndef OFN_OVERWRITEREPROMPT
#define OFN_OVERWRITEREPROMPT 0x00000002
#endif
// --- END OF FINAL FIX ---

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// --- GUI Control Handles ---
HWND hInputFileButton;
HWND hOutputFileButton;
HWND hProcessButton;
HWND hStatusLabel;
HWND hInputFileText;
HWND hOutputFileText;
HWND hOperationComboBox;

// --- Crypto++ Functions ---
void GenerateRSAKeys();
void EncryptRSA(const wchar_t* inputFile);
void DecryptRSA(const wchar_t* inputFile);
void GenerateDSASignature(const wchar_t* inputFile);
void VerifyDSASignature(const wchar_t* inputFile);

// --- Helper Functions ---
void SaveKey(const std::string& filename, const CryptoPP::RSA::PublicKey& key);
void SaveKey(const std::string& filename, const CryptoPP::RSA::PrivateKey& key);
void LoadKey(const std::string& filename, CryptoPP::RSA::PublicKey& key);
void LoadKey(const std::string& filename, CryptoPP::RSA::PrivateKey& key);

void SaveKey(const std::string& filename, const CryptoPP::DSA::PublicKey& key);
void SaveKey(const std::string& filename, const CryptoPP::DSA::PrivateKey& key);
void LoadKey(const std::string& filename, CryptoPP::DSA::PublicKey& key);
void LoadKey(const std::string& filename, CryptoPP::DSA::PrivateKey& key);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CYBERSECURITY, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CYBERSECURITY));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return static_cast<int>(msg.wParam); (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CYBERSECURITY));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CYBERSECURITY);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 400, 300, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        // Create GUI elements
        hInputFileButton = CreateWindow(
            L"BUTTON", L"Upload", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 10, 100, 30, hWnd, (HMENU)1, hInst, NULL);

        hInputFileText = CreateWindow(
            L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER,
            120, 10, 250, 30, hWnd, (HMENU)2, hInst, NULL);

        hProcessButton = CreateWindow(
            L"BUTTON", L"Process", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 50, 100, 30, hWnd, (HMENU)3, hInst, NULL);

        hOutputFileButton = CreateWindow(
            L"BUTTON", L"Download", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 90, 100, 30, hWnd, (HMENU)4, hInst, NULL);

        hOutputFileText = CreateWindow(
            L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER,
            120, 90, 250, 30, hWnd, (HMENU)5, hInst, NULL);


        hStatusLabel = CreateWindow(
            L"STATIC", L"Status: Ready", WS_CHILD | WS_VISIBLE | SS_LEFT,
            10, 130, 360, 20, hWnd, (HMENU)6, hInst, NULL);

        hOperationComboBox = CreateWindow(
            L"COMBOBOX", L"", CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
            120, 50, 250, 100, hWnd, (HMENU)7, hInst, NULL);
        // Add items to the combo box
        SendMessage(hOperationComboBox, CB_ADDSTRING, 0, (LPARAM)L"RSA Key Generation");
        SendMessage(hOperationComboBox, CB_ADDSTRING, 0, (LPARAM)L"RSA Encryption");
        SendMessage(hOperationComboBox, CB_ADDSTRING, 0, (LPARAM)L"RSA Decryption");
        SendMessage(hOperationComboBox, CB_ADDSTRING, 0, (LPARAM)L"DSA Signature Generation");
        SendMessage(hOperationComboBox, CB_ADDSTRING, 0, (LPARAM)L"DSA Signature Verification");
        // Set the default selection
        SendMessage(hOperationComboBox, CB_SETCURSEL, 0, 0);
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case 1: // Upload Button
        {
            OPENFILENAME ofn;
            wchar_t szFile[260] = { 0 };
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = L"All Files\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileName(&ofn) == TRUE)
            {
                SetWindowText(hInputFileText, ofn.lpstrFile);
            }
        }
        break;
        case 3: // Process Button
        {
            // Get selected operation
            int selectedIndex = SendMessage(hOperationComboBox, CB_GETCURSEL, 0, 0);
            // Get input file path
            wchar_t inputFile[260];
            GetWindowText(hInputFileText, inputFile, 260);

            SetWindowText(hStatusLabel, L"Status: Processing...");

            // Call the appropriate crypto function based on the selected operation
            switch (selectedIndex)
            {
            case 0: // RSA Key Generation
                GenerateRSAKeys();
                break;
            case 1: // RSA Encryption
                EncryptRSA(inputFile);
                break;
            case 2: // RSA Decryption
                DecryptRSA(inputFile);
                break;
            case 3: // DSA Signature Generation
                GenerateDSASignature(inputFile);
                break;
            case 4: // DSA Signature Verification
                VerifyDSASignature(inputFile);
                break;
            }
        }
        break;
        case 4: // Download Button
        {
            OPENFILENAME ofn;
            wchar_t szFile[260] = { 0 };
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = L"All Files\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_OVERWRITEREPROMPT;

            if (GetSaveFileName(&ofn) == TRUE)
            {
                SetWindowText(hOutputFileText, ofn.lpstrFile);
                // The saving of the processed data happens within each crypto function
            }
        }
        break;
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


// --- Crypto++ Implementations ---

void GenerateRSAKeys() {
    CryptoPP::AutoSeededRandomPool rng;

    CryptoPP::RSA::PrivateKey privateKey;
    privateKey.GenerateRandomWithKeySize(rng, 2048);
    CryptoPP::RSA::PublicKey publicKey(privateKey);

    SaveKey("rsa-private.key", privateKey);
    SaveKey("rsa-public.key", publicKey);

    SetWindowText(hStatusLabel, L"Status: RSA key pair generated.");
}

void EncryptRSA(const wchar_t* inputFile) {
    try {
        CryptoPP::RSA::PublicKey publicKey;
        LoadKey("rsa-public.key", publicKey);

        std::ifstream file(inputFile, std::ios::binary);
        if (!file.is_open()) {
            SetWindowText(hStatusLabel, L"Status: Error opening input file.");
            return;
        }

        std::ostringstream ss;
        ss << file.rdbuf();
        std::string plaintext = ss.str();

        std::string ciphertext;
        CryptoPP::RSAES_OAEP_SHA_Encryptor e(publicKey);
        CryptoPP::AutoSeededRandomPool rng;

        CryptoPP::StringSource(plaintext, true,
            new CryptoPP::PK_EncryptorFilter(rng, e,
                new CryptoPP::StringSink(ciphertext)
            )
        );

        std::ofstream out("encrypted.dat", std::ios::binary);
        out.write(ciphertext.c_str(), ciphertext.length());
        out.close();

        SetWindowText(hStatusLabel, L"Status: File encrypted successfully.");

    }
    catch (const CryptoPP::Exception& e) {
        std::wstringstream ws;
        ws << L"Status: Crypto++ Exception: " << e.what();
        SetWindowText(hStatusLabel, ws.str().c_str());
    }
}


void DecryptRSA(const wchar_t* inputFile) {
    try {
        CryptoPP::RSA::PrivateKey privateKey;
        LoadKey("rsa-private.key", privateKey);

        std::ifstream file(inputFile, std::ios::binary);
        if (!file.is_open()) {
            SetWindowText(hStatusLabel, L"Status: Error opening input file.");
            return;
        }
        std::ostringstream ss;
        ss << file.rdbuf();
        std::string ciphertext = ss.str();

        std::string plaintext;
        CryptoPP::RSAES_OAEP_SHA_Decryptor d(privateKey);
        CryptoPP::AutoSeededRandomPool rng;

        CryptoPP::StringSource(ciphertext, true,
            new CryptoPP::PK_DecryptorFilter(rng, d,
                new CryptoPP::StringSink(plaintext)
            )
        );
        wchar_t outputFile[260];
        GetWindowText(hOutputFileText, outputFile, 260);


        std::ofstream out(outputFile, std::ios::binary);
        out.write(plaintext.c_str(), plaintext.length());
        out.close();

        SetWindowText(hStatusLabel, L"Status: File decrypted successfully.");
    }
    catch (const CryptoPP::Exception& e) {
        std::wstringstream ws;
        ws << L"Status: Crypto++ Exception: " << e.what();
        SetWindowText(hStatusLabel, ws.str().c_str());
    }
}


void GenerateDSASignature(const wchar_t* inputFile)
{
    try
    {
        CryptoPP::AutoSeededRandomPool rng;

        // Generate keys
        CryptoPP::DSA::PrivateKey privateKey;
        privateKey.GenerateRandomWithKeySize(rng, 1024);

        CryptoPP::DSA::PublicKey publicKey;
        publicKey.AssignFrom(privateKey);

        SaveKey("dsa-private.key", privateKey);
        SaveKey("dsa-public.key", publicKey);


        // Sign message
        CryptoPP::DSA::Signer signer(privateKey);

        std::ifstream file(inputFile);
        if (!file.is_open()) {
            SetWindowText(hStatusLabel, L"Status: Error opening input file.");
            return;
        }

        std::string message((std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());

        std::string signature;
        CryptoPP::StringSource(message, true,
            new CryptoPP::SignerFilter(rng, signer,
                new CryptoPP::StringSink(signature)
            )
        );

        // Save signature
        std::ofstream sigFile("signature.dat", std::ios::binary);
        sigFile.write(signature.c_str(), signature.size());
        sigFile.close();

        SetWindowText(hStatusLabel, L"Status: DSA signature generated.");
    }
    catch (const CryptoPP::Exception& e)
    {
        std::wstringstream ws;
        ws << L"Status: Crypto++ Exception: " << e.what();
        SetWindowText(hStatusLabel, ws.str().c_str());
    }
}

void VerifyDSASignature(const wchar_t* inputFile)
{
    try
    {
        CryptoPP::DSA::PublicKey publicKey;
        LoadKey("dsa-public.key", publicKey);

        std::ifstream sigFile("signature.dat", std::ios::binary);
        if (!sigFile.is_open()) {
            SetWindowText(hStatusLabel, L"Status: Error opening signature file.");
            return;
        }
        std::string signature((std::istreambuf_iterator<char>(sigFile)),
            std::istreambuf_iterator<char>());


        std::ifstream file(inputFile);
        if (!file.is_open()) {
            SetWindowText(hStatusLabel, L"Status: Error opening input file.");
            return;
        }
        std::string message((std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());


        CryptoPP::DSA::Verifier verifier(publicKey);
        bool result = false;
        CryptoPP::StringSource(message + signature, true,
            new CryptoPP::SignatureVerificationFilter(
                verifier,
                new CryptoPP::ArraySink((CryptoPP::byte*)&result, sizeof(result))
            )
        );


        if (result)
            SetWindowText(hStatusLabel, L"Status: Signature is valid.");
        else
            SetWindowText(hStatusLabel, L"Status: Signature is not valid.");
    }
    catch (const CryptoPP::Exception& e)
    {
        std::wstringstream ws;
        ws << L"Status: Crypto++ Exception: " << e.what();
        SetWindowText(hStatusLabel, ws.str().c_str());
    }
}


// --- Helper Function Implementations ---

void SaveKey(const std::string& filename, const CryptoPP::RSA::PublicKey& key) {
    CryptoPP::ByteQueue queue;
    key.Save(queue);
    CryptoPP::FileSink file(filename.c_str());
    queue.CopyTo(file);
    file.MessageEnd();
}

void SaveKey(const std::string& filename, const CryptoPP::RSA::PrivateKey& key) {
    CryptoPP::ByteQueue queue;
    key.Save(queue);
    CryptoPP::FileSink file(filename.c_str());
    queue.CopyTo(file);
    file.MessageEnd();
}

void LoadKey(const std::string& filename, CryptoPP::RSA::PublicKey& key) {
    CryptoPP::ByteQueue queue;
    CryptoPP::FileSource file(filename.c_str(), true);
    file.TransferTo(queue);
    queue.MessageEnd();
    key.Load(queue);
}

void LoadKey(const std::string& filename, CryptoPP::RSA::PrivateKey& key) {
    CryptoPP::ByteQueue queue;
    CryptoPP::FileSource file(filename.c_str(), true);
    file.TransferTo(queue);
    queue.MessageEnd();
    key.Load(queue);
}

void SaveKey(const std::string& filename, const CryptoPP::DSA::PublicKey& key) {
    CryptoPP::ByteQueue queue;
    key.Save(queue);
    CryptoPP::FileSink file(filename.c_str());
    queue.CopyTo(file);
    file.MessageEnd();
}

void SaveKey(const std::string& filename, const CryptoPP::DSA::PrivateKey& key) {
    CryptoPP::ByteQueue queue;
    key.Save(queue);
    CryptoPP::FileSink file(filename.c_str());
    queue.CopyTo(file);
    file.MessageEnd();
}

void LoadKey(const std::string& filename, CryptoPP::DSA::PublicKey& key) {
    CryptoPP::ByteQueue queue;
    CryptoPP::FileSource file(filename.c_str(), true);
    file.TransferTo(queue);
    queue.MessageEnd();
    key.Load(queue);
}

void LoadKey(const std::string& filename, CryptoPP::DSA::PrivateKey& key) {
    CryptoPP::ByteQueue queue;
    CryptoPP::FileSource file(filename.c_str(), true);
    file.TransferTo(queue);
    queue.MessageEnd();
    key.Load(queue);
}