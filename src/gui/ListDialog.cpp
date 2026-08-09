/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "ListDialog.hpp"

#include <QInputDialog>
#include <QLineEdit>
#include <QQmlContext>
#include <QQuickWidget>
#include <QVBoxLayout>

namespace cao {
ListDialog::ListDialog(Sorting sort_by, QWidget *parent)
    : QDialog(parent)
    , model_(sort_by)
{
    setWindowTitle(tr("Dialog"));
    resize(347, 429);

    auto *layout = new QVBoxLayout(this); // NOLINT(cppcoreguidelines-owning-memory)

    qml_widget_ = new QQuickWidget(this); // NOLINT(cppcoreguidelines-owning-memory)
    qml_widget_->setResizeMode(QQuickWidget::SizeRootObjectToView);
    qml_widget_->rootContext()->setContextProperty("listModel", &model_);
    qml_widget_->rootContext()->setContextProperty("dialog", this);
    qml_widget_->setSource(QUrl("qrc:/qml/ListDialog.qml"));
    layout->addWidget(qml_widget_);

    connect(&model_, &ListDialogModel::addItemRequested, this, &ListDialog::add_user_item);
}

void ListDialog::set_user_add_item_visible(bool visible)
{
    model_.setAddItemVisible(visible);
}

void ListDialog::add_item(QListWidgetItem *item)
{
    model_.add_item(item);
}

void ListDialog::add_user_item()
{
    bool ok             = false;
    const QString &text = QInputDialog::getText(this, tr("New item"), tr("Name:"), QLineEdit::Normal, "", &ok);
    if (!ok || text.isEmpty())
        return;

    auto *item = new QListWidgetItem(text); // NOLINT(cppcoreguidelines-owning-memory)
    item->setCheckState(Qt::Checked);
    model_.add_item(item);
}

auto ListDialog::get_choices() -> std::vector<const QListWidgetItem *>
{
    return model_.get_choices();
}

auto ListDialog::items() -> std::vector<QListWidgetItem *>
{
    return model_.items();
}

void ListDialog::set_checked_items(const QStringList &text_list, bool add_missing_items)
{
    for (const QString &string : text_list)
        set_checked_items(string, add_missing_items);
}

void ListDialog::set_checked_items(const QString &text, bool add_missing_items)
{
    model_.set_checked_items(text, add_missing_items);
}

} // namespace cao
