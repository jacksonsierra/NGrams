/**
 * File: Ngrams.cpp
 * --------------
 * Implements Ngrams.
 */

//#include <cctype>
//#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include "console.h"
#include "filelib.h"
#include "hashmap.h"
#include "vector.h"
#include "simpio.h"
#include "random.h"
using namespace std;

//Constants
static const int kSTOP_NUMBER = 0;

//Prototypes
static void welcome();
static void buildMapNgrams(ifstream& file, HashMap< Vector<string>, Vector<string> >& mapNgrams, const int& numberOfNgrams);
static void readInFile(ifstream& file, Vector<string>& wordsInFile);
static void addKeyValuesToMap(HashMap< Vector<string>, Vector<string> >& mapNgrams, const Vector<string> wordsInFile, const int numberOfNgrams);
static Vector<string> chooseRandomStartingPoint(const HashMap< Vector<string>, Vector<string> >& mapNgrams);
static Vector<string> generateRandomSentence(const HashMap< Vector<string>, Vector<string> >& mapNgrams, const Vector<string>& startKey, const int numberOfNgrams, const int numberOfRandomWords);
static void printRandomSentence(const Vector<string> randomSentence);

/**
 * Function: main
 * The main welcomes a user and continues to generate random sentences
 * until the user opts out. To do this, it reads in a file at user's
 * request, asks for number of Ngrams for which the user would like to have
 * sentences generated, determines a random point in which to start
 * generating a sentence, and ultimately prints out the result.
 */
int main() {
    ifstream file;
    int numberOfNgrams;
    HashMap< Vector<string>, Vector<string> > mapNgrams;

    welcome();
    promptUserForFile(file, "Input file? ");
    while(true) {
        numberOfNgrams = getInteger("Value of N? ");
        if(numberOfNgrams > kSTOP_NUMBER) break;
        cout << "N must be 1 or greater." << endl;
    }
    buildMapNgrams(file, mapNgrams, numberOfNgrams);
    while(true) {
        int numberOfRandomWords = getInteger("\n# of random words to generate (0 to quit)? ");
        if(numberOfRandomWords <= kSTOP_NUMBER) break;
        Vector<string> startKey = chooseRandomStartingPoint(mapNgrams);
        Vector<string> randomSentence = generateRandomSentence(mapNgrams, startKey, numberOfNgrams, numberOfRandomWords);
        printRandomSentence(randomSentence);
    }
    cout << "Exiting." << endl;
    return 0;
}

/**
 * Procedure: welcome()
 * Prints out a greeting on a couple of lines.
 */
static void welcome() {
    cout << "Welcome to CS 106X Random Writer ('N-Grams')." << endl;
    cout << "This program makes random text based on a document." << endl;
    cout << "Give me an input file and an 'N' value for groups" << endl;
    cout << "of words, and I'll create random text for you" << endl << endl;
}

/**
 * Procedure: buildMapNgrams
 * This procedure builds a map for each word in the given file
 * as a key, and the associated values that follow that word in the text.
 * It does so by employing two helper functions and a number of parameters by
 * reference.  readInFile reads all the words of the file into a Vector<string>;
 * addKeyValuesToMap creates a map using the words from readInFile's Vector<string>.
 * All parameters are passed by reference from the main.
 * Parameters: file stream, HashMap of words in files and those that succeed them,
 * user-inputted number of Ngrams
 */
static void buildMapNgrams(ifstream& file, HashMap< Vector<string>, Vector<string> >& mapNgrams, const int& numberOfNgrams) {
    Vector<string> wordsInFile;
    readInFile(file, wordsInFile);
    addKeyValuesToMap(mapNgrams, wordsInFile, numberOfNgrams);
}

/**
 * Procedure: buildMapNgrams
 * This procedure builds a map for each N-gram in the given file
 * as a key, and the associated values that follow that N-gram in the text.
 * It does so by employing two helper functions and a number of parameters by
 * reference.  readInFile reads all the words of the file into a Vector<string>;
 * addKeyValuesToMap creates a map using the words from readInFile's Vector<string>.
 * All parameters are passed by reference from the main.
 * Parameters: file stream, HashMap of words in files and those that succeed them,
 * user-inputted number of Ngrams
 */
static void readInFile(ifstream& file, Vector<string>& wordsInFile) {
    string value;
    while(file >> value) {
        wordsInFile.add(value);
    }
    file.close();
}

/**
 * Procedure: addKeyValuesToMap
 * Main helper function to buildMapNgrams that constructs the succeeding words
 * after any given N-gram in the file read in.  It does so by looping through
 * each word in the file and constructing an N-gram stored in a Vector<string>
 * based on that reference in the file, additionally checking for word wrapping.
 * The subsequent words following that N-gram are stored in another Vector<string>,
 * since the same N-gram can appear multiple times. For each loop/N-gram, the
 * key and associated value are added put into a HashMap passed by reference.
 * Parameters: N-gram to subsequent word(s) HashMap by reference since this function alters it,
 * the associated words of the file in a constant Vector<string> that doesn't change,
 * and the constant number of user-defined Ngrams
 */
static void addKeyValuesToMap(HashMap< Vector<string>, Vector<string> >& mapNgrams, const Vector<string> wordsInFile, const int numberOfNgrams) {
    for(int wordNumber = 0; wordNumber < wordsInFile.size(); wordNumber++) {  //Loops through each word in file
        Vector<string> mapKeys;
        Vector<string> mapValues;

        for(int indexNgram = 0; indexNgram < numberOfNgrams - 1; indexNgram++) { //Loops through the N-1 window
            int keyInFile = (wordNumber + indexNgram) % wordsInFile.size();  //Location of the respective word in the N-1 window, wrapping if necessary
            mapKeys.add(wordsInFile[keyInFile]);  //Adds N-1 word to map of keys for HashMap
        }
        int valueInFile = (wordNumber + (numberOfNgrams - 1)) % wordsInFile.size();  //Location of the succeeding word after the N-gram
        if(mapNgrams.containsKey(mapKeys)) {  //Checks if N-gram already appeared in file
            mapValues = mapNgrams.get(mapKeys);  //Gets values associated with the key representing the already seen N-gram
        }
        mapValues.add(wordsInFile[valueInFile]);  //Adds new value to Vector<string> of map values
        mapNgrams.put(mapKeys,mapValues);  //Combines map key and map value into the existing HashMap
    }
}

/**
 * Function: chooseRandomStartingPoint
 * Helper function that returns a random starting point in the given file.
 * Because the file contents and associated N-grams and values have already been
 * pushed to a non-indexed HashMap, we have to use a range-based for loop
 * and a counter along with RandomInteger to get a random starting point/N-gram.
 * This function returns the corresponding N-gram starting point, which is housed
 * as a Vector<string>.
 * Parameters: N-gram to subsequent word(s) HashMap as a constant reference since we're just getting
 * a starting point
 */
static Vector<string> chooseRandomStartingPoint(const HashMap< Vector<string>, Vector<string> >& mapNgrams) {
    Vector<string> startKey;
    int counter = 0;
    int randomNumber = randomInteger(0, mapNgrams.size() - 1);
    for(Vector<string> i: mapNgrams) {
        counter++;
        if(counter == randomNumber) {
            startKey = i;
            break;
        }
    }
    return startKey;
}

/**
 * Function: generateRandomSentence
 * This function combines together the rest of the program via
 * reference parameters and returns a random sentence to the main.
 * A while loop operates until the number of words randomly generated
 * meets the user's requirement.  Within the while, a for loop
 * grabs the associated N-gram randomly determined previously, and gets
 * the potential values that can succeed the N-gram.  One such value
 * is determined at random, added to the running sentence, and the final
 * output is returned as a Vector<string>
 * Parameters: N-gram to subsequent word HashMap by constant reference,
 * randomly generated start key, which itself is an N0gram, the user-defined
 * number of N-grams (in theory could get this from size of startKey),
 * number of random words the user wants to generate by constant
 */
static Vector<string> generateRandomSentence(const HashMap< Vector<string>, Vector<string> >& mapNgrams,
                                            const Vector<string>& startKey, const int numberOfNgrams, const int numberOfRandomWords) {
    int numberOfWordsGenerated = startKey.size();  //Initializes sentence based on size of N-gram
    Vector<string> randomSentence = startKey;  //Random sentence is stored in a Vector<string> with a word at each address
    while(numberOfWordsGenerated < numberOfRandomWords) {  //Continue word generation until sentence is long enough
        Vector<string> currentWindow;
        for(int i = 0; i < numberOfNgrams - 1; i++) {  //For each member of the N-1 window
            string currentWord = randomSentence[numberOfWordsGenerated - (numberOfNgrams - 1) + i]; //Get the current N-gram
            currentWindow.add(currentWord);  //Add the current N-gram to a Vector<string>, since that's what the HashMap keys are
        }
        Vector<string> possibleNextWords = mapNgrams.get(currentWindow);  //Return all the possibilities for subsequent words given the N-gram from the HashMap
        string nextWord = possibleNextWords[randomInteger(0, possibleNextWords.size() - 1)];  //Randomly choose which word to add to the sentence
        randomSentence.add(nextWord);  //Add the randomly generated word to your running sentence
        numberOfWordsGenerated++;  //Increment counter for while condition
    }
    return randomSentence;
}

/**
 * Function: printRandomSentence
 * This function prints all the words housed in the Vector<string> that
 * represents the random sentence.
 */
static void printRandomSentence(const Vector<string> randomSentence) {
    cout << "...";
    for(string word: randomSentence) {
        cout << ' ' << word;
    }
    cout << "..." << endl;
}
