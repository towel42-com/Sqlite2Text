#include "DBSummary.h"


#include <QCoreApplication>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>



#ifdef _WIN32
#include <windows.h>
#endif
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <iostream>

void showHelp()
{
    std::cout 
        << "<CMD> infile [outfile=\"infile.txt\"] -schema\n"
        << "      infile - SQLite3 file\n"
        << "      outfile - Text file representing the database\n"
        << "      -schema - report on schema only\n"
        ;
}


int main( int argc, char ** argv  )
{
    QCoreApplication appl( argc, argv );
    Q_INIT_RESOURCE( application );

    QCoreApplication::setOrganizationName( "OnShore Consulting Services" );
    QCoreApplication::setApplicationName( "SQLite 2 XML" );
    QCoreApplication::setApplicationVersion( "1.0.0" );
    QCoreApplication::setOrganizationDomain( "www.towel42.com" );
    //appl.setWindowIcon( QPixmap( ":/resources/logo.png" ) );

    QString appDir = appl.applicationDirPath();

    QStringList libPaths = appl.libraryPaths();
    libPaths.push_front( appDir );
    appl.setLibraryPaths( libPaths );

    if ( !QSqlDatabase::isDriverAvailable( "QSQLITE" ) )
    {
        std::cerr << "Error initializing system - Could not find Database libraries.  Please re-install or contact support." << std::endl;
        return 1;
    }

    QString sqlFile;
    QString outFile;
    bool schemaOnly = false;
    for( int ii = 1; ii < argc; ++ii )
    {
        if ( argv[ ii ] == std::string( "-schema" ) )
            schemaOnly = true;
        else if ( sqlFile.isEmpty() )
            sqlFile = argv[ ii ];
        else if ( outFile.isEmpty() )
            outFile = argv[ ii ];
        else
        {
            showHelp();
            return 1;
        }
    }

    if ( sqlFile.isEmpty() || !QFileInfo( sqlFile ).isFile() )
    {
        std::cerr << "Input file must be set\n";
        showHelp();
        return 1;
    }

    if ( outFile.isEmpty() )
    {
        outFile = QFileInfo( sqlFile ).absoluteDir().absoluteFilePath( QFileInfo( sqlFile ).baseName() + ".txt" );
    }

    CDBSummary dbSummary( sqlFile );
    if ( !dbSummary.isValid() )
    {
        std::cerr << "Could not open '" + sqlFile.toStdString() + "' in sqlite\n";
        showHelp();
        return 1;
    }


    if ( !dbSummary.generateData() )
    {
        std::cerr << "Could not read '" + sqlFile.toStdString() + "' in sqlite\n";
        return 1;
    }

    if ( !dbSummary.writeData( outFile, schemaOnly ) )
    {
        std::cerr << "Could not write data to '" + outFile.toStdString() + "'\n";
        return 1;
    }
    return 0;
}

