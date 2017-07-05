/***************************************************************************
 *   Copyright (C) 2006 by Juan Roberto Cabral Flores   *
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
#include "QSculptApp.h"
#include <PlastilinaCore/IDocument.h>
#include <PlastilinaCore/ResourcesManager.h>
#include <QtGui/QSurfaceFormat>
#include <QtWidgets/QWidget>
#include <boost/program_options.hpp>
#include <stdexcept>
#include "QSculptWindow.h"

namespace po = boost::program_options;

QSculptApp* g_pApp = NULL;

QSculptApp::QSculptApp(int& argc, char** argv)
    : QApplication(argc, argv)
    , m_mainWindow(new QSculptWindow)
{
}

QSculptApp::~QSculptApp()
{
    // delete m_mainWindow;
}

QSculptWindow* QSculptApp::getMainWindow() { return m_mainWindow; }

bool QSculptApp::notify(QObject* receiver, QEvent* e)
{
    try
    {
        return QApplication::notify(receiver, e);
    }
    catch (std::exception& e)
    {
        qDebug() << "Exception: " << e.what();
    }
    return false;
}

/**
 * Main entry point.
 *
 * Creates the application object. Passes the program arguments to the
 * applicatoin object to process them. Creates and show the main
 * application window.
 */
int main(int argc, char** argv)
{
    int result = 0;

    try
    {
        using namespace std;
        po::options_description optionsDesc;
        po::variables_map       options;
        vector<string> default_search_dirs = {core::utils::get_app_path()};
        // Declare the supported options.
        optionsDesc.add_options()("help", "produce help message")(
            "resourcesdir",
            po::value<vector<string>>()->default_value(
                default_search_dirs, core::utils::get_app_path()),
            "path used to load all resources");

        po::store(po::parse_command_line(argc, argv, optionsDesc), options);
        po::notify(options);

        if (options.count("help"))
        {
            std::cout << optionsDesc << "\n";
            return 1;
        }

        // Set Resources search directories
        auto pathList = options["resourcesdir"].as<std::vector<std::string>>();
        for (auto path : pathList)
        {
            ResourcesManager::addResourcesDirectory(path);
        }

        QSurfaceFormat format;
        format.setDepthBufferSize(24);
        format.setStencilBufferSize(8);
        format.setVersion(4, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
        QSurfaceFormat::setDefaultFormat(format);

        QSculptApp a(argc, argv);

        g_pApp = (QSculptApp*)QSculptApp::instance();

        g_pApp->setOrganizationName("QSculpt");
        g_pApp->setOrganizationDomain("qsculpt.com");
        g_pApp->setApplicationName("QSculpt");

        g_pApp->getMainWindow()->show();

        result = a.exec();
    }
    catch (std::exception& e)
    {
        std::cerr << "What: " << e.what() << std::endl;
        result = -1;
    }
    catch (...)
    {
        result = -1;
    }

    return result;
}