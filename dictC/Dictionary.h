
#ifndef Dictionary_h
#define Dictionary_h

class Dictionary {
public:
    // Add a record to the dictionary. Returns false if key already exists
    virtual bool addRecord( const char * key, int data) = 0;

    // Find a key in the dictionary and return corresponding record or NULL
    virtual bool findRecord( const char * key, int & data) = 0;
};

#endif

