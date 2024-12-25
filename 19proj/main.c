#include <stdio.h>  // Provides input-output functions like printf, fopen, fgets, etc.
#include <stdlib.h> // Provides functions like abs(), exit(), etc.
#include <string.h> // Provides string manipulation functions like strlen(), strcpy(), strcat(), etc.
#include <ctype.h> // Provides character handling functions like toupper(), isprint(), etc.

// Function Prototypes
int convertRomanToDecimal(char *roman); // Converts a Roman numeral to its decimal equivalent.
int executeOperation(int operand1, int operand2, char operator); // Performs arithmetic operations on two numbers.
void convertNumberToWords(int num, char *output); // Converts a number to its English word representation.
void handleInputFile(const char *inputPath, const char *outputPath); // Processes input from a file and writes results to another file.
void writeResult(FILE *outFile, int result);  // Writes the result in words to the output file.
int isValidRomanNumeral(char *roman); // Validates if a string is a valid Roman numeral.
void sanitizeInput(char *str);  // Cleans input strings to remove unwanted characters.

// Main function
int main() {
    const char *inputPath = "Input.txt"; // Path to the input file.
    const char *outputPath = "Output.txt"; // Path to the output file.

    handleInputFile(inputPath, outputPath); // Processes the input and generates the output.

    printf("Processing complete. Check %s for results.\n", outputPath); // Notify user of completion.

    return 0;
}

// Function to convert Roman numeral to decimal
int convertRomanToDecimal(char *roman) {
    int total = 0, current = 0, next = 0; // Initialize total and temporary variables.
    int length = strlen(roman); // Length of the Roman numeral string.

    for (int i = 0; i < length; i++) { // Iterate over the Roman numeral string.
        switch (toupper(roman[i])) {   // Match the current character with its Roman value.
            case 'I': current = 1; break;
            case 'V': current = 5; break;
            case 'X': current = 10; break;
            case 'L': current = 50; break;
            case 'C': current = 100; break;
            case 'D': current = 500; break;
            case 'M': current = 1000; break;
            default: return -1; // Return -1 if an invalid character is found.
        }
        if (i + 1 < length) { // Check the next character if it exists.
            switch (toupper(roman[i + 1])) {
                case 'I': next = 1; break;
                case 'V': next = 5; break;
                case 'X': next = 10; break;
                case 'L': next = 50; break;
                case 'C': next = 100; break;
                case 'D': next = 500; break;
                case 'M': next = 1000; break;
                default: next = 0; // Default value for invalid characters.
            }
        } else {
            next = 0; // If there is no next character, set next to 0.
        }

        if (current < next) { // If current value is less than next, subtract it (e.g., IV = 4).
            total -= current;
        } else { // Otherwise, add it to the total.
            total += current;
        }
    }
    return total; // Return the computed total.
}

// Function to perform arithmetic operations
int executeOperation(int operand1, int operand2, char operator) {
    switch (operator) { // Perform the operation based on the operator.
        case '+': return operand1 + operand2;
        case '-': return operand1 - operand2;
        case '*': return operand1 * operand2;
        case '/': return (operand2 != 0) ? operand1 / operand2 : -1; // Prevent division by zero.
        default: return -1; // Return -1 for invalid operators.
    }
}

// Function to convert number to words
void convertNumberToWords(int num, char *output) {
    const char *ones[] = {"", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine"}; // Words for single-digit numbers.
    const char *teens[] = {"Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen", "Sixteen", "Seventeen", "Eighteen", "Nineteen"}; // Words for teen numbers.
    const char *tens[] = {"", "Ten", "Twenty", "Thirty", "Forty", "Fifty", "Sixty", "Seventy", "Eighty", "Ninety"}; // Words for tens.
    const char *places[] = {"", "Thousand", "Million", "Billion"}; // Place value suffixes.

    if (num == 0) { // Handle zero explicitly.
        strcpy(output, "Zero");
        return;
    }

    char buffer[256] = "";
    char temp[256] = "";
    int isNegative = (num < 0); // Check if the number is negative.
    num = abs(num); // Work with the absolute value.

    int chunk, place = 0;
     // Break number into chunks of 1000 and convert each to words.
    while (num > 0) {
        chunk = num % 1000;  // Extract the last 3 digits.

        if (chunk > 0) {
            char chunkWords[256] = "";

            if (chunk / 100 > 0) {
                strcat(chunkWords, ones[chunk / 100]);
                strcat(chunkWords, " Hundred ");
            }

            int remainder = chunk % 100;
            if (remainder > 10 && remainder < 20) {
                strcat(chunkWords, teens[remainder - 11]);
            } else {
                if (remainder / 10 > 0) {
                    strcat(chunkWords, tens[remainder / 10]);
                    strcat(chunkWords, " ");
                }
                if (remainder % 10 > 0) {
                    strcat(chunkWords, ones[remainder % 10]);
                }
            }

            if (place > 0) {
                strcat(chunkWords, " ");
                strcat(chunkWords, places[place]);
            }

            snprintf(temp, sizeof(temp), "%s %s", chunkWords, buffer);
            strcpy(buffer, temp);
        }

        num /= 1000;
        place++;
    }

    snprintf(output, 256, "%s%s", isNegative ? "Negative " : "", buffer);
    output[strcspn(output, "\n")] = '\0';
}

// Function to handle input and output files
void handleInputFile(const char *inputPath, const char *outputPath) {
    FILE *input = fopen(inputPath, "r"); // Open input file in read mode.
    FILE *output = fopen(outputPath, "w"); // Open output file in write mode.

    if (!input || !output) { // Check if files opened successfully.
        printf("Error opening file(s).\n");
        exit(1);
    }

    char line[256];
    while (fgets(line, sizeof(line), input)) { // Read each line from input.
        sanitizeInput(line); // Clean the line of unwanted characters.
        char roman1[50], roman2[50], operator;
        int num1, num2, result;

        if (sscanf(line, "%49s %c %49s", roman1, &operator, roman2) == 3) {
            if (!isValidRomanNumeral(roman1) || !isValidRomanNumeral(roman2)) {
                fprintf(output, "Invalid Roman numeral in line: %s\n", line);
                continue;
            }

            num1 = convertRomanToDecimal(roman1);
            num2 = convertRomanToDecimal(roman2);

            result = executeOperation(num1, num2, operator);

            if (result == -1) {
                fprintf(output, "Invalid operation in line: %s\n", line);
            } else {
                writeResult(output, result); // Write result in words to output.
            }
        } else {
            fprintf(output, "Invalid input format in line: %s\n", line);
        }
    }

    fclose(input);
    fclose(output);
}

// Function to validate Roman numerals
int isValidRomanNumeral(char *roman) {
    for (int i = 0; roman[i]; i++) { // Validate each character.
        char c = toupper(roman[i]);
        if (c != 'I' && c != 'V' && c != 'X' && c != 'L' && c != 'C' && c != 'D' && c != 'M') {
            return 0; // Return false if an invalid character is found.
        }
    }
    return 1; // Return true if all characters are valid.
}

// Function to sanitize input strings
void sanitizeInput(char *str) {
    char *ptr = str;
    while (*ptr) { // Iterate through the string.
        if (isprint(*ptr) || isspace(*ptr)) {
            ptr++;
        } else {
            *ptr = '\0'; // Remove non-printable characters.
            break;
        }
    }
}

// Function to write the result to output file
void writeResult(FILE *outFile, int result) {
    char resultInWords[256];
    convertNumberToWords(result, resultInWords); // Convert result to words.
    fprintf(outFile, "%s\n", resultInWords); // Write result in words to file.
}
