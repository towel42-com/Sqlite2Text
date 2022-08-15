#ifndef _DBSUMMARY_H
#define _DBSUMMARY_H

#include <QSqlDatabase>
#include <QStringList>

#include <map>

class CTableSummary;
class CDBSummary
{
public:
    CDBSummary( const QString & sqlFile );
    ~CDBSummary(){}

    bool isValid() const{ return fAOK; }
    bool generateData();

    bool writeData( const QString & outFileName, bool schemaOnly ) const;

private:
    bool fAOK{ false };
    QString fInFile;
    QSqlDatabase fDB;
    std::map< QString, CTableSummary * > fTables;
    int fSchemaVersion{ -1 };
    int fUserVersion{ -1 };
};

#endif
