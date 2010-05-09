/* This file is part of Clementine.

   Clementine is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Clementine is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Clementine.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMutex>
#include <QStringList>

#include <sqlite3.h>

#include "gtest/gtest_prod.h"

class Database : public QObject {
  Q_OBJECT

 public:
  Database(QObject* parent = 0, const QString& database_name = QString());

  static const int kSchemaVersion;
  static const char* kDatabaseFilename;

  QSqlDatabase Connect();
  bool CheckErrors(const QSqlError& error);

 signals:
  void Error(const QString& message);

 private:
  void UpdateDatabaseSchema(int version, QSqlDatabase& db);

  QString directory_;
  QMutex connect_mutex_;

  // Used by tests
  QString injected_database_name_;

  uint query_hash_;
  QStringList query_cache_;

  FRIEND_TEST(DatabaseTest, LikeWorksWithAllAscii);
  FRIEND_TEST(DatabaseTest, LikeWorksWithUnicode);
  FRIEND_TEST(DatabaseTest, LikeAsciiCaseInsensitive);
  FRIEND_TEST(DatabaseTest, LikeUnicodeCaseInsensitive);
  FRIEND_TEST(DatabaseTest, LikePerformance);
  FRIEND_TEST(DatabaseTest, LikeCacheInvalidated);
  FRIEND_TEST(DatabaseTest, LikeQuerySplit);

  // Do static initialisation like loading sqlite functions.
  static void StaticInit();

  // Custom LIKE() function for sqlite.
  bool Like(const char* needle, const char* haystack);
  static void SqliteLike(sqlite3_context* context, int argc, sqlite3_value** argv);
  typedef int (*Sqlite3CreateFunc) (
      sqlite3*, const char*, int, int, void*,
      void (*) (sqlite3_context*, int, sqlite3_value**),
      void (*) (sqlite3_context*, int, sqlite3_value**),
      void (*) (sqlite3_context*));

  // Sqlite3 functions. These will be loaded from the sqlite3 plugin.
  static Sqlite3CreateFunc _sqlite3_create_function;
  static int (*_sqlite3_value_type) (sqlite3_value*);
  static sqlite_int64 (*_sqlite3_value_int64) (sqlite3_value*);
  static const uchar* (*_sqlite3_value_text) (sqlite3_value*);
  static void (*_sqlite3_result_int64) (sqlite3_context*, sqlite_int64);
  static void* (*_sqlite3_user_data) (sqlite3_context*);

  static bool sStaticInitDone;
  static bool sLoadedSqliteSymbols;
};

class MemoryDatabase : public Database {
 public:
  MemoryDatabase(QObject* parent = 0) : Database(parent, ":memory:") {}
  ~MemoryDatabase() {
    // Make sure Qt doesn't reuse the same database
    QSqlDatabase::removeDatabase(Connect().connectionName());
  }
};

#endif // DATABASE_H
