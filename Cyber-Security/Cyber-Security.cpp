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
#include <thread>

// Crypto++ headers
#include "rsa.h"
#include "osrng.h"
#include "base64.h"
#include "files.h"
#include "dsa.h"
#include "sha.h"
#include "pssr.h"
#include "whrlpool.h"

// Headers and libraries for UI enhancements
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

// Manually define the missing identifier if not found
#ifndef OFN_OVERWRITEREPROMPT
#define OFN_OVERWRITEREPROMPT 0x00000002
#endif

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

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

// --- Crypto++ Functions (Updated Declarations) ---
void GenerateRSAKeys(HWND hWnd);
void EncryptRSA(const wchar_t* inputFile, HWND hWnd);
void DecryptRSA(const wchar_t* inputFile, HWND hWnd);
void GenerateDSASignature(const wchar_t* inputFile, HWND hWnd);
void VerifyDSASignature(const wchar_t* inputFile, HWND hWnd);

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

    return (int)msg.wParam;
}

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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hProgressBar; // Make progress bar handle accessible

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

        hProgressBar = CreateWindowEx(
            0, PROGRESS_CLASS, NULL,
            WS_CHILD | WS_VISIBLE,
            10, 160, 360, 20, // Position it below the status label
            hWnd, (HMENU)8, hInst, NULL);

        // Add items to the combo box
        SendMessage(hOperationComboBox, CB_ADDSTRING, 0, (LPARAM)L"RSA Key Generation");
        SendMessage(hOperationComboBox, CB_ADDSTRING, 0, (LPARAM)L"RSA Encryption");
        SendMessage(hOperationComboBox, CB_ADDSTRING, 0, (LPARAM)L"RSA Decryption");
        SendMessage(hOperationComboBox, CB_ADDSTRING, 0, (LPARAM)L"DSA Signature Generation");
        SendMessage(hOperationComboBox, CB_ADDSTRING, 0, (LPARAM)L"DSA Signature Verification");
        SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
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
            int selectedIndex = SendMessage(hOperationComboBox, CB_GETCURSEL, 0, 0);
            static wchar_t inputFile[260]; // Make static so it persists for the thread
            GetWindowText(hInputFileText, inputFile, 260);

            // Basic input validation
            if (selectedIndex > 0 && wcslen(inputFile) == 0) {
                MessageBox(hWnd, L"Please select an input file first.", L"Input Required", MB_OK | MB_ICONWARNING);
                return 0;
            }

            SetWindowText(hStatusLabel, L"Status: Processing...");
            SendMessage(hProgressBar, PBM_SETPOS, 0, 0); // Reset progress bar
            EnableWindow(hProcessButton, FALSE); // Disable the button

            // Run the crypto operation in a separate thread to keep the UI responsive
            std::thread([=]() {
                switch (selectedIndex)
                {
                case 0: GenerateRSAKeys(hWnd); break;
                case 1: EncryptRSA(inputFile, hWnd); break;
                case 2: DecryptRSA(inputFile, hWnd); break;
                case 3: GenerateDSASignature(inputFile, hWnd); break;
                case 4: VerifyDSASignature(inputFile, hWnd); break;
                }
                // When the thread is finished, send a message back to the UI thread
                PostMessage(hWnd, WM_APP + 1, 0, 0);
                }).detach();
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
    // This message is sent from the worker thread to update the progress bar
    case PBM_SETPOS:
    {
        SendMessage(hProgressBar, PBM_SETPOS, wParam, lParam);
    }
    break;
    // This custom message is sent from the worker thread when it's finished
    case WM_APP + 1:
    {
        EnableWindow(hProcessButton, TRUE); // Re-enable the process button
        SetWindowText(hStatusLabel, L"Status: Operation complete. Ready.");
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
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

// --- Helper function to display errors ---
void ShowError(HWND hWnd, const std::wstring& errorMessage)
{
    MessageBox(hWnd, errorMessage.c_str(), L"Error", MB_OK | MB_ICONERROR);
}


// --- Modified Crypto++ Implementations ---

void GenerateRSAKeys(HWND hWnd) {
    try {
        CryptoPP::AutoSeededRandomPool rng;
        PostMessage(hWnd, PBM_SETPOS, 25, 0);

        CryptoPP::RSA::PrivateKey privateKey;
        privateKey.GenerateRandomWithKeySize(rng, 2048);
        CryptoPP::RSA::PublicKey publicKey(privateKey);
        PostMessage(hWnd, PBM_SETPOS, 75, 0);

        SaveKey("rsa-private.key", privateKey);
        SaveKey("rsa-public.key", publicKey);

        PostMessage(hWnd, PBM_SETPOS, 100, 0);
    }
    catch (const CryptoPP::Exception& e) {
        std::wstringstream ws;
        ws << L"A cryptographic error occurred during key generation:\n\n" << e.what();
        ShowError(hWnd, ws.str());
    }
}

void EncryptRSA(const wchar_t* inputFile, HWND hWnd) {
    try {
        CryptoPP::RSA::PublicKey publicKey;
        LoadKey("rsa-public.key", publicKey);
        PostMessage(hWnd, PBM_SETPOS, 25, 0);

        std::ifstream file(inputFile, std::ios::binary);
        if (!file.is_open()) {
            ShowError(hWnd, L"Could not open the selected input file for encryption.");
            return;
        }

        std::ostringstream ss;
        ss << file.rdbuf();
        std::string plaintext = ss.str();
        PostMessage(hWnd, PBM_SETPOS, 50, 0);

        std::string ciphertext;
        CryptoPP::RSAES_OAEP_SHA_Encryptor e(publicKey);
        CryptoPP::AutoSeededRandomPool rng;

        CryptoPP::StringSource(plaintext, true,
            new CryptoPP::PK_EncryptorFilter(rng, e,
                new CryptoPP::StringSink(ciphertext)
            )
        );
        PostMessage(hWnd, PBM_SETPOS, 75, 0);

        std::ofstream out("encrypted.dat", std::ios::binary);
        out.write(ciphertext.c_str(), ciphertext.length());
        out.close();

        PostMessage(hWnd, PBM_SETPOS, 100, 0);
    }
    catch (const CryptoPP::Exception& e) {
        std::wstringstream ws;
        ws << L"A cryptographic error occurred during encryption:\n\n" << e.what();
        ShowError(hWnd, ws.str());
    }
}

void DecryptRSA(const wchar_t* inputFile, HWND hWnd) {
    try {
        CryptoPP::RSA::PrivateKey privateKey;
        LoadKey("rsa-private.key", privateKey);
        PostMessage(hWnd, PBM_SETPOS, 25, 0);

        std::ifstream file(inputFile, std::ios::binary);
        if (!file.is_open()) {
            ShowError(hWnd, L"Could not open the selected input file for decryption.");
            return;
        }
        std::ostringstream ss;
        ss << file.rdbuf();
        std::string ciphertext = ss.str();
        PostMessage(hWnd, PBM_SETPOS, 50, 0);

        std::string plaintext;
        CryptoPP::RSAES_OAEP_SHA_Decryptor d(privateKey);
        CryptoPP::AutoSeededRandomPool rng;

        CryptoPP::StringSource(ciphertext, true,
            new CryptoPP::PK_DecryptorFilter(rng, d,
                new CryptoPP::StringSink(plaintext)
            )
        );
        PostMessage(hWnd, PBM_SETPOS, 75, 0);

        wchar_t outputFile[260];
        GetWindowText(hOutputFileText, outputFile, 260);

        if (wcslen(outputFile) == 0) {
            ShowError(hWnd, L"Please select an output location using the 'Download' button before decrypting.");
            return;
        }

        std::ofstream out(outputFile, std::ios::binary);
        out.write(plaintext.c_str(), plaintext.length());
        out.close();

        PostMessage(hWnd, PBM_SETPOS, 100, 0);
    }
    catch (const CryptoPP::Exception& e) {
        std::wstringstream ws;
        ws << L"A cryptographic error occurred during decryption:\n\n" << e.what();
        ShowError(hWnd, ws.str());
    }
}

void GenerateDSASignature(const wchar_t* inputFile, HWND hWnd)
{
    try
    {
        CryptoPP::AutoSeededRandomPool rng;
        PostMessage(hWnd, PBM_SETPOS, 10, 0);

        CryptoPP::DSA::PrivateKey privateKey;
        privateKey.GenerateRandomWithKeySize(rng, 1024);
        CryptoPP::DSA::PublicKey publicKey;
        publicKey.AssignFrom(privateKey);
        PostMessage(hWnd, PBM_SETPOS, 30, 0);

        SaveKey("dsa-private.key", privateKey);
        SaveKey("dsa-public.key", publicKey);
        PostMessage(hWnd, PBM_SETPOS, 40, 0);

        std::ifstream file(inputFile, std::ios::binary);
        if (!file.is_open()) {
            ShowError(hWnd, L"Could not open the selected input file for signing.");
            return;
        }
        std::string message((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        PostMessage(hWnd, PBM_SETPOS, 60, 0);

        std::string signature;
        CryptoPP::DSA::Signer signer(privateKey);
        CryptoPP::StringSource(message, true,
            new CryptoPP::SignerFilter(rng, signer,
                new CryptoPP::StringSink(signature)
            )
        );
        PostMessage(hWnd, PBM_SETPOS, 80, 0);

        std::ofstream sigFile("signature.dat", std::ios::binary);
        sigFile.write(signature.c_str(), signature.size());
        sigFile.close();

        PostMessage(hWnd, PBM_SETPOS, 100, 0);
    }
    catch (const CryptoPP::Exception& e)
    {
        std::wstringstream ws;
        ws << L"A cryptographic error occurred during signature generation:\n\n" << e.what();
        ShowError(hWnd, ws.str());
    }
}

void VerifyDSASignature(const wchar_t* inputFile, HWND hWnd)
{
    try
    {
        CryptoPP::DSA::PublicKey publicKey;
        LoadKey("dsa-public.key", publicKey);

        std::ifstream sigFile("signature.dat", std::ios::binary);
        if (!sigFile.is_open()) {
            ShowError(hWnd, L"Could not open signature file 'signature.dat'.");
            return;
        }
        std::string signature((std::istreambuf_iterator<char>(sigFile)), std::istreambuf_iterator<char>());
        PostMessage(hWnd, PBM_SETPOS, 33, 0);

        std::ifstream file(inputFile, std::ios::binary);
        if (!file.is_open()) {
            ShowError(hWnd, L"Could not open the selected input file for verification.");
            return;
        }
        std::string message((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        PostMessage(hWnd, PBM_SETPOS, 66, 0);

        CryptoPP::DSA::Verifier verifier(publicKey);
        bool result = verifier.VerifyMessage(
            (const CryptoPP::byte*)message.c_str(), message.length(),
            (const CryptoPP::byte*)signature.c_str(), signature.length()
        );
        PostMessage(hWnd, PBM_SETPOS, 100, 0);

        if (result)
            MessageBox(hWnd, L"The signature is valid.", L"Verification Success", MB_OK | MB_ICONINFORMATION);
        else
            MessageBox(hWnd, L"The signature is NOT valid.", L"Verification Failed", MB_OK | MB_ICONWARNING);
    }
    catch (const CryptoPP::Exception& e)
    {
        std::wstringstream ws;
        ws << L"A cryptographic error occurred during signature verification:\n\n" << e.what();
        ShowError(hWnd, ws.str());
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