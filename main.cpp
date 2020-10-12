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


int main(int argc, char** argv) {

    // Seeding random generator
    srand((unsigned)time(0));

    std::ifstream infile;

    list<Name*> allocMBList;
    list<Name*> freedMBList;

    if (argc != 2) {
        cout << "Requires input file. Please specify file name" << endl;
        return EXIT_FAILURE;
    }

    infile.open(argv[1]);

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

            void *request;
            request = sbrk(wsize);
            strcpy((char*)request, token);

            Name* name = new Name();
            name->wptr = (char*)request;
            name->size = wsize;

            allocMBList.push_back(name);

            ++counter;

            if (counter == 1000) {

                // for (Name* name : allocMBList) {
                //     cout << "Starting address: " << (void*)name->wptr << ", size: " << name->size << ", name: " << name->wptr << endl;
                // }

                int randomCount = 0;

                while (randomCount < 500) {
                    
                    int random = rand() % allocMBList.size();
                    // cout << random << endl;
                    // cout << randomCount << endl;
                    list<Name*>::iterator it = std::next(allocMBList.begin(), random);

                    freedMBList.push_back(*it);
                    allocMBList.remove(*it);

                    ++randomCount;
                }

                // Testing - delete after


                cout << "Alloc List: " << allocMBList.size() << endl;
                cout << "Freed List: " << freedMBList.size() << endl;

                // list<Name*>::iterator it = std::next(allocMBList.begin(), 5);

                // freedMBList.push_back(*it);
                // allocMBList.remove(*it);

                // for (Name* name : allocMBList) {
                //     cout << "Starting address: " << (void*)name->wptr << ", size: " << name->size << ", name: " << name->wptr << endl;
                // }

                // cout << endl;

                // for (Name* name: freedMBList) {
                //     cout << "Starting address: " << (void*)name->wptr << ", size: " << name->size << ", name: " << name->wptr << endl;

                // }

                








            }

        }







    }

    infile.close();

    return EXIT_SUCCESS;
}