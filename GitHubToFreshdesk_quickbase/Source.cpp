#include <windows.h>
#include <iostream>
#include <string>
#include <regex>

using namespace std;

string getGitHubUser(string username) {
    string command = "curl -s https://api.github.com/users/" + username;
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    string result = "";
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    _pclose(pipe);
    return result;
}

string extractFieldValue(const string& input, const string& fieldName) {
    regex pattern("\"" + fieldName + "\": ?\"(.+?)\"");
    smatch match;
    if (regex_search(input, match, pattern)) {
        return match[1];
    }
    else {
        return "";
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <GitHub username>" << endl;
        return 1;
    }

    string username = argv[1];

    // Retrieve GitHub user information
    string githubInfo = getGitHubUser(username);

    // Parse the GitHub user information to extract the email address
    // (Assuming the email address is in the "email" field of the JSON response)
    size_t emailStart = githubInfo.find("\"email\": \"") + 10;
    size_t emailEnd = githubInfo.find("\",", emailStart);
    string email = githubInfo.substr(emailStart, emailEnd - emailStart);

    // Create the JSON payload for the new contact
    string payload = "{\"name\": \"" + username + "\", \"email\": \"" + email + "\"}";

    // Send the HTTP request to create the new contact
    string url = "https://yourcompany.freshdesk.com/api/v2/contacts";
    string FRESHDESK_TOKEN = "api-key";
    string command = "curl -s -X POST -u \"" + FRESHDESK_TOKEN + "\" -H \"Content-Type: application/json\" -d '" + payload + "' \"" + url + "\"";
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) return 1;
    char buffer[128];
    string result = "";
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    _pclose(pipe);

    // Check if the HTTP request was successful
    if (result.find("\"status\": \"created\"") == string::npos) {
        cout << "Error: " << result << endl;
        return 1;
    }

    cout << "Contact created successfully." << endl;

    return 0;
}

