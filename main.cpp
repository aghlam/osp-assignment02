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

            void* request;
            request = sbrk(wsize);
            strcpy((char*)request, token);


            if (counter < 20) {
                Name* name = new Name();
                name->wptr = (char*)request;
                name->size = wsize;
                allocMBList.push_back(name);

            } else if (counter >= 20 && counter < 40) {

                for (Name* freeName : freedMBList) {
                    if (wsize <= (unsigned)freeName->size) {
                        freeName->wptr = (char*)request;
                        freeName->size = wsize;
                        
                        break;
                    }
                }

            }


            ++counter;

            if (counter == 20) {

                for (Name* name : allocMBList) {
                    cout << "Starting address: " << (void*)name->wptr << ", size: " << name->size << ", name: " << name->wptr << endl;
                }
                cout << endl;

                int randomCount = 0;

                while (randomCount < 10) {
                    
                    int random = rand() % allocMBList.size();
                    // cout << random << endl;
                    // cout << randomCount << endl;
                    list<Name*>::iterator it = std::next(allocMBList.begin(), random);

                    freedMBList.push_back(*it);
                    allocMBList.remove(*it);

                    ++randomCount;
                }

                cout << "AT 20!" << endl;
                for (Name* name : allocMBList) {
                    cout << "Starting address: " << (void*)name->wptr << ", size: " << name->size << ", name: " << name->wptr << endl;
                }
                cout << endl;

                for (Name* name : freedMBList) {
                    cout << "Starting address: " << (void*)name->wptr << ", size: " << name->size << ", name: " << name->wptr << endl;
                }
                cout << endl;

            }

            if (counter == 40) {

                cout << "AT 40!" << endl;
                for (Name* name : allocMBList) {
                    cout << "Starting address: " << (void*)name->wptr << ", size: " << name->size << ", name: " << name->wptr << endl;
                }
                cout << endl;

                for (Name* name : freedMBList) {
                    cout << "Starting address: " << (void*)name->wptr << ", size: " << name->size << ", name: " << name->wptr << endl;
                }
                cout << endl;

                cout << "alloc size: " << allocMBList.size() << endl;
                cout << "freed size: " << freedMBList.size() << endl;

                cout << "compare Address" << endl;
                cout << (void*)freedMBList.front()->wptr << endl;
                cout << freedMBList.front()->wptr << endl;
                cout << freedMBList.front()->wptr[1] << endl;
                cout << (void*)freedMBList.front()->wptr << endl;
            }


        }

    }

    infile.close();

    return EXIT_SUCCESS;
}


