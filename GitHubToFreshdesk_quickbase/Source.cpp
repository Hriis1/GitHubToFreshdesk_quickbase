#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

string getGitHubUser(string username, const string& authKey) {
    //string command = "curl -s https://api.github.com/users/" + username;
    string command = "curl -s -H \"Authorization: token " + authKey + "\" https://api.github.com/users/" + username;
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
    std::cout << "Enter a GitHub authorization key: " << std::endl;
    string gitHubAuthKey = "";
    std::cin >> gitHubAuthKey;
    string githubInfo = getGitHubUser(username,gitHubAuthKey);
    std::cout << githubInfo;
    
    // Parse the GitHub user information
    string name = extractFieldValueBetter(githubInfo, "name");
    string publicRepos = extractFieldValueBetter(githubInfo, "public_repos");
    string id = extractFieldValueBetter(githubInfo, "id");
    string email = extractFieldValueBetter(githubInfo, "email");
    //this code is just for testing in case the given github user doesn't have a public email
    if (email == "null")
    {
        email = "noemail@example.com";
    }

    // Send the HTTP request to create the new contact
    string url = "https://" + freshdeskDomain + ".freshdesk.com/api/v2/contacts";
    string FRESHDESK_TOKEN = "uvKDJ2EufRJVFSxOqpy";

    bool addTUpdateF =true;
    std::cout << "1 for add 0 for update" << std::endl;
    std::cin >> addTUpdateF;
    if (addTUpdateF) {
        string command = R"(curl -v -u )" + FRESHDESK_TOKEN + R"(:X -H "Content-Type: application/json" -d "{\"name\":\")" + name + R"(\", \"email\":\")" + email + R"(\"}" "https://)" + freshdeskDomain + R"(.freshdesk.com/api/v2/contacts")";

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
    }
    else
    {
        string contactId = "0";
        std::cout << "Enter the contact Id of the contact you want to update" << std::endl;
        std::cin >> contactId;
        string command = R"(curl -v -u )" + FRESHDESK_TOKEN + R"(:X -H "Content-Type: application/json" -d "{\"name\":\")" + name + R"(\", \"email\":\")" + email + R"(\"}" -X PUT "https://)" + freshdeskDomain + R"(.freshdesk.com/api/v2/contacts/)" + contactId;

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
        if (result.find("\"status\": \"success\"") == string::npos) {
            cout << "Error: " << result << endl;
            return 1;
        }

        cout << "Contact updated successfully." << endl;

        return 0;

    }

    string name = extractFieldValue(data, "login");
    string id = extractFieldValue(data, "node_id");
    string avatar_url = extractFieldValue(data, "avatar_url");

    // Print the retrieved information
    cout << "Name: " << name << endl;
    cout << "id: " << id << endl;
    cout << "avatar_url: " << avatar_url << endl;

    return 0;

}

