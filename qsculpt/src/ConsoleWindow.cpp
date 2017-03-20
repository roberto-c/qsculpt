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
#include "Stable.h"
#include "ConsoleWindow.h"
#include "Console.h"
#include "ui_ConsoleWindow.h"

ConsoleWindow::ConsoleWindow(QWidget* parent)
    : QDockWidget(parent)
    , ui(new Ui::ConsoleWindow)
{
    ui->setupUi(this);
}

ConsoleWindow::~ConsoleWindow() { delete ui; }

void ConsoleWindow::changeEvent(QEvent* e)
{
    QDockWidget::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ConsoleWindow::executeLine()
{
    //    qDebug() << "Execute...";
    Console::instance()->evaluate(ui->input->text());
    ui->input->setText("");
}

void ConsoleWindow::write(const QString& text)
{
    ui->output->appendPlainText(text);
}