# sqlite3-pb-ext-example
Example for [sqlite3-pb-ext-gen](https://github.com/kzolti/sqlite3-pb-ext-gen/) project
### Dependencies:  
protobuf-compiler(proto), sqlite3-pb-ext-gen  
### Init repo
```
git clone git@github.com:kzolti/sqlite3-pb-ext-example.git \
&& cd sqlite3-pb-ext-example 
```

### Run cmake and build 
```
mkdir -p build && cd build \
&& cmake .. && cmake --build .
```

### Run example:
```
./sqlite_pb_ext_example
```
### output:
```
Opened database successfully
Create person_name_idx ..
The 'person' table is empty, Upload 1 000 000 records ...
 - Inserting records: 100 %  

Home phones from 'John Doe 873820' 
                      id                    name            phone_number
                  873820         John Doe 873820            555-873820-0
                      id                    name            phone_number
                  873820         John Doe 873820            555-873820-6

-=END=-
```
