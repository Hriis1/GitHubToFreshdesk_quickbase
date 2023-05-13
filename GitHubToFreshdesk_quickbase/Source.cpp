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

string extractFieldValueBetter(const string& input, const string& fieldName)
{
    string search = "\""+ fieldName +"\": ";
    size_t start = input.find(search) + search.length();
    size_t end = input.find(",", start);
    string output = input.substr(start, end - start);
    if (output[0] == '\"')
    {
        output.erase(output.begin());
        output.pop_back();
    }

    return output;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <GitHub username> <Freshdesk domain>" << endl;
        return 1;
    }

    string username = argv[1];
    string freshdeskDomain = argv[2];

    // Retrieve GitHub user information
    string githubInfo = getGitHubUser(username);
    std::cout << githubInfo;
    
    // Parse the GitHub user information to extract the email address
    // (Assuming the email address is in the "email" field of the JSON response)
    string name = extractFieldValueBetter(githubInfo, "name");
    string publicRepos = extractFieldValueBetter(githubInfo, "public_repos");
    string id = extractFieldValueBetter(githubInfo, "id");
    string email = extractFieldValueBetter(githubInfo, "email");
    //this code is just for testing in case the given github user doesn't have an email
    if (email == "null")
    {
        email = "testmain@example.com";
    }

    // Create the JSON payload for the new contact
    string payload = "{\"name\": \"" + username + "\", \"email\": \"" + email + "\"}";

    // Send the HTTP request to create the new contact
    string url = "https://" + freshdeskDomain + ".freshdesk.com/api/v2/contacts";
    string FRESHDESK_TOKEN = "uvKDJ2EufRJVFSxOqpy";
    string command = "curl -s -X POST -u \"" + FRESHDESK_TOKEN + ":X\" -H \"Content-Type: application/json\" -d '" + payload + "' \"" + url + "\"";
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
    cout << "API response:\n" << result << endl;
    if (result.find("\"status\": \"created\"") == string::npos) {
        cout << "Error: " << result << endl;
        return 1;
    }

    cout << "Contact created successfully." << endl;

    return 0;
}

