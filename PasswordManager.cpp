/*
  Password Manager
  Author: Fatimah
  Description:
  A secure C++ console-based password manager that uses a master password,
  SHA-256 hashing, random password generation, and encrypted file storage.
*/

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <ctime>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <cryptopp/filters.h>

using namespace std;
using namespace CryptoPP;

class PasswordManager {
private:
  string masterPasswordHash; // Stores hashed master password
  vector <string> usernames; // Stores usernames
  vector <string> passwords; // Stored passwords (plain or hashed as chosen)
  const string FILENAME = "passwords.txt"; // File to store credentials

  string hashPassword(const string& password); // Hashes the password
  string generateRandomPassword(int length = 16); // Generates a random password
  string encrypt(const string& data);                   // Simple reversible "encryption"
  string decrypt(const string& data);                   // Matching decryption

public:
  PasswordManager(); //Concstructor to load existing credentials
  bool setMasterPassword(const string& password); // Sets the master password
  bool authenticate(const string& password); // Authenticates the master password
  void resetMasterPassword(); // Resets the master password
  void addCredentials(const string& username); // Stores a new usernames with a generated password
  string getCredentials(const string& username); // Retrieves the generated password for the username
  void deleteCredentials(const string& username); //Removes stored credentials for the given usename
  void saveToFile(); // Saves cresentials to a file
  void loadFromFile(); // Loads credentials from a file
  void displayMenu(); // Displas the menu for the user
};


//Constructor loads saved credentials
PasswordManager::PasswordManager()
{
  loadFromFile();
}


//Hashing function using SHA-256 (for master password)
string PasswordManager::hashPassword(const string& password)
{
  string hash;
  SHA256 sha256; //Create a SHA-256 hash object
  StringSource(password, true, //Use the SHA-256 hash object to encrypt the inputted password
    new HashFilter(sha256, new HexEncoder(new StringSink(hash))));// Convert hash to hexadecimal
  return hash;
}

// Simple symmetric "encryption" using XOR with masterPasswordHash as key material
string PasswordManager::encrypt(const string& data) 
{
    if (masterPasswordHash.empty()) return data; // fallback
    string result = data;
    for (size_t i = 0; i < data.size(); i++) {
        result[i] = data[i] ^ masterPasswordHash[i % masterPasswordHash.size()];
    }
    return result;
}

string PasswordManager::decrypt(const string& data) 
{
    // XOR with same key reverses the operation
    return encrypt(data);
}

//Random Password Generator - Creates a random password
string PasswordManager::generateRandomPassword(int length)
{
  const string characters = //Password will be generated from these set of charcters
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "!@#$%^&*()-_=+[]{}|;:,.<>?";

  if (length < 8) length = 8; // basic security minimum

  string password;
  static bool seeded = false;
    if (!seeded) 
    {
        srand(static_cast<unsigned int>(time(nullptr)));
        seeded = true;
    }

  for (int i = 0; i < length; i++)
  {
    size_t index = rand() % characters.size();
    password += characters[index];
  }
  return password; // Return a password with mixed characters
}

//Establishes the master password
bool PasswordManager::setMasterPassword(const string& password)
{
  if (masterPasswordHash.empty())
  {
    masterPasswordHash = hashPassword(password); // Hash the entered password and save it as the master password
    cout << "Master password set successfully." << endl;
    saveToFile(); // Persist master password
    return true;
  }
  cout << "Master password is already set." << endl;
  return false;
}

// Verifies if stored master password matches entered master password
bool PasswordManager::authenticate(const string& password)
{
  if (masterPasswordHash.empty()) {
        cout << "No master password set. Please set it first." << endl;
        return false;
    }
  return hashPassword(password) == masterPasswordHash;
}

// Allows resetting the master password
void PasswordManager::resetMasterPassword() {
    if (masterPasswordHash.empty()) {
        cout << "No master password set yet." << endl;
        return;
    }

    string oldPassword, newPassword;
    cout << "Enter current master password: ";
    cin >> oldPassword;

    if (!authenticate(oldPassword)) {
        cout << "Incorrect master password. Cannot reset." << endl;
        return;
    }

    cout << "Enter new master password: ";
    cin >> newPassword;

    masterPasswordHash = hashPassword(newPassword);
    cout << "Master password reset successfully." << endl;
    saveToFile();
}

//Stores the username with a generated password
void PasswordManager::addCredentials(const string& username)
{
   if (username.empty()) {
        cout << "Username cannot be empty." << endl;
        return;
    }
    if (username.length() < 3) {
        cout << "Username too short. Minimum length is 3 characters." << endl;
        return;
    }

    int length;
    cout << "Enter desired password length (minimum 8): ";
    cin >> length;
    if (!cin || length < 8) {
        cout << "Invalid or too short length. Using default length 16." << endl;
        cin.clear();
        string temp;
        getline(cin, temp);
        length = 16;
    }

    string newPassword = generateRandomPassword(length); // Generates a random password
    usernames.push_back(username);                   // Store the username
    passwords.push_back(newPassword);                // Store the password (plain text here)

    cout << "Generated password for " << username << ": " << newPassword << endl;
    saveToFile(); // Save updated credentials to file
}

//Retrieves a generated password for a given username
string PasswordManager::getCredentials(const string& username)
{
  for (size_t i =0; i < usernames.size(); i++) //Loop through the stored username
  {
    if (usernames[i] == username)
    {
    return passwords[i]; // Return stored password
    }
  }
  return "User not found"; // Return error message if username is not found
}

// Removes stored credentials for a given username
void PasswordManager:: deleteCredentials(const string& username)
{
  for (size_t i=0; i < usernames.size(); i++) // Loop through the stored usernames
  {
    if (usernames[i] == username)
    {
      // Remove the username and corresponding password from the vectors
      usernames.erase(usernames.begin() + i);
      passwords.erase(passwords.begin() + i);
      cout << "Credentials for " << username << " deleted." << endl;
      saveToFile();
      return;
    }
  }
 cout << "Username not found." << endl;
}

//Saves credentails to a file
void PasswordManager::saveToFile()
{
  ofstream file(FILENAME, ios::binary); //Open file for writing
  if (!file)
  {
    cout << "Error! Could not open file for saving." << endl; //Display error message if file cannot open
    return;
  }

  // First line: master password hash (if set)
  if (!masterPasswordHash.empty()) {
        file << "MASTER:" << masterPasswordHash << endl;
    } else {
        file << "MASTER:" << "NONE" << endl;
    }

  // Save each username and encrypted password to file using a clear delimiter
  for (size_t i=0; i < usernames.size(); i++) // Save each username and hashed password to file
  {
     string encUser = encrypt(usernames[i]);
     string encPass = encrypt(passwords[i]);
     file << "USER:" << encUser << ":PASS:" << encPass << endl;
  }

  file.close();
  cout << "Credentials saved successfully to file." << endl; // Display success message after saving is complete and file is closed
}

//Loads credentails from a file
void PasswordManager::loadFromFile()
{
  ifstream file(FILENAME, ios::binary); //Open file to read
  if (!file)
  {
    cout << "Error! No existing credentials found. Starting fresh." << endl;
    return;
  }

  usernames.clear(); // Clear existing usernames
  passwords.clear(); // Clear existing passwords

  string line;
    while (getline(file, line)) {
        if (line.rfind("MASTER:", 0) == 0) {
            // Line starts with "MASTER:"
            string value = line.substr(7);
            if (value != "NONE") {
                masterPasswordHash = value;
            }
        } else if (line.rfind("USER:", 0) == 0) {
            // Expected format: USER:<username>:PASS:<password>
            size_t userStart = 5;
            size_t passLabelPos = line.find(":PASS:");
            if (passLabelPos != string::npos) 
            {
               string encUsername = line.substr(userStart, passLabelPos - userStart);
               string encPassword = line.substr(passLabelPos + 6);
               string username = decrypt(encUsername);
               string password = decrypt(encPassword);
               usernames.push_back(username);
               passwords.push_back(password);
            }
        }
    }

  file.close();
  cout << "Credentials loaded successfully from file." << endl;
}

// Provides an interactive menu for the user
void PasswordManager::displayMenu()
{
  int option = 0;
  string username, password;

  cout << "=============================\n";
  cout << "   Secure Password Manager   \n";
  cout << "=============================\n";

  if(masterPasswordHash.empty()) // Prompt user yo set master password, or check if its already set
  {
    cout << "No master password set. Please set a master password: ";
    cin >> password;
    setMasterPassword(password);
  }

  cout << "Enter master password to continue: "; // Prompt user to enter master password again to authenticate the master password
  cin >> password;

  if (!authenticate(password)) // Exit program if authentication fails
  {
    cout << "Incorrect master password. Exiting." << endl;
    return;
  }

do
{
  // Display title and menu options
  cout << endl << endl << "Password Manager Menu" << endl << endl;
  cout << "1. Add Credentials" << endl;
  cout << "2. Get Credentials" << endl;
  cout << "3. Delete Credentials" << endl;
  cout << "4. Save to File" << endl;
  cout << "5. Load from File" << endl;
  cout << "6. Reset Master Password" << endl;
  cout << "7.Exit" << endl;
  cout << "Enter your option: " << endl;
  cin >> option;

  switch (option)
  {
    case 1: // Enter a username and add credentials for it
      cout << "Enter username ";
      cin >> username;
      addCredentials(username);
      break;

    case 2: // Enter a username and get the crdentials for that specific username
      cout << "Enter username: ";
      cin >> username;
      cout << "Retrieved password: " << getCredentials(username) << endl;
      break;

    case 3: //Enter a username and delete its credentials
      cout << "Enter username: ";
      cin >> username;
      deleteCredentials(username);
      break;

    case 4: // Save credentials to a file
      saveToFile();
      break;

    case 5: // Load credentials from a file
      loadFromFile();
      break;

    case 6: // Reset master password
      resetMasterPassword();
      break;

    case 7: // Exit the program
      cout << "Exiting Password Manager....." << endl;
      break ;

    default: // Handle invalid option
      cin.clear();
            {
                string temp;
                getline(cin, temp);
            }
            cout << "Invalid input. Please enter a valid option from 1 to 7." << endl;
  }


 } while (option !=7);
}

//Main function
int main ()
{
  PasswordManager manager;// Create a PasswordManager object
  manager.displayMenu(); // Call menu function
  return 0; //Exit the program successfully
}
