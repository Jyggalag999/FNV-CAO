/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "ListDialogModel.hpp"

#include <algorithm>

namespace cao {

ListDialogModel::ListDialogModel(Sorting sort_by, QObject *parent)
    : QAbstractListModel(parent)
    , sorting_(sort_by)
{
}

auto ListDialogModel::rowCount(const QModelIndex &parent) const -> int
{
    if (parent.isValid())
        return 0;

    return static_cast<int>(items_.size());
}

auto ListDialogModel::data(const QModelIndex &index, int role) const -> QVariant
{
    if (!index.isValid() || static_cast<size_t>(index.row()) >= items_.size())
        return {};

    const auto &item = *items_[static_cast<size_t>(index.row())];
    switch (role)
    {
        case TextRole: return item.text();
        case CheckedRole: return item.checkState() == Qt::Checked;
        case HiddenRole: return !item.text().contains(filter_text_, Qt::CaseInsensitive);
        default: return {};
    }
}

auto ListDialogModel::setData(const QModelIndex &index, const QVariant &value, int role) -> bool
{
    if (!index.isValid() || static_cast<size_t>(index.row()) >= items_.size())
        return false;

    if (role != CheckedRole)
        return false;

    auto &item           = *items_[static_cast<size_t>(index.row())];
    const auto new_state = value.toBool() ? Qt::Checked : Qt::Unchecked;
    if (item.checkState() == new_state)
        return true;

    item.setCheckState(new_state);
    sort_items();
    return true;
}

auto ListDialogModel::roleNames() const -> QHash<int, QByteArray>
{
    return {
        {TextRole, "text"},
        {CheckedRole, "checked"},
        {HiddenRole, "hidden"},
    };
}

void ListDialogModel::add_item(QListWidgetItem *item)
{
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);

    // Weird workaround to display the checkbox. It isn't displayed otherwise (inherited from the
    // old Widgets version - QListWidgetItem quirk, not ours).
    if (item->checkState() != Qt::Checked)
        item->setCheckState(Qt::Unchecked);

    beginResetModel();
    items_.push_back(std::unique_ptr<QListWidgetItem>(item));
    endResetModel();

    sort_items();
}

auto ListDialogModel::get_choices() const -> std::vector<const QListWidgetItem *>
{
    std::vector<const QListWidgetItem *> result;
    for (const auto &item : items_)
        if (item->checkState() == Qt::Checked)
            result.push_back(item.get());

    return result;
}

auto ListDialogModel::items() const -> std::vector<QListWidgetItem *>
{
    std::vector<QListWidgetItem *> result;
    result.reserve(items_.size());
    for (const auto &item : items_)
        result.push_back(item.get());

    return result;
}

void ListDialogModel::set_checked_items(const QString &text, bool add_missing_items)
{
    bool found = false;
    for (const auto &item : items_)
    {
        if (item->text().compare(text, Qt::CaseSensitive) == 0)
        {
            item->setCheckState(Qt::Checked);
            found = true;
        }
    }

    if (!found && add_missing_items)
    {
        auto *new_item = new QListWidgetItem(text); // NOLINT(cppcoreguidelines-owning-memory)
        new_item->setCheckState(Qt::Checked);
        add_item(new_item);
        return;
    }

    sort_items();
}

auto ListDialogModel::filterText() const -> QString
{
    return filter_text_;
}

void ListDialogModel::setFilterText(const QString &text)
{
    if (filter_text_ == text)
        return;

    filter_text_ = text;
    emit filterTextChanged();

    if (!items_.empty())
        emit dataChanged(index(0), index(static_cast<int>(items_.size()) - 1), {HiddenRole});
}

auto ListDialogModel::addItemVisible() const -> bool
{
    return add_item_visible_;
}

void ListDialogModel::setAddItemVisible(bool visible)
{
    if (add_item_visible_ == visible)
        return;

    add_item_visible_ = visible;
    emit addItemVisibleChanged();
}

void ListDialogModel::toggleChecked(int row)
{
    if (row < 0 || static_cast<size_t>(row) >= items_.size())
        return;

    const bool currently_checked = items_[static_cast<size_t>(row)]->checkState() == Qt::Checked;
    setData(index(row), !currently_checked, CheckedRole);
}

void ListDialogModel::requestAddItem()
{
    emit addItemRequested();
}

void ListDialogModel::sort_items()
{
    beginResetModel();
    std::stable_sort(items_.begin(), items_.end(), [this](const auto &a, const auto &b) {
        const bool a_checked = a->checkState() == Qt::Checked;
        const bool b_checked = b->checkState() == Qt::Checked;
        if (a_checked != b_checked)
            return a_checked;

        if (sorting_ == Sorting::Text)
            return a->text().compare(b->text(), Qt::CaseInsensitive) < 0;

        return false; // Insertion: stable_sort keeps original relative order within each group
    });
    endResetModel();
}

} // namespace cao
