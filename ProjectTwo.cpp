#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// Structure to hold course information
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// Node structure for the binary search tree
struct Node {
    Course course;
    Node* left;
    Node* right;

    Node(Course aCourse) {
        course = aCourse;
        left = nullptr;
        right = nullptr;
    }
};

// Binary Search Tree class
class BinarySearchTree {
private:
    Node* root;

    void addNode(Node* node, Course course) {
        if (course.courseNumber < node->course.courseNumber) {
            if (node->left == nullptr) {
                node->left = new Node(course);
            }
            else {
                addNode(node->left, course);
            }
        }
        else {
            if (node->right == nullptr) {
                node->right = new Node(course);
            }
            else {
                addNode(node->right, course);
            }
        }
    }

    void inOrder(Node* node) {
        if (node != nullptr) {
            inOrder(node->left);

            cout << node->course.courseNumber << ", "
                 << node->course.courseTitle << endl;

            inOrder(node->right);
        }
    }

    void destroyTree(Node* node) {
        if (node != nullptr) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }

public:
    BinarySearchTree() {
        root = nullptr;
    }

    ~BinarySearchTree() {
        destroyTree(root);
    }

    void Insert(Course course) {
        if (root == nullptr) {
            root = new Node(course);
        }
        else {
            addNode(root, course);
        }
    }

    void PrintCourseList() {
        inOrder(root);
    }

    Course Search(string courseNumber) {
        Node* current = root;

        while (current != nullptr) {
            if (current->course.courseNumber == courseNumber) {
                return current->course;
            }
            else if (courseNumber < current->course.courseNumber) {
                current = current->left;
            }
            else {
                current = current->right;
            }
        }

        Course course;
        return course;
    }
};

// Splits one line of the file by commas
vector<string> splitLine(string line) {
    vector<string> tokens;
    string token;
    stringstream ss(line);

    while (getline(ss, token, ',')) {
        tokens.push_back(token);
    }

    return tokens;
}

// Checks if a course number exists in the course list
bool courseExists(vector<Course> courses, string courseNumber) {
    for (Course course : courses) {
        if (course.courseNumber == courseNumber) {
            return true;
        }
    }

    return false;
}

// Loads course data from file
bool loadCourses(string fileName, BinarySearchTree& courseTree) {
    ifstream inputFile(fileName);

    if (!inputFile.is_open()) {
        cout << "Error: Could not open file." << endl;
        return false;
    }

    vector<Course> courses;
    string line;

    while (getline(inputFile, line)) {
        vector<string> tokens = splitLine(line);

        // Each line must have at least course number and course title
        if (tokens.size() < 2) {
            cout << "Error: File format is incorrect." << endl;
            inputFile.close();
            return false;
        }

        Course course;
        course.courseNumber = tokens.at(0);
        course.courseTitle = tokens.at(1);

        // Any remaining tokens are prerequisites
        for (unsigned int i = 2; i < tokens.size(); ++i) {
    string prerequisite = tokens.at(i);

    // Remove spaces, tabs, and carriage returns from the prerequisite
    prerequisite.erase(remove(prerequisite.begin(), prerequisite.end(), ' '), prerequisite.end());
    prerequisite.erase(remove(prerequisite.begin(), prerequisite.end(), '\t'), prerequisite.end());
    prerequisite.erase(remove(prerequisite.begin(), prerequisite.end(), '\r'), prerequisite.end());

    if (!prerequisite.empty()) {
        course.prerequisites.push_back(prerequisite);
    }
}

        courses.push_back(course);
    }

    inputFile.close();

    // Validate that all prerequisites exist as courses in the file
    for (Course course : courses) {
        for (string prerequisite : course.prerequisites) {
            if (!courseExists(courses, prerequisite)) {
                cout << "Error: Prerequisite " << prerequisite
                     << " does not exist as a course in the file." << endl;
                return false;
            }
        }
    }

    // Insert courses into the binary search tree
    for (Course course : courses) {
        courseTree.Insert(course);
    }

    return true;
}

// Prints information for one course
void printCourseInformation(BinarySearchTree& courseTree, string courseNumber) {
    Course course = courseTree.Search(courseNumber);

    if (course.courseNumber.empty()) {
        cout << "Course not found." << endl;
        return;
    }

    cout << course.courseNumber << ", " << course.courseTitle << endl;

    cout << "Prerequisites: ";

    if (course.prerequisites.empty()) {
        cout << "None";
    }
    else {
        for (unsigned int i = 0; i < course.prerequisites.size(); ++i) {
            Course prerequisiteCourse = courseTree.Search(course.prerequisites.at(i));

            cout << prerequisiteCourse.courseNumber << " "
                 << prerequisiteCourse.courseTitle;

            if (i < course.prerequisites.size() - 1) {
                cout << ", ";
            }
        }
    }

    cout << endl;
}

int main() {
    BinarySearchTree courseTree;
    bool dataLoaded = false;
    int choice = 0;
    string fileName;
    string courseNumber;

    cout << "Welcome to the course planner." << endl;

    while (choice != 9) {
        cout << endl;
        cout << "Menu:" << endl;
        cout << "  1. Load Data Structure" << endl;
        cout << "  2. Print Course List" << endl;
        cout << "  3. Print Course" << endl;
        cout << "  9. Exit" << endl;
        cout << "What would you like to do? ";

        cin >> choice;

        switch (choice) {
        case 1:
            cout << "Enter file name: ";
            cin >> fileName;

            dataLoaded = loadCourses(fileName, courseTree);

            if (dataLoaded) {
                cout << "Course data loaded successfully." << endl;
            }

            break;

        case 2:
            if (!dataLoaded) {
                cout << "Please load the course data first." << endl;
            }
            else {
                cout << "Here is a sample schedule:" << endl;
                courseTree.PrintCourseList();
            }

            break;

        case 3:
            if (!dataLoaded) {
                cout << "Please load the course data first." << endl;
            }
            else {
                cout << "What course do you want to know about? ";
                cin >> courseNumber;

                // Convert user input to uppercase for easier matching
                transform(courseNumber.begin(), courseNumber.end(), courseNumber.begin(), ::toupper);

                printCourseInformation(courseTree, courseNumber);
            }

            break;

        case 9:
            cout << "Thank you for using the course planner!" << endl;
            break;

        default:
            cout << choice << " is not a valid option." << endl;
            break;
        }
    }

    return 0;
}