#include <iostream>
#include <iomanip>
#include <sqlite3.h>
#include <string>
#include "../proto/gen/addressbook.pb.h"
using namespace std;

// print sqlite output
static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    int colWidth = 24;
    for (int i = 0; i < argc; i++) {
        cout << setw(colWidth) << azColName[i];
    }
    cout << endl;
    for (int i = 0; i < argc; i++) {
        if (argv[i]) {
            cout << setw(colWidth) << argv[i];
        } else {
            cout << setw(colWidth) << "NULL";
        }
    }
    cout << endl;
    return 0;
}

int main(int argc, char *argv[])
{   
    // init rand
    srand(time(nullptr));
    // create and open db
    if ((sqlite3_initialize()) != SQLITE_OK)  exit(1);
    sqlite3 *db;
    int rc = sqlite3_open("addressbook.sqlite", &db);
    if( rc ) {
        cerr<< "Can't open database:"<< sqlite3_errmsg(db)<<endl;
        return(1);
    } else {
        cout <<"Opened database successfully"<<endl;
        // enable extension load
        sqlite3_db_config(db,SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION,1,NULL);
    }
// get binary directory from cmake config
#ifdef CMAKE_CURRENT_BINARY_DIR
    std::string currentBinaryDir = CMAKE_CURRENT_BINARY_DIR;
#else
    cerr<<"not defined CMAKE_CURRENT_BINARY_DIR"<<endl;
    sqlite3_close(db);
    return 1;
#endif
    // load extension
    char *zErrMsg = nullptr;
    rc = sqlite3_load_extension(db, std::string(currentBinaryDir + "/sqlite3_pb_ext/lib/system_sqlite3/libsqlite3_pb_ext").c_str(), nullptr, &zErrMsg);
    if( rc != SQLITE_OK ){
        cerr<< "SQL error:"<< zErrMsg<<endl;
        sqlite3_free(zErrMsg);
    }
    // disable extension load
    sqlite3_db_config(db,SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION,0,NULL);
    const char *createTableSql = "CREATE TABLE IF NOT EXISTS person (id INTEGER PRIMARY KEY, proto BLOB);";
    rc = sqlite3_exec(db, createTableSql, NULL, NULL, NULL);
    if (rc != SQLITE_OK) {
        cerr<< "Error creating table: " << sqlite3_errmsg(db)<<endl ;
        sqlite3_close(db);
        return 1;
    }
    // create table 
    const char *createPersonIdx = "CREATE INDEX IF NOT EXISTS person_name_idx ON person (pb_extract(proto,'tutorial.Person','name') ASC);";
    rc = sqlite3_exec(db, createPersonIdx, NULL, NULL, NULL);
    if (rc == SQLITE_OK) {
        std::cout<<"Create person_name_idx .."<<std::endl;
    }else{
        cerr<< "Error creating person_name_idx: " << sqlite3_errmsg(db)<<endl ;
        sqlite3_close(db);
        return 1;
    }
    const char* query = "SELECT COUNT(*) FROM person;";
    sqlite3_stmt* statement;
    rc = sqlite3_prepare_v2(db, query, -1, &statement, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Query preparation error: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return rc;
    }

    // Upload person table 1 000 000 records
    rc = sqlite3_step(statement);
    if (rc == SQLITE_ROW) {
        int rowCount = sqlite3_column_int(statement, 0);
        if (rowCount == 0) {
            std::cout << "The 'person' table is empty, Upload 1 000 000 records ..."<<std::endl;
            const char *insertSql = "INSERT OR REPLACE INTO person (id, proto) VALUES (?, ?)";
            sqlite3_exec(db, "BEGIN TRANSACTION;", 0, 0, 0);
            for (int status_counter = 0; status_counter < 20; ++status_counter) {
                for (int i = 1; i <= 50000; ++i) {
                    const int id=status_counter*50000+i;
                    sqlite3_stmt *stmt;
                    rc = sqlite3_prepare_v2(db, insertSql, -1, &stmt, NULL);
                    if (rc != SQLITE_OK) {
                        cerr << "Error preparing statement: " << sqlite3_errmsg(db) <<endl;
                        sqlite3_close(db);
                        return 1;
                    }
                    tutorial::Person person;
                    person.set_id(id);
                    std::string name="John Doe " + std::to_string(id);
                    person.set_name(name);
                    std::string email="john.doe_" + std::to_string(id) + "@example.com";
                    person.set_email(email);
                    tutorial::Person::PhoneNumber *phone = person.add_phones();
                    std::string number="555-" + std::to_string(id) + "-0";
                    phone->set_number(number);
                    tutorial::Person::PhoneType type = tutorial::Person::PhoneType(tutorial::Person::PhoneType::Person_PhoneType_HOME);
                    phone->set_type(type);
                    const int to = std::rand() % 5 + 3;
                    for (int j = 1; j < to; ++j) {
                        tutorial::Person::PhoneNumber *phone = person.add_phones();
                        std::string number="555-" + std::to_string(id) + "-" + std::to_string(j);
                        phone->set_number(number);
                        tutorial::Person::PhoneType type = tutorial::Person::PhoneType(std::rand() % 3);
                        phone->set_type(type);
                    }
                    google::protobuf::Timestamp *update = new google::protobuf::Timestamp();
                    update->set_seconds(id);
                    update->set_nanos(id % 1000);
                    person.set_allocated_last_updated(update);

                    const std::string serialized = person.SerializeAsString();
                    sqlite3_bind_int(stmt, 1, id);
                    sqlite3_bind_blob(stmt, 2, serialized.data(), serialized.size(), SQLITE_STATIC);
                    rc = sqlite3_step(stmt);
                    if (rc == SQLITE_DONE) {
                        sqlite3_finalize(stmt);
                    }else{
                        cerr << "Error inserting data: " << sqlite3_errmsg(db) <<endl;
                        sqlite3_finalize(stmt);
                        sqlite3_close(db);
                        return 1;
                    }
                }
                std::cout<<"\r - Inserting records: "<< status_counter*5<<" %  "<<std::flush;
            }
            std::cout<<std::endl;
            sqlite3_exec(db, "COMMIT;", 0, 0, 0);
        } else {
            std::cout << "There are "<< std::to_string(rowCount) <<" records in the person table" << std::endl;
        }
    } else {
        std::cerr << "Query execution error: " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(statement);

    // run query
    const std::string person_name="John Doe " + std::to_string(rand() % 1000000 + 1);
    const std::string sql = "SELECT  "
                            "id "
                            ", pb_extract(person.value, person.type_name, 'name') name "
                            ", pb_extract(phone.value, phone.type_name, 'number') phone_number "
                            "FROM person AS person_t"
                            ", pb_field(proto, 'tutorial.Person','') person"
                            ", pb_each(person.value, person.type_name, 'phones') phone  "
                            ", pb_field(phone.value, phone.type_name, 'type') phone_type  "
                            " WHERE "
                            "pb_extract(proto,'tutorial.Person','name')='" + person_name + "'"
                                            " AND phone_type.type_name='tutorial.Person.PhoneType.HOME'"
                                            ";"
        ;
    std::cout<<"\nHome phones from '"<<person_name<<"' "<<std::endl;
    rc = sqlite3_exec(db, sql.c_str(), callback, nullptr, &zErrMsg);
    if( rc != SQLITE_OK ){
        cerr<< "SQL error:"<< zErrMsg<<endl;
        sqlite3_free(zErrMsg);
    }
    
    if(sqlite3_close(db)){
        std::cerr<<"sqlite3 closing err";
        return 1;
    }

    std::cout<<"\n-=END=- "<<std::endl;
    return 0;
}
