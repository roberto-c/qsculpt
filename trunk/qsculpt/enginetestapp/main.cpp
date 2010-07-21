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
#include <iomanip>
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

enum Flags {
    F_0 = 0,
    F_1 = 1<<0,
    F_2 = 1<<1,
    F_3 = 1<<2,
    F_A = 0xFFFFFFFF
};

Flags flags = F_0;
void addFlag(Flags flag) {
    flags = (Flags)(flags | flag);
}

void removeFlag(Flags flag) {
    flags = (Flags)(flags & ~flag);
}

Flags getFlags() {
    return flags;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QHash<int, Vertex*> table;
    hash_map<int, Vertex*> hashmap;
    cout << "QHash size: "  << sizeof(table) << endl;
    cout << "hash_map size: "  << sizeof(hashmap) << endl;


    unsigned int f = 0x0403;
    cout << setfill('0') << setw(8) << setbase(16) << getFlags() << endl;
    addFlag((Flags)f);
    cout << setfill('0') << setw(8) << setbase(16) << getFlags() << endl;
    removeFlag(F_1);
    cout << setfill('0') << setw(8) << setbase(16) << getFlags() << endl;

    cout << endl;

    Vertex *v = new Vertex(Point3());
    cout << setfill('0') << setw(8) << setbase(16) << v->flags() << endl;
    v->addFlag((VertexFlags)f);
    cout << setfill('0') << setw(8) << setbase(16) << v->flags() << endl;
    v->removeFlag(VF_Selected);
    cout << setfill('0') << setw(8) << setbase(16) << v->flags() << endl;
    //return a.exec();
}
