/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "PatternsManagerWindow.hpp"

#include "utils/utils.hpp"

#include <QInputDialog>
#include <QMessageBox>
#include <QQmlContext>
#include <QQuickWidget>
#include <QVBoxLayout>

namespace cao {

PatternsManagerWindow::PatternsManagerWindow(Settings &settings, QWidget *parent)
    : QDialog(parent)
    , settings_(settings)
{
    setWindowTitle(tr("Patterns manager"));
    resize(400, 300);

    auto *layout = new QVBoxLayout(this); // NOLINT(cppcoreguidelines-owning-memory)

    qml_widget_ = new QQuickWidget(this); // NOLINT(cppcoreguidelines-owning-memory)
    qml_widget_->setResizeMode(QQuickWidget::SizeRootObjectToView);
    qml_widget_->rootContext()->setContextProperty("patternsModel", &model_);
    qml_widget_->setSource(QUrl("qrc:/qml/PatternsManagerWindow.qml"));
    layout->addWidget(qml_widget_);

    connect(&model_, &PatternsManagerModel::newRequested, this, &PatternsManagerWindow::create_pattern);
    connect(&model_, &PatternsManagerModel::removeRequested, this, &PatternsManagerWindow::delete_pattern);
    connect(&model_, &PatternsManagerModel::renameRequested, this, &PatternsManagerWindow::update_pattern);
    connect(&model_, &PatternsManagerModel::moveRequested, this, &PatternsManagerWindow::move_pattern);

    update_patterns();
}

auto PatternsManagerWindow::displayed_patterns() const -> std::vector<PerFileSettings *>
{
    std::vector<PerFileSettings *> result;
    for (auto *pfs : settings_.current_profile().per_file_settings())
        if (pfs->pattern.text() != k_default_pattern.text())
            result.push_back(pfs);

    return result;
}

void PatternsManagerWindow::update_patterns()
{
    std::vector<std::pair<QString, bool>> rows;
    for (auto *pfs : displayed_patterns())
        rows.emplace_back(to_qstring(pfs->pattern.text()), pfs->pattern.type() == Pattern::Type::Regex);

    model_.set_patterns(std::move(rows));
}

void PatternsManagerWindow::update_pattern(int row, const QString &new_text)
{
    auto patterns = displayed_patterns();
    if (row < 0 || static_cast<size_t>(row) >= patterns.size())
        return;

    auto *pfs               = patterns[static_cast<size_t>(row)];
    const auto old_pattern  = pfs->pattern.text();
    const auto new_pattern  = to_u8string(new_text);
    pfs->pattern            = Pattern{new_pattern, pfs->pattern.type()};

    if (settings_.gui.selected_pattern == old_pattern && old_pattern != new_pattern)
        settings_.gui.selected_pattern = new_pattern;

    update_patterns();
}

void PatternsManagerWindow::move_pattern(int source_row, int destination_row)
{
    settings_.current_profile().move_per_file_settings(source_row, destination_row);
    update_patterns();
}

void PatternsManagerWindow::create_pattern()
{
    bool ok = false;
    const QString &pat_name
        = QInputDialog::getText(this, tr("New Pattern"), tr("Name:"), QLineEdit::Normal, "", &ok);
    if (!ok || pat_name.isEmpty())
        return;

    const auto new_pfs_name = to_u8string(pat_name);

    // Check whether such pattern already exists.
    auto all_pfs = settings_.current_profile().per_file_settings();
    auto found   = std::ranges::find(all_pfs, new_pfs_name, [](const PerFileSettings *pfs) {
        return pfs->pattern.text();
    });

    if (found != all_pfs.end())
        return;

    // Choosing Pattern type

    QStringList type_list{"Glob", "Regex"};
    const QString type_str = QInputDialog::getItem(this,
                                                   tr("Pattern type"),
                                                   tr("What kind of a pattern would you like to create?"),
                                                   type_list,
                                                   0,
                                                   false,
                                                   &ok);
    if (!ok)
        return;

    Pattern::Type pattern_type;
    if (type_str == "Glob")
        pattern_type = Pattern::Type::Wildcard;
    else if (type_str == "Regex")
        pattern_type = Pattern::Type::Regex;
    else
        return;

    // Choosing base Pattern

    QStringList patterns_list;
    for (auto *pfs : all_pfs)
        patterns_list.push_back(to_qstring(pfs->pattern.text()));

    const QString base_pfs_qs = QInputDialog::getItem(this,
                                                      tr("Base pattern"),
                                                      tr("Which pattern do you want to use as a base?"),
                                                      patterns_list,
                                                      0,
                                                      false,
                                                      &ok);

    if (!ok)
        return;

    const auto base_pfs = to_u8string(base_pfs_qs);

    PerFileSettings new_pfs = **std::ranges::find(all_pfs, base_pfs, [](const PerFileSettings *pfs) {
        return pfs->pattern.text();
    });

    new_pfs.pattern = Pattern{new_pfs_name, pattern_type};

    settings_.current_profile().prepend_per_file_settings(std::move(new_pfs));
    update_patterns();
}

void PatternsManagerWindow::delete_pattern(int row)
{
    auto patterns = displayed_patterns();
    if (row < 0 || static_cast<size_t>(row) >= patterns.size())
        return;

    const auto current = to_qstring(patterns[static_cast<size_t>(row)]->pattern.text());

    // Ensure user cannot delete default pattern to avoid issues (defensive - displayed_patterns()
    // already excludes it, so this can't currently trigger, but matches the old code's intent).
    if (to_u8string(current) == k_default_pattern.text())
        return;

    const auto button = QMessageBox::warning(
        this,
        tr("Remove Pattern"),
        tr("Are you sure you want to remove Pattern '%1'? This action cannot be undone").arg(current),
        QMessageBox::No | QMessageBox::Yes);

    if (button != QMessageBox::Yes)
        return;

    // Ensure that the current selected pattern is correct to avoid crashes.
    if (current_per_file_settings(settings_).pattern.text() == to_u8string(current))
        settings_.gui.selected_pattern = k_default_pattern.text();

    settings_.current_profile().remove_per_file_settings(to_u8string(current));
    update_patterns();
}

} // namespace cao
