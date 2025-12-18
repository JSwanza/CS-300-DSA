#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <cctype>
#include <algorithm>
#include <limits>
#include <ios>

using namespace std;

// Course structure to hold course data
struct Course {
    string courseNumber;
    string title;
    vector<string> prerequisites;
};

// BST Node structure
struct BSTNode {
    Course data;
    BSTNode* left;
    BSTNode* right;
    BSTNode(const Course& c) : data(c), left(nullptr), right(nullptr) {}
};

// Trim function to remove leading/trailing whitespace
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

// Validate course number format: 4 uppercase letters + 3 digits
bool isValidCourseNumber(const string& num) {
    if (num.length() != 7) return false;
    for (int i = 0; i < 4; ++i) {
        if (!isupper(static_cast<unsigned char>(num[i]))) return false;
    }
    for (int i = 4; i < 7; ++i) {
        if (!isdigit(static_cast<unsigned char>(num[i]))) return false;
    }
    return true;
}

// Insert a course into the BST (sorted by courseNumber)
void insertBST(BSTNode*& node, const Course& course) {
    if (node == nullptr) {
        node = new BSTNode(course);
        return;
    }
    if (course.courseNumber < node->data.courseNumber) {
        insertBST(node->left, course);
    }
    else if (course.courseNumber > node->data.courseNumber) {
        insertBST(node->right, course);
    }
    // Ignore duplicates
}

// Search for a course in the BST, return pointer to Course if found
Course* searchBST(BSTNode* node, const string& target) {
    if (node == nullptr) return nullptr;
    if (node->data.courseNumber == target) return &(node->data);
    if (target < node->data.courseNumber) {
        return searchBST(node->left, target);
    }
    else {
        return searchBST(node->right, target);
    }
}

// In-order traversal to print sorted list
void inOrderTraversal(BSTNode* node) {
    if (node == nullptr) return;
    inOrderTraversal(node->left);
    cout << node->data.courseNumber << ", " << node->data.title << endl;
    inOrderTraversal(node->right);
}

// Print single course information and prerequisites
void PrintSingleCourse(BSTNode* root, const string& target, const string& originalInput) {
    Course* course = searchBST(root, target);
    if (course == nullptr) {
        cout << "Course not found: " << originalInput << endl;
        return;
    }
    cout << course->courseNumber << ", " << course->title << endl;
    if (course->prerequisites.empty()) {
        // Sample doesn't show "No prerequisites", so omit
    }
    else {
        cout << "Prerequisites: ";
        for (size_t i = 0; i < course->prerequisites.size(); ++i) {
            cout << course->prerequisites[i];
            if (i < course->prerequisites.size() - 1) {
                cout << ", ";
            }
        }
        cout << endl;
    }
}

// Recursively delete BST to free memory
void deleteTree(BSTNode* node) {
    if (node == nullptr) return;
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}

// Load courses from CSV file into BST with validation
void LoadCourses(const string& filename, BSTNode*& root, bool& loaded) {
    if (loaded) {
        cout << "Already loaded" << endl;
        return;
    }

    set<string> allCourseNumbers;
    vector<string> lines;
    ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        cout << "Error: Cannot open " << filename << endl;
        return;
    }

    string line;
    while (getline(inputFile, line)) {
        string trimmedLine = trim(line);
        if (trimmedLine.empty() || (trimmedLine.size() >= 2 && trimmedLine[0] == '/' && trimmedLine[1] == '/')) {
            continue;
        }
        lines.push_back(trimmedLine);

        // First pass: collect course numbers
        istringstream iss(trimmedLine);
        string token;
        if (getline(iss, token, ',')) {
            token = trim(token);
            if (!token.empty()) {
                allCourseNumbers.insert(token);
            }
        }
    }
    inputFile.close();

    int errorCount = 0;
    int loadedCount = 0;

    // Second pass: parse and validate
    for (const string& l : lines) {
        istringstream iss(l);
        vector<string> tokens;
        string token;
        while (getline(iss, token, ',')) {
            tokens.push_back(trim(token));
        }

        if (tokens.size() < 2) {
            cout << "Format Error: " << l << endl;
            ++errorCount;
            continue;
        }

        string courseNumber = tokens[0];
        if (!isValidCourseNumber(courseNumber)) {
            cout << "Invalid course number: " << courseNumber << endl;
            ++errorCount;
            continue;
        }

        string title = tokens[1];
        vector<string> prerequisites;
        for (size_t i = 2; i < tokens.size(); ++i) {
            string prereq = tokens[i];
            if (prereq.empty()) continue;
            if (allCourseNumbers.find(prereq) == allCourseNumbers.end()) {
                cout << "Prereq Error for " << courseNumber << ": " << prereq << " invalid" << endl;
                ++errorCount;
            }
            else {
                prerequisites.push_back(prereq);
            }
        }

        Course newCourse{ courseNumber, title, prerequisites };
        insertBST(root, newCourse);
        ++loadedCount;
    }

    cout << "Loaded " << loadedCount << " courses with " << errorCount << " errors" << endl;
    loaded = true;
}

// Main program with menu
int main() {
    cout << "Welcome to the course planner." << endl << endl;

    BSTNode* root = nullptr;
    bool loaded = false;

    while (true) {
        cout << "1. Load Data Structure." << endl;
        cout << "2. Print Course List." << endl;
        cout << "3. Print Course." << endl;
        cout << "9. Exit" << endl;
        cout << "What would you like to do? ";
        int choice;
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice" << endl;
            continue;
        }

        switch (choice) {
        case 1: {
            string filename;
            cout << "Enter filename: ";
            cin >> filename;
            LoadCourses(filename, root, loaded);
            break;
        }
        case 2:
            if (!loaded) {
                cout << "Load data first" << endl;
            }
            else {
                cout << "Here is a sample schedule:" << endl;
                inOrderTraversal(root);
            }
            break;
        case 3:
            if (!loaded) {
                cout << "Load data first" << endl;
            }
            else {
                string targetCourse;
                string originalInput;
                cout << "What course do you want to know about? ";
                cin >> targetCourse;
                originalInput = targetCourse;
                // Convert to uppercase for search (courses are uppercase)
                transform(targetCourse.begin(), targetCourse.end(), targetCourse.begin(),
                    [](unsigned char c) { return toupper(c); });
                PrintSingleCourse(root, targetCourse, originalInput);
            }
            break;
        case 9:
            cout << "Thank you for using the course planner!" << endl;
            deleteTree(root);
            return 0;
        default:
            cout << choice << " is not a valid option." << endl;
        }
        cout << endl;  // Extra newline for cleaner output spacing
    }

    return 0;
}
