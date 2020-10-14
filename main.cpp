#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <unistd.h>


using std::cout;
using std::endl;
using std::list;
using std::string;


struct Name {

    char* wptr;
    int size;

};

void createSbrkBlock(list<Name*> &memList, int wsize, const char* token);

bool firstFitStrategy(list<Name*> &allocMBList, list<Name*> &freedMBList, int wsize, const char* token);

bool bestFitStrategy(list<Name*> &allocMBList, list<Name*> &freedMBList, int wsize, const char* token);

bool worstFitStrategy(list<Name*> &allocMBList, list<Name*> &freedMBList, int wsize, const char* token);

bool combineMemoryBlock(list<Name*> &freedMBList);

void splitMemoryBlock(list<Name*> &allocMBList, list<Name*> &freedMBList, list<Name*>::iterator it, int wsize, const char* token);


// ------ Helper methods ------
void sortByMemAddress(list<Name*> &memList);

void sortByMemSizeSmallToLarge(list<Name*> &memList);

void sortByMemSizeLargeToSmall(list<Name*> &memList);

void randomRemoval(list<Name*> &allocMBList, list<Name*> &freedMBList, int count);

void printMBList(list<Name*> memList);
// ----------------------------


int main(int argc, char** argv) {

    // Seeding random generator
    srand((unsigned)time(0));

    std::ifstream infile;

    list<Name*> allocMBList;
    list<Name*> freedMBList;

    if (argc != 3) {
        cout << "Error - requires two arguments to run." << endl;
        cout << "Run program with the input: ./main <strategy> <filename>" << endl;
        cout << "<scheduler> selection: -first | -best | -worst" << endl;
        cout << "<filename>: Name of file including extension" << endl;
        return EXIT_FAILURE;
    }

    infile.open(argv[2]);

    if (infile.fail()) {
        cout << "Error - could not open file" << endl;
        cout << "Did you forget the extension?" << endl;
        return EXIT_FAILURE;

    } else {

        // Start allocating to list
        int counter = 0;

        std::string line;
        const char* token;

        while (!infile.eof()) {

            std::getline(infile, line);
            token = line.c_str();

            size_t wsize = strlen(token) + 1;

            if (counter < 100) {

                createSbrkBlock(allocMBList, wsize, token);

            // } else if (counter >= 100 && ((counter+1) % 100 == 0)) { // For every 500
            } else if (counter >= 100) {

                if (argv[1] == string("-first")) {
                    if (!firstFitStrategy(allocMBList, freedMBList, wsize, token)) {
                        createSbrkBlock(allocMBList, wsize, token);
                    }

                } else if (argv[1] == string("-best")) {
                    if (!bestFitStrategy(allocMBList, freedMBList, wsize, token)) {
                        createSbrkBlock(allocMBList, wsize, token);
                    }

                } else if (argv[1] == string("-worst")) {
                    if (!worstFitStrategy(allocMBList, freedMBList, wsize, token)) {
                        createSbrkBlock(allocMBList,wsize, token);
                    }

                } 

            }

            ++counter;

            if (counter == 100) {
                // Remove 
                randomRemoval(allocMBList, freedMBList, 50);
                
                // Sort by address
                sortByMemAddress(freedMBList);

                // Combining any contiguous blocks


            } else if (counter > 100 && counter % 100 == 0) {

                // cout << "alloc listl, size:  " << allocMBList.size() << endl;
                // printMBList(allocMBList);

                // cout << "free list, size: " << freedMBList.size() << endl;
                // printMBList(freedMBList);
                // Remove
                randomRemoval(allocMBList, freedMBList, 50);

                // Sort by address
                sortByMemAddress(freedMBList);

                // Combine
                bool combined = true;
                while (combined) {
                    combined = combineMemoryBlock(freedMBList);
                }

            }

        }

    }

    cout << "Final alloc list, size: " << allocMBList.size() << endl;
    printMBList(allocMBList);

    cout << "Final freed list, size: " << freedMBList.size() << endl;
    printMBList(freedMBList);

    infile.close();

    for (Name* name : allocMBList) {
        delete name;
    }

    for (Name* name : freedMBList) {
        delete name;
    }

    return EXIT_SUCCESS;
}

// Find first fit
bool firstFitStrategy(list<Name*> &allocMBList, list<Name*> &freedMBList, int wsize, const char* token) {

    list<Name*>::iterator it;

    for (it = freedMBList.begin(); it != freedMBList.end(); ++it) {
        if (wsize == (*it)->size) {
            strcpy((*it)->wptr, token);
            allocMBList.push_back(*it);
            freedMBList.remove(*it);

            return true;

        } else if (wsize < (*it)->size) {
            splitMemoryBlock(allocMBList, freedMBList, it, wsize, token);
            return true;
        }
    }

    return false;
}

// Find best fit
bool bestFitStrategy(list<Name*> &allocMBList, list<Name*> &freedMBList, int wsize, const char* token) {

    sortByMemSizeSmallToLarge(freedMBList); // Order the list by size smallest to largest

    return firstFitStrategy(allocMBList, freedMBList, wsize, token);

}

// Find worst fit
bool worstFitStrategy(list<Name*> &allocMBList, list<Name*> &freedMBList, int wsize, const char* token) {

    sortByMemSizeLargeToSmall(freedMBList);

    return firstFitStrategy(allocMBList, freedMBList, wsize, token);

}

void createSbrkBlock(list<Name*> &memList, int wsize, const char* token) {

    void* request;
    request = sbrk(wsize);
    strcpy((char*)request, token);

    Name* name = new Name();
    name->size = wsize;
    name->wptr = (char*)request;
    memList.push_back(name);

}

bool combineMemoryBlock(list<Name*> &freedMBList) {

    for (unsigned int i = 0; i < freedMBList.size()-1; ++i) {

        list<Name*>::iterator it1 = std::next(freedMBList.begin(), i);
        list<Name*>::iterator it2 = std::next(freedMBList.begin(), i+1);

        if ((*it1)->wptr + (*it1)->size == (*it2)->wptr) {
            (*it1)->size += (*it2)->size;

            freedMBList.remove(*it2);

            return true;
        }

    }

    return false;
}

void splitMemoryBlock(list<Name*> &allocMBList, list<Name*> &freedMBList, list<Name*>::iterator it, int wsize, const char* token) {

    // Create new struct
    Name* newName = new Name();

    // Split and add to free list
    newName->wptr = &(*it)->wptr[wsize];
    newName->size = (*it)->size - wsize;
    freedMBList.push_back(newName);

    // Allocate, add to alloc list, remove from freed list
    strcpy((*it)->wptr, token);
    (*it)->size = wsize;
    
    allocMBList.push_back(*it);
    freedMBList.remove(*it);

}


// ------ Helper methods ------
// Method to sort by address
void sortByMemAddress(list<Name*> &memList) {
    memList.sort([](Name* a, Name* b) {
        return (void*)a->wptr < (void*)b->wptr;
    });
}

// Method to sort by size - smallest to largest
void sortByMemSizeSmallToLarge(list<Name*> &memList) {
    memList.sort([](Name* a, Name* b) {
        return (uintptr_t)a->size < (uintptr_t)b->size;
    });
}

// Method to sort by size - largest to smallest
void sortByMemSizeLargeToSmall(list<Name*> &memList) {
    memList.sort([](Name* a, Name* b) {
        return (uintptr_t)a->size > (uintptr_t)b->size;
    });
}

void randomRemoval(list<Name*> &allocMBList, list<Name*> &freedMBList, int count) {

    int randomCount = 0;

    // Randomly move n to freed list
    while (randomCount < count) {
        // cout << "This random removal works: " << randomCount << endl;
        
        int random = rand() % allocMBList.size();
        list<Name*>::iterator it = std::next(allocMBList.begin(), random);

        freedMBList.push_back(*it);
        allocMBList.remove(*it);

        ++randomCount;
    }
}

// For printing out the lists
void printMBList(list<Name*> memList) {

    for (Name* name : memList) {
        printf("Starting address: %p, Size: %d, Name: %s\n", name->wptr, name->size, (char *)name->wptr);
    }

    cout << endl;
}

