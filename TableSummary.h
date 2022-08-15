#ifndef _TABLESUMMARY_H
#define _TABLESUMMARY_H

#include <QStringList>
#include <QSqlRecord>

#include <map>
#include <list>
#include <cstdint>
class QSqlDatabase;
class QTextStream;

class CTableSummary
{
public:
    CTableSummary( const QString & tableName, QSqlDatabase & db );
    ~CTableSummary(){}

    bool isValid() const{ return fAOK; }
    bool readData();
    void writeHeaderData( QTextStream & ts ) const;
    void writeData( QTextStream & ts, bool schemaOnly ) const;

    uint64_t schemaHash() const;
private:
    std::map< uint64_t, uint64_t > computeColumnMaxWidths() const;

    QString fTableName;
    QSqlRecord fRecord;
    QSqlDatabase & fDB;
    bool fAOK{ false };

    std::list< std::list< QString > > fData;
    mutable std::pair< bool, uint64_t > fSchemaHash{ false, -1 };
};

extern QString kSpacer;
#endif
