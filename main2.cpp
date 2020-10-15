#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <unistd.h>
#include <memory>

using std::cout;
using std::endl;
using std::list;
using std::string;
using std::shared_ptr;
using std::make_shared;


struct Name {

    Name(int size, char* wptr):
        size(size),
        wptr(wptr)
    {}

    int size;
    char* wptr;

};


bool firstFitStrategy(list<shared_ptr<Name>> &allocMBList, list<shared_ptr<Name>> &freedMBList, int wsize, const char* token);

bool bestFitStrategy(list<shared_ptr<Name>> &allocMBList, list<shared_ptr<Name>> &freedMBList, int wsize, const char* token);

bool worstFitStrategy(list<shared_ptr<Name>> &allocMBList, list<shared_ptr<Name>> &freedMBList, int wsize, const char* token);

void createSbrkBlock(list<shared_ptr<Name>> &memList, int wsize, const char* token);

bool combineMemoryBlock(list<shared_ptr<Name>> &freedMBList);

void splitMemoryBlock(list<shared_ptr<Name>> &allocMBList, list<shared_ptr<Name>> &freedMBList, list<shared_ptr<Name>>::iterator it, int wsize, const char* token);

void randomRemoval(list<shared_ptr<Name>> &allocMBList, list<shared_ptr<Name>> &freedMBList, int count);

void sortByMemAddress(list<shared_ptr<Name>> &memList);

void sortByMemSizeSmallToLarge(list<shared_ptr<Name>> &memList);

void sortByMemSizeLargeToSmall(list<shared_ptr<Name>> &memList);

void outputCSV(list<shared_ptr<Name>> allocMBList, list<shared_ptr<Name>> freedMBList, string filename, int total);

int findTotalSize(list<shared_ptr<Name>> allocMBList, list<shared_ptr<Name>> freedMBList);

void printMBList(list<shared_ptr<Name>> memList);


int main(int argc, char** argv) {

    // Seeding random generator
    srand((unsigned)time(0));

    std::ifstream infile;

    list<shared_ptr<Name>> allocMBList;
    list<shared_ptr<Name>> freedMBList;

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

    // delete after
    printMBList(allocMBList);
    printMBList(freedMBList);

    int total = findTotalSize(allocMBList, freedMBList);

    outputCSV(allocMBList, freedMBList, outputFileName, total);
    cout << "Data output to " << outputFileName << endl;

    // Clean up
    sbrk(-total);

    return EXIT_SUCCESS;
}

bool firstFitStrategy(list<shared_ptr<Name>> &allocMBList, list<shared_ptr<Name>> &freedMBList, int wsize, const char* token) {
    
    list<shared_ptr<Name>>::iterator it;

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

bool bestFitStrategy(list<shared_ptr<Name>> &allocMBList, list<shared_ptr<Name>> &freedMBList, int wsize, const char* token) {

    sortByMemSizeSmallToLarge(freedMBList); // Order the list by size from smallest to largest

    return firstFitStrategy(allocMBList, freedMBList, wsize, token);

}

bool worstFitStrategy(list<shared_ptr<Name>> &allocMBList, list<shared_ptr<Name>> &freedMBList, int wsize, const char* token) {

    sortByMemSizeLargeToSmall(freedMBList); // Order the list by size from largest to smallest

    return firstFitStrategy(allocMBList, freedMBList, wsize, token);

}

void createSbrkBlock(list<shared_ptr<Name>> &memList, int wsize, const char* token) {

    // auto request;
    auto request = sbrk(wsize);
    strcpy((char*)request, token);

    auto name = make_shared<Name>(wsize, (char*)request);
    // name->size = wsize;
    // name->wptr = (char*)request;
    memList.push_back(name);

}

bool combineMemoryBlock(list<shared_ptr<Name>> &freedMBList) {

    for (unsigned int i = 0; i < freedMBList.size() - 1; ++i) {
        list<shared_ptr<Name>>::iterator it1 = std::next(freedMBList.begin(), i);
        list<shared_ptr<Name>>::iterator it2 = std::next(freedMBList.begin(), i + 1);

        if ((*it1)->wptr + (*it1)->size == (*it2)->wptr) {
            (*it1)->size += (*it2)->size;

            freedMBList.remove(*it2);

            return true;
        }

    }

    return false;
}

void splitMemoryBlock(list<shared_ptr<Name>> &allocMBList, list<shared_ptr<Name>> &freedMBList, list<shared_ptr<Name>>::iterator it, int wsize, const char* token) {

    // Split memory block and create new struct for smaller block
    auto name = make_shared<Name>((*it)->size - wsize, &(*it)->wptr[wsize]);

    freedMBList.push_back(name);

    strcpy((*it)->wptr, token);
    (*it)->size = wsize;

    // Allocate, add to alloc list, remove from freed list
    allocMBList.push_back(*it);
    freedMBList.remove(*it);

}

void sortByMemAddress(list<shared_ptr<Name>> &memList) {
    memList.sort([](shared_ptr<Name> a, shared_ptr<Name> b) {
        return a->wptr < b->wptr;   
    });
}

void sortByMemSizeSmallToLarge(list<shared_ptr<Name>> &memList) {
    memList.sort([](shared_ptr<Name> a, shared_ptr<Name> b) {
        return a->size < b->size;
    });
}

void sortByMemSizeLargeToSmall(list<shared_ptr<Name>> &memList) {
    memList.sort([](shared_ptr<Name> a, shared_ptr<Name> b) {
        return a->size > b->size;
    });
}

void randomRemoval(list<shared_ptr<Name>> &allocMBList, list<shared_ptr<Name>> &freedMBList, int count) {

    int randomCount = 0;

    while (randomCount < count) {
        int random = rand() % allocMBList.size();
        list<shared_ptr<Name>>::iterator it = std::next(allocMBList.begin(), random);

        freedMBList.push_back(*it);
        allocMBList.remove(*it);

        ++randomCount;
    }

}

void outputCSV(list<shared_ptr<Name>> allocMBList, list<shared_ptr<Name>> freedMBList, string filename, int total) {

    std::ofstream outfile;

    // Output CSV file to filename
    outfile.open(filename);

    outfile << "Total memory:," << total << endl; 
    outfile << endl;

    outfile << "allocMBList" << endl;
    outfile << "Address," << "Size," << "Content" << endl;

    for (shared_ptr<Name> name : allocMBList) {
        outfile << (void*)name->wptr << "," 
                << name->size << "," 
                << name->wptr
                << endl;
    }

    outfile << endl;

    outfile << "freedMBList" << endl;
    outfile << "Address," << "Size" << endl;

    for (shared_ptr<Name> name : freedMBList) {
        outfile << (void*)name->wptr << "," 
                << name->size
                << endl;
    }

    outfile.close();

}

int findTotalSize(list<shared_ptr<Name>> allocMBList, list<shared_ptr<Name>> freedMBList) {

    int total = 0;

    for (shared_ptr<Name> name : allocMBList) {
        total += name->size;
    }

    for (shared_ptr<Name> name : freedMBList) {
        total += name->size;
    }
    
    return total;
}

void printMBList(list<shared_ptr<Name>> memList) {

    cout << "List size: " << memList.size() << endl;

    for (shared_ptr<Name> name : memList) {
        printf("Starting address: %p, Size: %d, Name: %s\n", name->wptr, name->size, (char*)name->wptr);
    }

    cout << endl;
}