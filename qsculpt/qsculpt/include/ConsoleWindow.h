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

#ifndef CONSOLEWINDOW_H
#define CONSOLEWINDOW_H

#include <QDockWidget>

namespace Ui {
    class ConsoleWindow;
}

class ConsoleWindow : public QDockWidget
{
    Q_OBJECT

public:
    explicit ConsoleWindow(QWidget *parent = 0);
    ~ConsoleWindow();

    void write(const QString &);
    
public slots:
    void executeLine();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ConsoleWindow *ui;
};

#endif // CONSOLEWINDOW_H
