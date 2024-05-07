# STUDENT GRADE MANAGEMENT SYSTEM

This project is a student grade management system implemented in the C programming language. The system allows users to manage student grades stored in a file. It utilizes process creation with the `fork()` system call to handle file operations concurrently.

## Project Structure

The project is organized into three main modules:

1. main.c: Contains the main function which orchestrates user interaction and command execution.
2. cmdHandler.c: Responsible for parsing user input and dispatching commands.
3. cmdProcess.c: Implements the functionality of each command.

## Sample Commands and Functionality

1. gtuStudentGradesCMD: Displays all available commands and their descriptions.

    * Implemented with a new process using fork() for execution.
    * Output formatted to be suitable for writing to the log file.

2. gtuStudentGradesCMD with filename parameter: Creates a file with the provided name.

    * Filename validation ensures only entries with .txt extension are accepted.
    * File creation is executed in a child process.
    * Error conditions in file operations are handled gracefully.

3. addStudentGradeCMD: Adds a new student's grade to the file.

    * Checks if the student name already exists in the file to update the grade if necessary.
    * New entries are appended to the file using "name surname" "grade" format.

4. searchStudentCMD: Searches for a student's grade by name.

    * Reads all lines in the file and searches for the specified name.
    * Prints the information if found; otherwise, informs that it was not found.

5. sortAllCMD: Sorts all student grades in the file based on user-selected criteria.

    * Original file remains unchanged; sorted entries are displayed on the screen.

6. showAllCMD: Displays all student grades stored in the file.

    * Prints all entries stored in the tokens array.

7. listGradesCMD: Displays the first 5 entries in the file.

    * Prints the first 5 elements of the tokens array.

8. listSomeCMD: Displays entries based on page index and amount.

    * Converts the tokens array into a "book" format, with a specified number of lines per page.

9. logToFile: Logs command inputs, execution results, and error explanations to log.log.

## Usage

1. **Compiling and Running:**

    ```bash
    gcc main.c cmdHandler.c cmdProcess -o System
    ./System
    ```
    or 

    ```bash
    make
    ```

2. **Commands:**
    
    - **gtuStudentGrades "grades.txt"** : Create a file with name grades.txt

    - **addStudentGrade "Name Surname" "AA"**: Append a new student's grade to the file.
    
    - **searchStudent "Name Surname"**: Search for a student's grade by name.
    
    - **sortAll "grades.txt"**:  Sort student grades in the file. (with four selection)
    
    - **showAll "grades.txt"**: Display all student grades stored in the file. 
    
    - **listGrades "grades.txt"**: Display first five entry in the file.

    - **listSome X Y "grades.txt"**: Divides the number of lines in the file by X, where each X is a page. Y. prints the contents of the page in the index to the screen.

    - **gtuStudentGrades**: Display all available commands and instructions.


## Points to be Considered

* A separate process is used for logging to ensure logging operations do not interfere with command execution.
* Error handling is comprehensive to handle various scenarios gracefully.
* Tokenization and array manipulation functions are defined to facilitate command processing and file operations.

## Example Usage

```bash

Enter (q) for quit.
gtuStudentGrades "grades.txt"
addStudentGrade "ahmet ozdemir" "AA" "grades.txt"
showAll "grades.txt"
"ahmet ozdemir" "AA"
addStudentGrade "erkan zergeroglu" "FF" "grades.txt"
sortAll "grades.txt"
Sort by:
1 - Student
2 - Grade
2
Sort method:
1 - Ascending
2 - Descending
2
"erkan zergeroglu" "FF"
"ahmet ozdemir" "AA"
q


```

* If you have any further information or questions about this sample, please feel free to get in touch. Good coding! 🚀