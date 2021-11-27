# OUSQL - ORM (Object Relationship Manager)

This is a simple wrapper around a sqlite database (code resides in lib/OUSqlite) to provide native conversions between sqlite field types and C++ plain old datatypes plud std::string.

A number of basic functions are provided:
+ define tables based upon a record layout
+ define keys in the record
+ define foreign key relationships
+ create tables based upon the defined structure
+ methods are provided for adding, inserting, updating, and deleting table records

These techniques are represented extensively in the lib/TFTrading/*Manager.[h
|cpp] files.

By way of working through an example of an implementation, it will be easier to understand the usage.

The following code hasn't been compiletested, but is designed to provide guidance to usage.  See actual code in the suggested files for reference.

To define the schema of a row in a table, a structure is created.  lib/TFTrading/Instrument.h supplies a specific example for the Instrument record representation on disk.  For consistency for definitions for various table types, the structure is named TableRowDef:

```
struct TableRowDef {

// TableRowDef contains a templated method which is used for mapping between database fields
// and C++ plain old data types.  A function will be passed (Action) and will be called for 
// each of the fields, either as a database->variable or as variable->database.
  template<class Action>
  void Fields( Action& action ) {
    // ou::db::Field is a generic method into which the action will be passed with the
    // database field name and a reference to a variable
    ou::db::Field( action, "field1", field1 );
    ou::db::Field( action, "field2", field2 );
  }

  // these are the variables used in the above method as source or destination
  int field1;
  std::string field2;

  // supply a default constructor for a read operation
  TableRowDef()
  : field1 {}
  {}

  // an example specialized constructor for populating various fields prior to a
  // write or update operation
  TableRowDef( int field1_, const std::string& field2_ )
  : field1( field1_ ), field2( field2_ )
  {}
};

// For records with keys, create a structure which inherits TableRowDef.  This segregation
// allows TableRowDef to be used for updates, and TableCreateDef for record creation with a few extra schema descriptions.

struct TableCreateDef: TableRowDef {

  template<class Action>
  void Fields( Action& action ) {
    // include TableRowDef with basic field definitions
    TableRowDef::Fields( action );
    // designate one or more fields for the key
    ou::db::Key( action, "field1" );
    // designate one or more foreign keys
    ou::db::Constraint( action, "field2", "othertable", "field3" );
  }
};
```

A database object needs to be created and populated with a series of callbacks.  When the database is opened, it will be automatically constructed and populated if it doesn't exist, then normal read/write/update operations can be performed.

In many of the example projects, HandlePopulateDatabase is typically used to populate initital accounts and summary portfolios.

```
ou::tf::DBOps db;

db.OnLoad.Add( MakeDelegate( this, &App::HandleDbOnLoad ) );
db.OnPopulate.Add( MakeDelegate( this, &App::HandleDbOnPopulate ) );
db.OnRegisterTables.Add( MakeDelegate( this, &App::HandleRegisterTables ) );
db.OnRegisterRows.Add( MakeDelegate( this, &App::HandleRegisterRows ) );
db.OnPopulate.Add( MakeDelegate( this, &App::HandlePopulateDatabase ) );
db.SetOnLoadDatabaseHandler( MakeDelegate( this, &App::HandleLoadDatabase ) );

db.Open( sDbName );
```

As there is simple table management and caching in place for auto-creation, tables need to be registered along with their row definitions.

```
void App::HandleRegisterTables( ou::db::Session& session ) {
  // note: this uses the TableCreateDef (table creation definition)
  session.RegisterTable<TableCreateDef>( "table1" );
}

void App::HandleRegisterRows( ou::db::Session& session ) {
  // note: this uses the regular field definition
  session.MapRowDefToTableName<TableRowDef>( "table1" );
}
```
Here is an example insertion:

```
TableRowDef trd;

ou::db::QueryFields<TableRowDef>::pQueryFields_t pTable1 
  = db.Insert<TableRowDef>( trd ).NoExecute(); // don't execute the query

// populate variables
trd.field1 = 1;
trd.field2 = "a string";

// a parameterized insert is performed
db.Bind<TableRowDef>( pTable1 );
db.Execute( pTable1 );
```

More examples can be found in the various manager files.

