/***************************************************************************
 *   Copyright (C) $YEAR$ by Juan Roberto Cabral Flores   *
 *   roberto.cabral@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
//#include <Eigen/Core>
//#include <Eigen/NewStdVector>
//#include <QtCore/QCoreApplication>

#include <ext/hash_set>
#include <ext/hash_map>
#include <QHash>
#include <string>
#include <iostream>
#include "command/ICommand.h"
#include "Vertex.h"

namespace __gnu_cxx
{
        template<> struct hash< std::string >
        {
                size_t operator()( const std::string& x ) const
                {
                        return hash< const char* >()( x.c_str() );
                }
        };
}

using namespace std;
using namespace __gnu_cxx;

typedef hash_map<string, ICommand*> MyMap;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QHash<int, Vertex*> table;
    hash_map<int, Vertex*> hashmap;
    cout << "QHash size: "  << sizeof(table) << endl;
    cout << "hash_map size: "  << sizeof(hashmap) << endl;

    time_t delta = 0;
    time_t t0, t1;
    t0 = t1 = time(NULL);
    delta = abs(t1 - t0);
    cout << delta << endl;
    for (int i = 0; i < 10000000; ++i) {
        Vertex *v = new Vertex;
        table.insert(v->iid(), v);
        hashmap[v->iid()] = v;
    }
    t0 = time(NULL);
    delta = abs( t0 - t1);
    cout << delta << endl;
    for (int i = 0; i < 10000000; ++i) {
        table.contains( (i % 100) + 3);
    }
    t1 = time(NULL);
    delta = abs( t1 - t0);
    cout << delta << endl;
    for (int i = 0; i < 10000000; ++i) {
        hashmap.find((i % 100) + 3);
    }
    t0 = time(NULL);
    delta = abs( t0 - t1);
    cout << delta << endl;
    foreach(Vertex* v, table) {
        delete v;
    }
    table.clear();
    hashmap.clear();
    //return a.exec();
}
