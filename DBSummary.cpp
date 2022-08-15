#include "DBSummary.h"
#include "TableSummary.h"

#include <QFile>
#include <iostream>
#include <QTextStream>
#include <QSqlQuery>

CDBSummary::CDBSummary( const QString & sqlFile )
{
    fInFile = sqlFile;
    fDB = QSqlDatabase::addDatabase( "QSQLITE", "MainConnection" );
    fDB.setConnectOptions( "QSQLITE_OPEN_READONLY=1" );
    fDB.setDatabaseName( fInFile );
    fAOK = fDB.open();
}

bool CDBSummary::generateData()
{
    auto tables = fDB.tables();
    for( auto && ii : tables )
    {
        auto curr = new CTableSummary( ii, fDB );
        if ( !curr->isValid() || !curr->readData() )
        {
            delete curr;
            return false;
        }
        fTables[ ii ] = curr;
    }

    QSqlQuery query( "PRAGMA schema_version;", fDB );

    if ( !query.exec() )
    {
        std::cerr << "Could not read schema_version" << "\n";
        return false;
    }

    if ( query.next() )
        fSchemaVersion = query.value( 0 ).toInt();

    query.clear();

    query = QSqlQuery( "PRAGMA user_version;", fDB );

    if ( !query.exec() )
    {
        std::cerr << "Could not read user_version" << "\n";
        return false;
    }

    if ( query.next() )
        fUserVersion = query.value( 0 ).toInt();

    query.clear();
    return true;
}

bool CDBSummary::writeData( const QString & fileName, bool schemaOnly ) const
{
    QFile fi( fileName );
    if ( !fi.open( QFile::WriteOnly ) )
    {
        std::cerr << "Could not open file '" + fileName.toStdString() + "' for writing.\n";
        return false;
    }

    QTextStream ts( &fi );

    ts << kSpacer
       << "DataBase overview" << "\n"
       << kSpacer
       << QString( "SchemaVersion: %1" ).arg( fSchemaVersion, 10, 10 ) << "\n"
       << QString( "UserVersion: %1" ).arg( fUserVersion, 10, 10 ) << "\n"
       << kSpacer
       << "Tables overview" << "\n"
       << kSpacer
    ;
    ts << QString( "%1|%2|%3|%4" ).arg( "Table", 25 ).arg( "Columns", 10 ).arg( "Rows", 10 ).arg( "Hash", 25 ) << "\n"
       << QString( "%1|%2|%3|%4" ).arg( QString( 25, '-' ), 25 ).arg( QString( 10, '-' ),10 ).arg( QString( 10, '-' ), 10 ).arg( QString( 25, '-' ), 25 ) << "\n"
       ;

    for( auto ii : fTables )
    {
        ii.second->writeHeaderData( ts );
    }

    ts << kSpacer
       << "Content\n"
       << kSpacer
       ;

    for ( auto ii : fTables )
    {
        ii.second->writeData( ts, schemaOnly );
    }

    ts << "EOF\n";
    return true;
}
