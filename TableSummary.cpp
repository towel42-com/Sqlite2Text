#include "TableSummary.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QTextStream>
#include <QSqlField>

#include <iostream>

QString kSpacer = QString( 125, '-' ) + "\n";

CTableSummary::CTableSummary( const QString & tableName, QSqlDatabase & db ) :
    fTableName( tableName ),
    fDB( db )
{
    fRecord = fDB.record( tableName );
    fAOK = !fRecord.isEmpty();
}

bool CTableSummary::readData()
{
    QString sql = QString( "SELECT * FROM '%1' ORDER BY %2" ).arg( fTableName );

    QString orderBy;
    for( int ii = 0; ii < fRecord.count(); ++ii )
    {
        if ( ii != 0 )
            orderBy += ", ";
        orderBy += QString::number( ii + 1 );

    }
    sql = sql.arg( orderBy );

    QSqlQuery query( fDB );
    if ( !query.prepare( sql ) ) 
    {
        std::cerr << query.lastError().driverText().toStdString() << "\n";
        std::cerr << query.lastError().databaseText().toStdString() << "\n";
        return false;
    }

    if ( !query.exec() )
    {
        std::cerr << query.lastError().driverText().toStdString() << "\n";
        std::cerr << query.lastError().databaseText().toStdString() << "\n";
        return false;
    }

    while ( query.next() )
    {
        std::list< QString > currData;
        for( int ii = 0; ii < fRecord.count(); ++ii )
        {
            currData.push_back( query.value( ii ).toString() );
        }
        fData.push_back( currData );
    }

    return true;
}

uint64_t CTableSummary::schemaHash() const
{
    if ( fSchemaHash.first )
        return fSchemaHash.second;

    uint64_t retVal = 0;
    for( int ii = 0; ii < fRecord.count(); ++ii )
    {
        auto field = fRecord.field( ii );
        retVal = ( retVal << 5 ) + retVal + qHash( field.isAutoValue() );
        retVal = ( retVal << 5 ) + retVal + qHash( field.isGenerated() );
        retVal = ( retVal << 5 ) + retVal + qHash( field.isReadOnly() );
        retVal = ( retVal << 5 ) + retVal + qHash( field.length() );
        retVal = ( retVal << 5 ) + retVal + qHash( field.name() );
        retVal = ( retVal << 5 ) + retVal + qHash( field.precision() );
        retVal = ( retVal << 5 ) + retVal + qHash( field.requiredStatus() );
        retVal = ( retVal << 5 ) + retVal + qHash( field.type() );
    }

    fSchemaHash.first = true;
    fSchemaHash.second = retVal;
    return retVal;
}

void CTableSummary::writeHeaderData( QTextStream & ts ) const
{
    ts << QString( "%1|%2|%3|%4" ).arg( fTableName, 25 ).arg( fRecord.count(), 10 ).arg( fData.size(), 10 ).arg( schemaHash(), 25 ) << "\n";
}

std::map< uint64_t, uint64_t > CTableSummary::computeColumnMaxWidths() const
{
    std::map< uint64_t, uint64_t > retVal; // columnNumber to width
    for( int ii = 0; ii < fRecord.count(); ++ii )
    {
        retVal[ ii ] = fRecord.fieldName( ii ).length();
    }


    for( auto && row : fData )
    {
        int colNum = 0;
        for( auto && col : row )
        {
            retVal[ colNum ] = std::max( retVal[ colNum ], (uint64_t)col.length() );
            colNum++;
        }
    }

    return retVal;
}

void CTableSummary::writeData( QTextStream & ts, bool schemaOnly ) const
{
    if ( schemaOnly )
    {
        ts << "Table: " << fTableName << "\n";
        return;
    }

    ts << kSpacer
       << "Table: " << fTableName << "\n"
       << kSpacer
       ;

    auto maxWidths = computeColumnMaxWidths();


    for( int ii = 0; ii < fRecord.count(); ++ii )
    {
        if ( ii != 0 )
            ts << "|";
        ts << QString( "%1" ).arg( fRecord.fieldName( ii ), maxWidths[ ii ] );
    }
    ts << "\n";
    for ( int ii = 0; ii < fRecord.count(); ++ii )
    {
        if ( ii != 0 )
            ts << "|";
        ts << QString( (int)maxWidths[ ii ], '-' );
    }
    ts << "\n";

    for( auto && row : fData )
    {
        int colNum = 0;
        for ( auto && col : row )
        {
            if ( colNum != 0 )
                ts << "|";
            ts << QString( "%1" ).arg( col, maxWidths[ colNum ] );
            colNum++;
        }
        ts << "\n";
    }

}
