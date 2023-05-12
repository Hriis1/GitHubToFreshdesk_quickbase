#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <streambuf>
#include <regex>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <Windows.h>

using namespace std;

string retrieveDataFromGitHubAPI(const string& username) {
    // Set up the URL to retrieve user information
    string url = "https://api.github.com/users/" + username;

    // Set up the curl command to retrieve user information
    string curlCmd = "curl -s -H \"Accept: application/vnd.github.v3+json\" \"" + url + "\"";

    // Create a pipe to capture the output of the curl command
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    HANDLE pipeRead, pipeWrite;
    if (!CreatePipe(&pipeRead, &pipeWrite, &sa, 0)) {
        return "";
    }

    // Set up the process information for the curl command
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdError = pipeWrite;
    si.hStdOutput = pipeWrite;
    si.dwFlags |= STARTF_USESTDHANDLES;
    ZeroMemory(&pi, sizeof(pi));

    // Start the curl process and wait for it to finish
    if (!CreateProcessA(NULL, (LPSTR)curlCmd.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        return "";
    }
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Read the output of the curl command from the pipe
    CloseHandle(pipeWrite);
    string output;
    char buffer[256];
    DWORD bytesRead;
    while (ReadFile(pipeRead, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead != 0) {
        output.append(buffer, bytesRead);
    }
    CloseHandle(pipeRead);

    return output;
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

int main(int argc, char* argv[])
{
    // Check if a username was provided
    if (argc < 2) {
        cout << "Please provide a username as a command line argument." << endl;
        return 1;
    }

    // Retrieve user information from GitHub API
    string username = argv[1];
    string data = retrieveDataFromGitHubAPI(username);

    // Parse the response
    string name = extractFieldValue(data, "login");
    string id = extractFieldValue(data, "node_id");
    string avatar_url = extractFieldValue(data, "avatar_url");

    // Print the retrieved information
    cout << "Name: " << name << endl;
    cout << "id: " << id << endl;
    cout << "avatar_url: " << avatar_url << endl;

    return 0;
}
