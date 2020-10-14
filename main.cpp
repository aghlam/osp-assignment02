#include <iostream>
#include <fstream>
#include <list>
#include <cstring>
#include <unistd.h>

using std::cout;
using std::endl;
using std::list;


struct Name {

    char* wptr;
    int size;

};


void createSbrkBlock(list<Name*> &memList, int wsize, const char* token);

bool bestFitStrategy(list<Name*> &allocMBList, list<Name*> &freedMBList, int wsize, const char* token);

bool combineMemoryBlock(list<Name*> &freedMBList);


// ------ Helper methods ------
void sortByMemAddress(list<Name*> &memList);

void sortByMemSize(list<Name*> &memList);

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

            } else if (counter >= 100 && ((counter+1) % 50 == 0)) { // For every 500

                if (!bestFitStrategy(allocMBList, freedMBList, wsize, token)) {
                    createSbrkBlock(allocMBList, wsize, token);
                }

            }

            ++counter;

            if (counter == 100) {

                
                // Testing - delete after
                cout << "Initial " << counter << " list" << endl;
                printMBList(allocMBList);

                int randomCount = 0;

                // Randomly move n to freed list
                while (randomCount < 50) {
                    
                    int random = rand() % allocMBList.size();
                    list<Name*>::iterator it = std::next(allocMBList.begin(), random);

                    freedMBList.push_back(*it);
                    allocMBList.remove(*it);

                    ++randomCount;
                }

                //Testing - delete after
                cout << "alloc list after splitting" << endl;
                printMBList(allocMBList);
                
                // Sort by address
                sortByMemAddress(freedMBList);

                // Testing - delete after
                cout << "free list before merging, size: " << freedMBList.size() << endl;
                printMBList(freedMBList);

                // Combining any contiguous blocks
                bool combined = true;
                while (combined) {
                    combined = combineMemoryBlock(freedMBList);
                }

                // Testing - delete after
                cout << "free list after merging, size: " << freedMBList.size() << endl;
                printMBList(freedMBList);

            }


        }

    }

    infile.close();

    for (Name* name : allocMBList) {
        delete name;
    }

    for (Name* name : freedMBList) {
        delete name;
    }

    return EXIT_SUCCESS;
}

// Find best fit
bool bestFitStrategy(list<Name*> &allocMBList, list<Name*> &freedMBList, int wsize, const char* token) {

    sortByMemSize(freedMBList); // Order the list by size

    //Testing - delet after
    // cout << "Freed list after sorting by size" << endl;
    // printMBList(freedMBList);

    list<Name*>::iterator it;

    for(it = freedMBList.begin(); it != freedMBList.end(); ++it) {

        // Since list is ordered by size, it should find the first wsize that fits
        if (wsize == (*it)->size) {
            strcpy((*it)->wptr, token); // Copy word over
            allocMBList.push_back(*it); // Copy to alloc list
            freedMBList.remove(*it); // Remove from free list
            return true;

        } else if (wsize < (*it)->size) {


            //Splitting logic here

            strcpy((*it)->wptr, token); // Copy word over
            allocMBList.push_back(*it); // Copy to alloc list
            freedMBList.remove(*it); // Remove from free list

            return true;
        }
    }

    return false;

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


// ------ Helper methods ------
// Method to sort by address
void sortByMemAddress(list<Name*> &memList) {
    memList.sort([](Name* a, Name* b) {
        return (void*)a->wptr < (void*)b->wptr;
    });
}

// Method to sort by size
void sortByMemSize(list<Name*> &memList) {
    memList.sort([](Name* a, Name* b) {
        return (uintptr_t)a->size < (uintptr_t)b->size;
    });
}

// For printing out the lists
void printMBList(list<Name*> memList) {

    for (Name* name : memList) {
        printf("Starting address: %p, Size: %d, Name: %s\n", name->wptr, name->size, (char *)name->wptr);
    }

    cout << endl;
}

