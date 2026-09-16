/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "gui/MainWindow.hpp"
#include "gui/utils/utils.hpp"
#include "logger.hpp"
#include "manager.hpp"
#include "settings/json.hpp"
#include "settings/settings.hpp"
#include "version.hpp"

#include <filesystem>

#include <plog/Log.h>

#include <QApplication>
#include <QCommandLineParser>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QTranslator>

void init()
{
    QCoreApplication::setApplicationName("Cathedral Assets Optimizer");
    QCoreApplication::setApplicationVersion(k_cao_version);
}

void init_translations()
{
    // Qt translations
    static QTranslator qt_translator;
    if (qt_translator.load(QLocale::system(),
                           "qtbase",
                           "_",
                           QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
    {
        QCoreApplication::installTranslator(&qt_translator);
    }

    // CAO translations
    static QTranslator assets_opt_translator;
    if (qt_translator.load(QLocale::system(), "AssetsOpt", "_", "translations"))
    {
        QCoreApplication::installTranslator(&assets_opt_translator);
    }
}

void display_error(bool cli, const std::string &err)
{
    std::cerr << err << '\n' << std::flush;
    PLOG_FATAL << err;
    if (!cli)
        QMessageBox::critical(nullptr, "Unknown error", QString::fromStdString(err));
}

auto main(int argc, char *argv[]) -> int
{
    auto app = std::make_unique<QCoreApplication>(argc, argv);

    init();

    QCommandLineParser parser;
    parser.addPositionalArgument("profile",
                                 "The profile to use: a saved profile's name, or (with "
                                 "--profile-file) a path to a standalone JSON profile file");
    parser.addOption({"cli", "Do not run the GUI"});
    parser.addOption({"profile-file",
                      "Treat the positional argument as a path to a standalone JSON profile "
                      "file instead of a name looked up in the saved settings.json profile "
                      "list. Runs that profile directly, without depending on (or being able "
                      "to corrupt) any saved profile."});
    parser.process(*app);

    const bool cli = parser.isSet("cli");

    if (!cli)
    {
        app.reset(); // Destroying the QCoreApplication before creating a QApplication is required
        app = std::make_unique<QApplication>(argc, argv);
    }

    init_translations();

    try
    {
        if (!cao::init_logging(cao::Settings::state_directory()))
            throw std::runtime_error("Failed to initialize logging.");

        if (cli)
        {
            const auto profile_arg = parser.positionalArguments().value(0);

            cao::Settings settings;

            if (parser.isSet("profile-file"))
            {
                // Direct-file mode: load exactly one profile from an external JSON file and run
                // it, entirely independent of the shared settings.json profile list. This is the
                // "bypass all the profile jank" path: no lookup-by-name, no dependency on any
                // saved profile existing, and no risk that a schema-mismatched external profile
                // corrupts (or silently wipes, via the make_base() fallback in load_settings())
                // the user's real saved profiles the way writing into settings.json's profiles_
                // array could.
                const auto profile_path = std::filesystem::path(profile_arg.toStdWString());
                auto profile            = cao::json::read_from_file<cao::Profile>(profile_path);
                if (!profile)
                    throw std::runtime_error("Failed to load profile file: "
                                             + profile_path.string());

                settings.create_profile(u8"cli", std::move(*profile));
                if (!settings.set_current_profile(u8"cli"))
                    throw std::runtime_error("Internal error: failed to select the just-created "
                                             "in-memory profile");
            }
            else
            {
                settings = cao::load_settings();

                // NB: previously this only checked get_profile() for existence and then ran
                // whatever current_profile_index_ already happened to be, which could silently
                // run the wrong profile. set_current_profile() both checks existence and
                // actually selects it.
                if (!settings.set_current_profile(cao::to_u8string(profile_arg)))
                    throw std::runtime_error("Profile not found");
            }

            cao::Manager manager;
            manager.run_optimization(settings, std::stop_token{}); // TODO: handle signals
        }
        else
        {
            auto settings = cao::load_settings();
            auto window = cao::MainWindow{settings};
            window.show();
            return app->exec();
        }
    }
    catch (const std::exception &e)
    {
        display_error(cli, e.what());
        return 1;
    }

    return 0;
}
