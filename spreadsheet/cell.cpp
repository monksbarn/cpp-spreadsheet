#include "cell.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <optional>
#include <string>

bool HasCyrcle(const Position &pos, const Position &searching, const SheetInterface *sheet) {
    const Cell *cell = dynamic_cast<const Cell *>(sheet->GetCell(pos));
    if (cell) {
        for (const auto &item : cell->GetReferencedCells()) {
            if (item != searching) {
                return HasCyrcle(item, searching, sheet);
            } else {
                return true;
            }
        }
    }
    return false;
}

bool HasFormula(const std::string &text) {
    return ((text.front() == FORMULA_SIGN) && text.size() > 1);
}

Cell::Cell(const Position &pos, std::string text, SheetInterface *sheet) {
    SetPosition(pos);
    sheet_ = sheet;
    Set(text);
}

std::optional<double> Cell::GetCashe() const {
    return cashe_;
}

void Cell::SetPosition(const Position &pos) {
    cell_pos_.col = pos.col;
    cell_pos_.row = pos.row;
}

void Cell::Set(std::string text) {
    std::vector<Position> link_cell_tmp = std::move(linked_cells_.affect_cells);
    if (!text.empty()) {
        if (text.front() == ESCAPE_SIGN) {
            value_.emplace<std::string>(std::move(text));
        } else if (HasFormula(text)) {
            Formula formula(text.substr(1));
            linked_cells_.affect_cells = std::move(formula.GetReferencedCells());
            for (auto &pos : linked_cells_.affect_cells) {
                auto it = linked_cells_.dependent_cells.find(pos);
                if (pos == cell_pos_ || (it != linked_cells_.dependent_cells.end() && *it == pos) || HasCyrcle(pos, cell_pos_, sheet_)) {
                    linked_cells_.affect_cells = std::move(link_cell_tmp);
                    throw CircularDependencyException("CircularDependency: " + cell_pos_.ToString());
                }
            }
            for (auto &pos : linked_cells_.affect_cells) {
                Cell *cell_ptr = dynamic_cast<Cell *>(sheet_->GetCell(pos));
                if (!cell_ptr) {
                    sheet_->SetCell(pos, "");
                    cell_ptr = dynamic_cast<Cell *>(sheet_->GetCell(pos));
                }
                cell_ptr->SetDependentCell(cell_pos_);
            }
            is_formula_ = true;
            expression_ = formula.GetExpression();
            ResetCash();
            FormulaInterface::Value val = formula.Evaluate(*sheet_);

            if (std::holds_alternative<double>(val)) {
                value_.emplace<double>(std::get<double>(val));
            } else {
                value_.emplace<FormulaError>(std::get<FormulaError>(val));
            }
        } else {
            value_.emplace<std::string>(std::move(text));
        }
    }
    is_empty_ = false;
}

void Cell::SetSheet(SheetInterface *sheet) {
    sheet_ = sheet;
}

void Cell::Clear() {
    if (!is_empty_) {
        if (std::holds_alternative<std::string>(value_)) {
            std::get<std::string>(value_).clear();
        }
        expression_.clear();
        is_empty_ = true;
        ResetCash();
        linked_cells_.affect_cells.clear();
        linked_cells_.dependent_cells.clear();
    }
}

Cell::Value Cell::GetValue() const {
    Value result;
    if (!is_empty_) {
        if (std::holds_alternative<double>(value_)) {
            result.emplace<double>(std::get<double>(value_));
        } else if (std::holds_alternative<std::string>(value_)) {
            std::string text = std::get<std::string>(value_);
            if (!text.empty() && text.front() == ESCAPE_SIGN) {
                result.emplace<std::string>(std::get<std::string>(value_).substr(1));
            } else {
                result.emplace<std::string>(std::get<std::string>(value_));
            }
        } else {
            result.emplace<FormulaError>(std::get<FormulaError>(value_));
        }
    }
    return result;
}
std::string Cell::GetText() const {
    std::string result;
    if (!is_empty_) {
        if (std::holds_alternative<double>(value_) || std::holds_alternative<FormulaError>(value_)) {
            result = "=" + expression_;
        } else {
            result = std::get<std::string>(value_);
        }
    }
    return result;
}

std::vector<Position> Cell::GetReferencedCells() const {
    // std::vector<Position> result = linked_cells_.affect_cells;
    // for (auto &pos : linked_cells_.dependent_cells) {
    //     result.push_back(pos);
    // }
    // std::sort(result.begin(), result.end());
    // result.erase(std::unique(result.begin(), result.end()), result.end());
    // return result;
    return linked_cells_.affect_cells;
}

void Cell::SetCashe(double value) const {
    cashe_.emplace(value);
}

void Cell::SetDependentCell(const Position &pos) {
    linked_cells_.dependent_cells.insert(pos);
}

bool Cell::IsEmpty() const {
    return is_empty_;
}

bool Cell::IsFormula() const {
    return is_formula_;
}

void Cell::ResetCash() {
    cashe_.reset();
    for (auto &pos : linked_cells_.dependent_cells) {
        dynamic_cast<Cell *>(sheet_->GetCell(pos))->ResetCash();
    }
}

const SheetInterface *Cell::GetSheet() const {
    return sheet_;
}
