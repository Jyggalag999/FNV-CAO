/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <QAbstractListModel>
#include <QListWidgetItem>
#include <QString>

#include <memory>
#include <vector>

namespace cao {

/// @brief Owns ListDialog's item data and exposes it to QML's ListView. QListWidgetItem stays
/// the storage currency (ListDialog's public API - add_item/get_choices/items - is unchanged and
/// still deals in QListWidgetItem*), it's just no longer parented to a live QListWidget: this
/// model owns the items directly and is the single source of truth for both the C++ API and the
/// QML-visible rows.
///
/// Ordering is a single stable_sort comparator (checked items first, then by Sorting), re-run
/// after every mutation, rather than the old Widgets version's incremental single-item
/// find_insert_pos. Two behavioral notes from that rewrite, both with zero observable effect
/// today since ListDialog has no live callers (see AdvancedTexturesModule's unused
/// texture_format_dialog_ member):
///  - Sorting::Text now genuinely sorts by item text. The old find_insert_pos compared
///    `item > other` on two `const QListWidgetItem *` - pointer/address comparison, not text -
///    so "Text" sorting never actually sorted alphabetically. Fixed, not replicated.
///  - Within a checked/unchecked group, order is now insertion order (stable_sort's guarantee)
///    rather than the old code's "prepend within group" quirk, which looked like an artifact of
///    its single-item-insertion algorithm rather than an intentional design choice.
class ListDialogModel final : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString filterText READ filterText WRITE setFilterText NOTIFY filterTextChanged)
    Q_PROPERTY(bool addItemVisible READ addItemVisible WRITE setAddItemVisible NOTIFY
                   addItemVisibleChanged)

public:
    enum class Sorting : std::uint8_t
    {
        Text,
        Insertion
    };

    enum Roles : int
    {
        TextRole = Qt::UserRole + 1,
        CheckedRole,
        HiddenRole,
    };

    explicit ListDialogModel(Sorting sort_by, QObject *parent = nullptr);

    [[nodiscard]] auto rowCount(const QModelIndex &parent = {}) const -> int override;
    [[nodiscard]] auto data(const QModelIndex &index, int role) const -> QVariant override;
    auto setData(const QModelIndex &index, const QVariant &value, int role) -> bool override;
    [[nodiscard]] auto roleNames() const -> QHash<int, QByteArray> override;

    /// Takes ownership of item, same as the old QListWidget::insertItem did.
    void add_item(QListWidgetItem *item);

    [[nodiscard]] auto get_choices() const -> std::vector<const QListWidgetItem *>;
    [[nodiscard]] auto items() const -> std::vector<QListWidgetItem *>;

    void set_checked_items(const QString &text, bool add_missing_items);

    [[nodiscard]] auto filterText() const -> QString;
    void setFilterText(const QString &text);

    [[nodiscard]] auto addItemVisible() const -> bool;
    void setAddItemVisible(bool visible);

    /// Re-checks (and re-sorts) the item at `row`, or unchecks it - mirrors the old itemChanged
    /// handler that kept checked items at the top.
    Q_INVOKABLE void toggleChecked(int row);

    /// QML's "Add item" button asks ListDialog (which owns the QInputDialog prompt - a plain
    /// utility dialog, not worth reimplementing in QML) to add a new item.
    Q_INVOKABLE void requestAddItem();

signals:
    void filterTextChanged();
    void addItemVisibleChanged();
    void addItemRequested();

private:
    /// Stable-sorts items_ (checked first, then by Sorting) and wraps the reorder in a model
    /// reset. Call after any mutation that can affect ordering.
    void sort_items();

    std::vector<std::unique_ptr<QListWidgetItem>> items_;
    Sorting sorting_;
    QString filter_text_;
    bool add_item_visible_ = true;
};

} // namespace cao
