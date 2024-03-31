#pragma once

#include <optional>
#include <set>
#include <vector>

#include "common.h"
#include "formula.h"

bool HasCyrcle(const Position &pos, const Position &searching, const SheetInterface *sheet);

bool HasFormula(const std::string &text);

struct LinkedCells {
    std::vector<Position> affect_cells;
    std::set<Position> dependent_cells;
};

class Cell : public CellInterface {
   public:
    Cell() = default;
    Cell(const Position &pos, std::string text, SheetInterface *sheet);

    ~Cell() = default;

    void Set(std::string text);
    void SetPosition(const Position &pos);
    void SetSheet(SheetInterface *sheet);
    void SetCashe(double value) const;
    void SetDependentCell(const Position &pos);

    void Clear();

    CellInterface::Value GetValue() const override;
    std::optional<double> GetCashe() const;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;
    const SheetInterface *GetSheet() const;

    bool IsEmpty() const;
    bool IsFormula() const;

   private:
    CellInterface::Value value_;
    std::string expression_;
    bool is_empty_ = true;
    bool is_formula_ = false;
    Position cell_pos_;
    void ResetCash();
    mutable std::optional<double> cashe_;
    SheetInterface *sheet_;
    LinkedCells linked_cells_;
};