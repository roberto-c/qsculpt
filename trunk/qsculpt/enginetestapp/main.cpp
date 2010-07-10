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

#include <ext/hash_map>
#include <string>
#include <iostream>
#include "command/ICommand.h"

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

    vector<Eigen::Vector3f> v;

    MyMap m;
    m["2"] = NULL;

    cout << m["2"] << endl;

    //return a.exec();
}
