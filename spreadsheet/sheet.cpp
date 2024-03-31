#include "sheet.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

#include "cell.h"
#include "common.h"

using namespace std::literals;

// void Sheet::SetCell(const Position &pos) {
//     sheet_[pos.col][pos.row] = std::make_unique<Cell>();
//     sheet_[pos.col][pos.row]->SetSheet(this);
//     sheet_[pos.col][pos.row]->SetPosition(pos);
// }

void Sheet::SetCell(Position pos, std::string text) {
    if (pos.IsValid()) {
        int width = pos.col + 1;
        int height = pos.row + 1;

        if (width > size_.cols) {
            sheet_.resize(width);
            for (int i = size_.cols; i < width; ++i) {
                sheet_[i].resize(height);
            }
            size_.cols = width;
        }
        if (height > size_.rows) {
            for (int i = 0; i < size_.cols; ++i) {
                sheet_[i].resize(height);
            }
            size_.rows = height;
        }
        if (sheet_[pos.col][pos.row].get()) {
            sheet_[pos.col][pos.row]->Set(text);
        } else {
            sheet_[pos.col][pos.row] = std::make_unique<Cell>();
            sheet_[pos.col][pos.row]->SetSheet(this);
            sheet_[pos.col][pos.row]->SetPosition(pos);
            sheet_[pos.col][pos.row]->Set(text);
        }
    } else {
        throw InvalidPositionException("Invalid position: " + pos.ToString());
    }
}

const CellInterface *Sheet::GetCell(Position pos) const {
    if (pos.IsValid()) {
        if (pos.row < size_.rows && pos.col < size_.cols) {
            return sheet_[pos.col][pos.row].get();
        }
        return nullptr;
    } else {
        throw InvalidPositionException("Invalid position: " + pos.ToString());
    }
}
CellInterface *Sheet::GetCell(Position pos) {
    if (pos.IsValid()) {
        if (pos.row < size_.rows && pos.col < size_.cols) {
            return sheet_[pos.col][pos.row].get();
        }
        return nullptr;
    } else {
        throw InvalidPositionException("Invalid position: " + pos.ToString());
    }
}

void Sheet::ClearCell(Position pos) {
    if (pos.IsValid()) {
        if (pos.row < size_.rows && pos.col < size_.cols) {
            sheet_[pos.col][pos.row].reset(nullptr);
            if (pos.col + 1 == size_.cols) {
                int rigth_bound = size_.cols - 1;
                while (rigth_bound > 0) {
                    bool is_empty = true;
                    for (int i = 0; i < size_.rows; ++i) {
                        if (sheet_[rigth_bound][i].get()) {
                            is_empty = false;
                            break;
                        }
                    }
                    if (!is_empty) {
                        break;
                    }
                    rigth_bound--;
                }
                size_.cols = sheet_[0][0].get() ? rigth_bound + 1 : 0;
            }

            if (pos.row + 1 == size_.rows) {
                int bottom_bound = size_.rows - 1;
                while (bottom_bound > 0) {
                    bool is_empty = true;
                    for (int i = 0; i < size_.cols; ++i) {
                        if (sheet_[i][bottom_bound].get()) {
                            is_empty = false;
                            break;
                        }
                    }
                    if (!is_empty) {
                        break;
                    }
                    bottom_bound--;
                }
                size_.rows = sheet_[0][0].get() ? bottom_bound + 1 : 0;
            }
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
            if (sheet_[y][x].get()) {
                std::visit(
                    [&](const auto &x) {
                        output << x;
                    },
                    sheet_[y][x]->GetValue());
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
            if (sheet_[y][x].get()) {
                output << sheet_[y][x]->GetText();
            }
            is_start = false;
        }
        output << '\n';
    }
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}