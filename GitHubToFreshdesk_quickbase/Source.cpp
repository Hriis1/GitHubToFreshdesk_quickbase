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
   //std::cout << githubInfo;
    
    // Parse the GitHub user information
    string name = extractFieldValueBetter(githubInfo, "name");
    string publicRepos = extractFieldValueBetter(githubInfo, "public_repos");
    string id = extractFieldValueBetter(githubInfo, "id");
    string email = extractFieldValueBetter(githubInfo, "email");
    //this code is just for testing in case the given github user doesn't have an email
    if (email == "null")
    {
        email = "testmain@example.com";
    }

    // Send the HTTP request to create the new contact
    string url = "https://" + freshdeskDomain + ".freshdesk.com/api/v2/contacts";
    string FRESHDESK_TOKEN = "uvKDJ2EufRJVFSxOqpy";
    //string command = "curl -u " + FRESHDESK_TOKEN + ":X -H \"Content-Type: application/json\" -X POST -d '{ \"name\" : \"" + name + "\", \"email\" : \"" + email + "\"}' 'https://" + freshdeskDomain + ".freshdesk.com/api/v2/contacts'";
    //string command = "curl -u uvKDJ2EufRJVFSxOqpy -H \"Content - Type: application / json\" -X POST -d ‘{ “name” : “Clark Gosho”, “email” : “superman@freshdesk.com”, “custom_fields” : { “department” : “Superhero” } }’ 'https://test12334.freshdesk.com/api/v2/contacts’";
    //string command = "curl -v -u uvKDJ2EufRJVFSxOqpy:X -H 'Content-Type: application/json' -X POST -d '{ \"name\":\"Super Man\", \"email\":\"superman@freshdesk.com\"}' 'https://test12334.freshdesk.com/api/v2/contacts'";
    //string command = "curl -v -u uvKDJ2EufRJVFSxOqpy:X -H \"Content-Type: application/json\" -X POST -d '{ \"name\":\"Super Man\", \"email\":\"superman@freshdesk.com\"}' 'https://test12334.freshdesk.com/api/v2/contacts'";
    //string command = "curl -v -u uvKDJ2EufRJVFSxOqpy:X -H \"Content-Type: application/json\" -d '{ \"name\":\"Super Man\", \"email\":\"superman@freshdesk.com\" }' 'https://test12334.freshdesk.com/api/v2/contacts'";
    //string command = "curl -v -u uvKDJ2EufRJVFSxOqpy:X -H \"Content-Type: application/json\" -d '{ \"name\":\"Super Man\", \"email\":\"superman@freshdesk.com\" }' 'https://test12334.freshdesk.com/api/v2/contacts.json'";
    //string command = "curl -v -u uvKDJ2EufRJVFSxOqpy:X -H \"Content-Type: application/json\" -d '{ \"name\":\"Super Man\", \"email\":\"superman@freshdesk.com\" }' 'https://test12334.freshdesk.com'";
    //string command = R"(curl -v -u uvKDJ2EufRJVFSxOqpy:X -H "Content-Type: application/json" -d '{ "name":"Super Man", "email":"superman@freshdesk.com" }' 'https://test12334.freshdesk.com/api/v2/contacts')";
    //string command = R"(curl -v -u uvKDJ2EufRJVFSxOqpy:X -H "Content-Type: application/json" -d "{"name":"Super Man", "email":"superman@freshdesk.com"}" "https://test12334.freshdesk.com/api/v2/contacts")";
    
    //string commandTest = R"(curl -v -u uvKDJ2EufRJVFSxOqpy:X -H "Content-Type: application/json" -d "{\"name\":\"Spider Man\", \"email\":\"Spiderman@freshdesk.com\"}" "https://test12334.freshdesk.com/api/v2/contacts")";


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

    return 0;

}

