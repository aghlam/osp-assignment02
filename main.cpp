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


/**
 * Uses First Fit Strategy to allocate new elements into memory
 * @param allocMBList reference allocMBList to store used memory pointers
 * @param freedMBList reference freedMBList to store free memory pointers
 * @param wsize int size of the element to be stored
 * @param token pointer to the element to be stored
 * @return true if successfully allocated
*/
bool firstFitStrategy(list<Name*> &allocMBList, list<Name*> &freedMBList, int wsize, const char* token);

/**
 * Uses Best Fit Strategy to allocate new elements into memory
 * After sorting the freedMBList from smallest to largest memory size it calls on 
 * firstFitStrategy method to allocate as it uses the same logic.
 * @param allocMBList reference allocMBList to store used memory pointers
 * @param freedMBList reference freedMBList to store free memory pointers
 * @param wsize int size of the element to be stored
 * @param token pointer to the element to be stored
 * @return true if successfully allocated
*/
bool bestFitStrategy(list<Name*> &allocMBList, list<Name*> &freedMBList, int wsize, const char* token);

/**
 * Uses Worst Fit Strategy to allocate new elements into memory.
 * After sorting the freedMBList from largest to smallest memory size it calls on 
 * firstFitStrategy method to allocate as it uses the same logic.
 * @param allocMBList reference allocMBList to store used memory pointers
 * @param freedMBList reference freedMBList to store free memory pointers
 * @param wsize int size of the element to be stored
 * @param token pointer to the element to be stored
 * @return true if successfully allocated
*/
bool worstFitStrategy(list<Name*> &allocMBList, list<Name*> &freedMBList, int wsize, const char* token);

/**
 * Allocates new block of memory using sbrk() and adds it to list
 * @param memList reference list to add new element
 * @param wsize int size of memory block to be created
 * @param token pointer to element to be stored
*/
void createSbrkBlock(list<Name*> &memList, int wsize, const char* token);

/**
 * Used to combine any memory that are adjacent to each other in the list
 * @param freedMBList list of elements to combine
 * @return false if there are no more elements to combine
*/
bool combineMemoryBlock(list<Name*> &freedMBList);

/**
 * Splits element into smaller memory blocks to allocate and saves them to the lists after allocation
 * @param allocMBList reference list to save new element/memory block
 * @param freedMBList reference list to save split memory block
 * @param it iterator location of element to be split
 * @param wsize int size of memory block to be stored
 * @param token pointer to element to be stored
*/
void splitMemoryBlock(list<Name*> &allocMBList, list<Name*> &freedMBList, list<Name*>::iterator it, int wsize, const char* token);

/**
 * Sorts list based on memory address
 * @param memList reference list to be sorted
*/
void sortByMemAddress(list<Name*> &memList);

/**
 * Sorts list based on smallest to largest memory size
 * @param memList reference list to be sorted
*/
void sortByMemSizeSmallToLarge(list<Name*> &memList);

/**
 * Sorts list based on largest to smallest memory size
 * @param memList reference list to be sorted
*/
void sortByMemSizeLargeToSmall(list<Name*> &memList);

/**
 * Randomly removes elements from one list to another
 * @param allocMBList reference allocMBList to remove from
 * @param freedMBList reference freedMBList to move to
 * @param count int number of elements to remove
*/
void randomRemoval(list<Name*> &allocMBList, list<Name*> &freedMBList, int count);

/**
 * Outputs data to a CSV file
 * @param filename string name of file to be output to
 * @param total int total amount of memory used
*/
void outputCSV(list<Name*> allocMBList, list<Name*> freedMBList, string filename, int total);

/**
 * Finds the total memory used by adding the size in each list
 * @param allocMBList allocMBList to be added
 * @param freedMBList freedMBList to be added
 * @return int total of memory size
*/
int findTotalSize(list<Name*> allocMBList, list<Name*> freedMBList);

/**
 * Help for testing and prints out list
 * @param memList list to be printed out
*/
void printMBList(list<Name*> memList);


int main(int argc, char** argv) {

    // Seeding random generator
    srand((unsigned)time(0));

    std::ifstream infile;

    list<Name*> allocMBList;
    list<Name*> freedMBList;

    if (argc != 4) {
        cout << "Error - requires three arguments to run." << endl;
        cout << "Run program with the input: ./main <strategy> <input> <output>" << endl;
        cout << "<scheduler> selection: -first | -best | -worst" << endl;
        cout << "<input>: Name of input file including extension" << endl;
        cout << "<output>: Name of output file including extension" << endl;
        return EXIT_FAILURE;

    }

    if (argv[1] != string("-best") && argv[1] != string("-first") && argv[1] != string("-worst")) {
        cout << "Error - strategy input doesn't exist" << endl;
        cout << "Please use: -first | -best | -worst" << endl;
        return EXIT_FAILURE;
    }

    string outputFileName = argv[3];

    infile.open(argv[2]);

    if (infile.fail()) {
        cout << "Error - could not open input file" << endl;
        cout << "Did you forget the extension?" << endl;
        infile.close();
        return EXIT_FAILURE;

    } else {

        int counter = 0;

        string line;
        const char* token;

        // Start allocating to lists
        while (!infile.eof()) {

            std::getline(infile, line);
            token = line.c_str();

            size_t wsize = strlen(token) + 1;

            if (counter < 1000) {
                createSbrkBlock(allocMBList, wsize, token);

            } else if (counter >= 1000) {

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

            if (counter % 1000 == 0) {
                // Remove randomly
                randomRemoval(allocMBList, freedMBList, 500);
                
                // Sort by address
                sortByMemAddress(freedMBList);

                // Combining any contiguous blocks
                bool combined = true;
                while (combined) {
                    combined = combineMemoryBlock(freedMBList);
                }

            } 

        }

    }

    infile.close();

    sortByMemAddress(freedMBList);
    sortByMemAddress(allocMBList);

    int total = findTotalSize(allocMBList, freedMBList);

    outputCSV(allocMBList, freedMBList, outputFileName, total);
    cout << "Data output to " << outputFileName << endl;

    // Clean up
    sbrk(-total);

    for (Name* name : allocMBList) {
        delete name;
    }

    for (Name* name : freedMBList) {
        delete name;
    }

    return EXIT_SUCCESS;
}


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

bool bestFitStrategy(list<Name*> &allocMBList, list<Name*> &freedMBList, int wsize, const char* token) {

    sortByMemSizeSmallToLarge(freedMBList); // Order the list by size from smallest to largest

    return firstFitStrategy(allocMBList, freedMBList, wsize, token);

}

bool worstFitStrategy(list<Name*> &allocMBList, list<Name*> &freedMBList, int wsize, const char* token) {

    sortByMemSizeLargeToSmall(freedMBList); // Order the list by size from largest to smallest

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

void sortByMemAddress(list<Name*> &memList) {
    memList.sort([](Name* a, Name* b) {
        return (void*)a->wptr < (void*)b->wptr;
    });
}

void sortByMemSizeSmallToLarge(list<Name*> &memList) {
    memList.sort([](Name* a, Name* b) {
        return (uintptr_t)a->size < (uintptr_t)b->size;
    });
}

void sortByMemSizeLargeToSmall(list<Name*> &memList) {
    memList.sort([](Name* a, Name* b) {
        return (uintptr_t)a->size > (uintptr_t)b->size;
    });
}

void randomRemoval(list<Name*> &allocMBList, list<Name*> &freedMBList, int count) {

    int randomCount = 0;

    while (randomCount < count) {
        
        int random = rand() % allocMBList.size();
        list<Name*>::iterator it = std::next(allocMBList.begin(), random);

        freedMBList.push_back(*it);
        allocMBList.remove(*it);

        ++randomCount;
    }
}

void outputCSV(list<Name*> allocMBList, list<Name*> freedMBList, string filename, int total) {

    std::ofstream outfile;

    // Output CSV file to filename
    outfile.open(filename);

    outfile << "Total memory:," << total << endl; 
    outfile << endl;

    outfile << "allocMBList" << endl;
    outfile << "Address," << "Size," << "Content" << endl;

    for (Name* name : allocMBList) {
        outfile << (void*)name->wptr << "," 
                << name->size << "," 
                << name->wptr 
                << endl;
    }

    outfile << endl;

    outfile << "freedMBList" << endl;
    outfile << "Address," << "Size" << endl;

    for (Name* name : freedMBList) {
        outfile << (void*)name->wptr << "," 
                << name->size
                << endl;
    }

    outfile.close();

}

int findTotalSize(list<Name*> allocMBList, list<Name*> freedMBList) {

    int a = 0, b = 0;

    for(Name* name : allocMBList) {
        a += name->size;
    }

    for(Name* name : freedMBList) {
        b += name->size;
    }
    
    return a + b;

}

void printMBList(list<Name*> memList) {

    for (Name* name : memList) {
        printf("Starting address: %p, Size: %d, Name: %s\n", name->wptr, name->size, (char *)name->wptr);
    }

    cout << endl;
}
