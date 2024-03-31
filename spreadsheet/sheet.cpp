#include "sheet.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

#include "cell.h"
#include "common.h"

using namespace std::literals;

void Sheet::SetCell(Position pos, std::string text) {
    if (pos.IsValid()) {
        size_.cols = std::max(size_.cols, pos.col + 1);
        size_.rows = std::max(size_.rows, pos.row + 1);
        if (auto it = sheet_.find(pos); it == sheet_.end()) {
            sheet_[pos] = std::make_unique<Cell>();
        }
        sheet_[pos]->SetSheet(this);
        sheet_[pos]->SetPosition(pos);
        sheet_[pos]->Set(text);

    } else {
        throw InvalidPositionException("Invalid position: " + pos.ToString());
    }
}

const CellInterface *Sheet::GetCell(Position pos) const {
    if (pos.IsValid()) {
        if (auto it = sheet_.find(pos); it != sheet_.end()) {
            return it->second.get();
        }
        return nullptr;
    } else {
        throw InvalidPositionException("Invalid position: " + pos.ToString());
    }
}
CellInterface *Sheet::GetCell(Position pos) {
    if (pos.IsValid()) {
        if (auto it = sheet_.find(pos); it != sheet_.end()) {
            return it->second.get();
        }
        return nullptr;
    } else {
        throw InvalidPositionException("Invalid position: " + pos.ToString());
    }
}

void Sheet::ClearCell(Position pos) {
    if (pos.IsValid()) {
        if (auto it = sheet_.find(pos); it != sheet_.end()) {
            it->second->Clear();
            sheet_.erase(it);
        }
        size_.rows = 0;
        size_.cols = 0;
        for (const auto &[pos, cell] : sheet_) {
            size_.cols = std::max(size_.cols, pos.col + 1);
            size_.rows = std::max(size_.rows, pos.row + 1);
        }
    } else {
        throw InvalidPositionException("The position is not allowed");
    }
}

Size Sheet::GetPrintableSize() const {
    return size_;
}
void Sheet::PrintValues(std::ostream &output) const {
    for (int x = 0; x < size_.rows; ++x) {
        bool is_start = true;
        for (int y = 0; y < size_.cols; ++y) {
            if (!is_start) {
                output << '\t';
            }
            Position pos{x, y};
            if (auto it = sheet_.find(pos); it != sheet_.end()) {
                std::visit([&](const auto &x) {
                    output << x;
                },
                           it->second->GetValue());
            }
            is_start = false;
        }
        output << '\n';
    }
}
void Sheet::PrintTexts(std::ostream &output) const {
    for (int x = 0; x < size_.rows; ++x) {
        bool is_start = true;
        for (int y = 0; y < size_.cols; ++y) {
            if (!is_start) {
                output << '\t';
            }
            Position pos{x, y};
            if (auto it = sheet_.find(pos); it != sheet_.end()) {
                output << it->second->GetText();
            }
            is_start = false;
        }
        output << '\n';
    }
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}