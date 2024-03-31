#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "cell.h"
#include "common.h"

struct Hasher {
    size_t operator()(const Position &pos) const {
        return (pos.col < pos.row) ? pos.row : pos.col + pos.MAX_COLS;
    }
};

class Sheet : public SheetInterface {
   public:
    Sheet() = default;
    ~Sheet() = default;

    void SetCell(Position pos, std::string text) override;

    const CellInterface *GetCell(Position pos) const override;
    CellInterface *GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream &output) const override;
    void PrintTexts(std::ostream &output) const override;

   private:
    Size size_;
    std::unordered_map<Position, std::unique_ptr<Cell>, Hasher> sheet_;
};