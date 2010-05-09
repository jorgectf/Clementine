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

#ifndef LIBRARYDIRECTORYMODEL_H
#define LIBRARYDIRECTORYMODEL_H

#include <QStandardItemModel>
#include <QIcon>

#include "directory.h"

class LibraryBackend;

class LibraryDirectoryModel : public QStandardItemModel {
  Q_OBJECT

 public:
  LibraryDirectoryModel(LibraryBackend* backend, QObject* parent = 0);

  // To be called by GUIs
  void AddDirectory(const QString& path);
  void RemoveDirectory(const QModelIndex& index);

 private slots:
  // To be called by the backend
  void DirectoryDiscovered(const Directory& directories);
  void DirectoryDeleted(const Directory& directories);

 private:
  static const int kIdRole = Qt::UserRole + 1;

  QIcon dir_icon_;
  LibraryBackend* backend_;
};

#endif // LIBRARYDIRECTORYMODEL_H
