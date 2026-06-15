# Password-Manager
A C++ console‑based password manager that uses a master password, SHA‑256 hashing, random password generation, and encrypted file storage.

This is a secure, console-based Password Manager written in C++.  
It allows users to store, retrieve, and delete login credentials while protecting them with a master password.

## Key Features
- Master password secured using SHA‑256 hashing  
- Random password generator with customizable length  
- Simple XOR-based encryption for stored usernames and passwords  
- Secure storage in a local file (`passwords.txt`)  
- Add, view, and delete credentials  
- Reset the master password  
- Automatic loading and saving of encrypted data  

## How It Works
When the program starts, it loads previously saved credentials and prompts the user to authenticate using the master password.  
All usernames and passwords are encrypted before being written to the file and decrypted when retrieved.

## Screenshot
![Program Screenshot](password_manager_screenshot.png)

## Technologies Used
- C++  
- Crypto++ library (SHA‑256 hashing, hex encoding)  
- File I/O for persistent storage  

## How to Run
1. Compile the program:  
   `g++ PasswordManager.cpp -o PasswordManager`
2. Run it:  
   `./PasswordManager`


## Future Improvements
- Add stronger encryption (AES instead of XOR)
- Add a graphical user interface (GUI)
- Store credentials in a secure database instead of a text file
- Add password strength validation
- Add unit tests for reliability
